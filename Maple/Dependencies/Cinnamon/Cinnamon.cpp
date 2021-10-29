#include "Cinnamon.h"

#include <TlHelp32.h>
#include <hde32.h>

LONG __stdcall Cinnamon::hwbrkHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		for (const auto& hook : hooks)
		{
			if (hook->Type == HookType::HardwareBreakpoint)
			{
				HardwareBreakpointHook* hwbpHook = static_cast<HardwareBreakpointHook*>(hook);
				if (hwbpHook->FunctionAddress == reinterpret_cast<void*>(ExceptionInfo->ContextRecord->Eip))
				{
					ExceptionInfo->ContextRecord->Eip = reinterpret_cast<DWORD>(hwbpHook->HookFunctionAddress);
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}
	}
	
	return EXCEPTION_CONTINUE_SEARCH;
}

void* __fastcall Cinnamon::hkBaseThreadInitThunk(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter)
{
	HANDLE	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
	if (hThread)
	{
		for (const auto& hook : hooks)
		{
			if (hook->Type == HookType::HardwareBreakpoint)
			{
				HardwareBreakpointHook* hwbpHook = static_cast<HardwareBreakpointHook*>(hook);
				installHWBP(hwbpHook->FunctionAddress, hwbpHook->ThreadHandle, hThread);
			}
		}
		
		CloseHandle(hThread);
	}

	return m_oBaseThreadInitThunk(Unknown, StartAddress, ThreadParameter);
}

Hook* Cinnamon::findHook(std::string name)
{
	for (const auto& hook : hooks)
		if (hook->Name == name)
			return hook;
	
	return nullptr;
}

void Cinnamon::removeHook(std::string name)
{
	for (size_t i = 0; i < hooks.size(); i++)
	{
		if (hooks[i]->Name == name)
		{
			hooks.erase(hooks.begin() + i);
			
			break;
		}
	}
}

unsigned int Cinnamon::getInstructionOffsetInBytes(void* functionAddress, unsigned int offset)
{
	hde32s hde;
	
	unsigned int length = 0;
	for (unsigned int i = 0; i < offset; i++)
		length += hde32_disasm(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(functionAddress) + length), &hde);
	
	return length;
}

std::vector<unsigned char> Cinnamon::getPrologue(void* functionAddress, unsigned int offset)
{
	hde32s hde;

	unsigned int prologueLength = offset;
	while (prologueLength < 5 + offset)
		prologueLength += hde32_disasm(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(functionAddress) + prologueLength), &hde);
	
	std::vector<unsigned char> originalBytes;
	for (unsigned int i = 0; i < prologueLength; i++)
		originalBytes.push_back(*reinterpret_cast<unsigned char*>(reinterpret_cast<DWORD>(functionAddress) + i));

	return originalBytes;
}

void Cinnamon::relocateRelativeAddresses(void* oldLocation, void* newLocation, unsigned int length)
{
	hde32s hde;
	unsigned int currentLength = 0;
	while (currentLength != length)
	{
		const unsigned int instructionLength = hde32_disasm(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(newLocation) + currentLength), &hde);
		if (hde.opcode == 0xE9) //relative jmp
		{
			const DWORD destinationAddress = reinterpret_cast<uintptr_t>(oldLocation) + currentLength + instructionLength + *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(newLocation) + currentLength + 0x1);
			void* relocatedDestinationAddress = reinterpret_cast<void*>(destinationAddress - (reinterpret_cast<DWORD>(newLocation) + currentLength + instructionLength));

			*reinterpret_cast<void**>(reinterpret_cast<DWORD>(newLocation) + currentLength + 0x1) = relocatedDestinationAddress;
		}
		//todo: add support for call instruction and other jmp variants

		currentLength += instructionLength;
	}
}

void* Cinnamon::createCodeCave(void* functionAddress, std::vector<unsigned char> prologue)
{
	void* codeCaveAddress = VirtualAlloc(nullptr, prologue.size() + 0x5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	for (size_t i = 0; i < prologue.size(); i++)
		*reinterpret_cast<unsigned char*>(reinterpret_cast<DWORD>(codeCaveAddress) + i) = prologue[i];

	relocateRelativeAddresses(functionAddress, codeCaveAddress, prologue.size());

	*reinterpret_cast<unsigned char*>(reinterpret_cast<DWORD>(codeCaveAddress) + prologue.size()) = 0xE9;

	void* relativeReturnAddress = reinterpret_cast<void*>((reinterpret_cast<DWORD>(functionAddress) + prologue.size()) - (reinterpret_cast<DWORD>(codeCaveAddress) + prologue.size()) - 0x5);
	*reinterpret_cast<void**>(reinterpret_cast<DWORD>(codeCaveAddress) + prologue.size() + 0x1) = relativeReturnAddress;

	return codeCaveAddress;
}

CinnamonResult Cinnamon::installHWBP(void* functionAddress, HANDLE hookThread, HANDLE hThread)
{
	HANDLE thread = hThread == nullptr ? hookThread : hThread;
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (!GetThreadContext(thread, &context))
	{
		CloseHandle(thread);
		return CinnamonResult::GetThreadContextFailed;
	}

	for (int i = 0; i < 4; i++)
		if (*(reinterpret_cast<size_t*>(&context.Dr0) + i) == reinterpret_cast<size_t>(functionAddress))
			return CinnamonResult::HookAlreadyInstalled;

	int index = -1;
	if (!(context.Dr7 & 1))
		index = 0;
	else if (!(context.Dr7 & 4))
		index = 1;
	else if (!(context.Dr7 & 16))
		index = 2;
	else if (!(context.Dr7 & 64))
		index = 3;

	if (index < 0)
		return CinnamonResult::NoFreeIndex;

	context.Dr7 |= 1 << (2 * index) | 0x100;
	*(reinterpret_cast<size_t*>(&context.Dr0) + index) = reinterpret_cast<size_t>(functionAddress);

	DWORD suspendCount = 0xdeadc0de;
	if (GetThreadId(thread) != GetCurrentThreadId())
		suspendCount = SuspendThread(hookThread);

	if (!SetThreadContext(thread, &context))
		return CinnamonResult::SetThreadContextFailed;

	if (suspendCount <= 0)
		ResumeThread(thread);

	return CinnamonResult::Success;
}

void* Cinnamon::installTrampoline(void* functionAddress, void* hookFunctionAddress, std::vector<unsigned char> prologue, unsigned int offset)
{
	void* codeCaveAddress = createCodeCave(functionAddress, prologue);

	void* relativeAddress = reinterpret_cast<void*>(reinterpret_cast<DWORD>(hookFunctionAddress) - reinterpret_cast<DWORD>(functionAddress) - offset - 0x5);

	DWORD oldProtect;
	VirtualProtect(functionAddress, prologue.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

	for (unsigned int i = 0; i < prologue.size(); i++)
		*reinterpret_cast<unsigned char*>(reinterpret_cast<DWORD>(functionAddress) + i) = 0x90;
	
	*reinterpret_cast<unsigned char*>(reinterpret_cast<DWORD>(functionAddress) + offset) = 0xE9;
	*reinterpret_cast<void**>(reinterpret_cast<DWORD>(functionAddress) + offset + 0x1) = relativeAddress;

	VirtualProtect(functionAddress, prologue.size(), oldProtect, &oldProtect);

	return codeCaveAddress;
}

CinnamonResult Cinnamon::InstallHook(std::string name, void* functionAddress, void* hookFunctionAddress, void** originalFunction, HookType type)
{
	if (findHook(name) != nullptr)
		return CinnamonResult::HookAlreadyInstalled;

	const unsigned int offset = type == HookType::UndetectedByteCodePatch ? getInstructionOffsetInBytes(functionAddress, 1) : 0;

	const std::vector<unsigned char> prologue = getPrologue(functionAddress, offset);
	
	switch (type)
	{
		case HookType::UndetectedByteCodePatch:
		case HookType::ByteCodePatch:
		{
			void* codeCaveAddress = installTrampoline(functionAddress, hookFunctionAddress, prologue, offset);

			*originalFunction = codeCaveAddress;

			hooks.push_back(new ByteCodePatchHook(name, functionAddress, codeCaveAddress, prologue));

			return CinnamonResult::Success;
		}
		case HookType::HardwareBreakpoint:
		{
			if (exceptionHandlerHandle == reinterpret_cast<PVOID>(0x00DE00FF))
				exceptionHandlerHandle = AddVectoredExceptionHandler(TRUE, hwbrkHandler);

			if (findHook("BaseThreadInitThunk") == nullptr)
			{
				void* baseThreadInitThunkPointer = GetProcAddress(GetModuleHandleA("kernel32.dll"), "BaseThreadInitThunk");
				InstallHook("BaseThreadInitThunk", baseThreadInitThunkPointer, hkBaseThreadInitThunk, reinterpret_cast<void**>(&m_oBaseThreadInitThunk));
			}

			if (m_oBaseThreadInitThunk == reinterpret_cast<fnBaseThreadInitThunk>(0x00000000))
				return CinnamonResult::BaseThreadInitThunkFailed;

			DWORD processId = GetCurrentProcessId();
			HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (h != INVALID_HANDLE_VALUE)
			{
				THREADENTRY32 te;
				te.dwSize = sizeof(te);
				if (Thread32First(h, &te))
				{
					do
					{
						if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID) && te.th32OwnerProcessID == processId)
						{
							if (te.th32ThreadID == GetCurrentThreadId())
								continue;

							HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);

							if (hThread)
							{
								CinnamonResult hwbpResult = installHWBP(functionAddress, hThread);
								if (hwbpResult == CinnamonResult::Success)
								{
									void* codeCaveAddress = createCodeCave(functionAddress, prologue);
									*originalFunction = codeCaveAddress;
									
									hooks.push_back(new HardwareBreakpointHook(name, functionAddress, hookFunctionAddress, codeCaveAddress, prologue, hThread));
								}

								CloseHandle(h);
								CloseHandle(hThread);
								
								return hwbpResult;
							}
						}
						te.dwSize = sizeof(te);
					} while (Thread32Next(h, &te));
				}

				CloseHandle(h);
			}

			return CinnamonResult::CreateToolhelp32SnapshotFailed;
		}
	}

	return CinnamonResult::UnknownHookType;
}

CinnamonResult Cinnamon::UninstallHook(std::string name)
{
	Hook* hook = findHook(name);
	if (hook == nullptr)
		return CinnamonResult::HookNotFound;

	switch (hook->Type)
	{
		case HookType::ByteCodePatch:
		{
			for (size_t i = 0; i < hook->Prologue.size(); i++)
				*reinterpret_cast<unsigned char*>((reinterpret_cast<DWORD>(hook->FunctionAddress) + i)) = hook->Prologue[i];

			VirtualFree(hook->CodeCaveAddress, 0, MEM_RELEASE);

			removeHook(name);

			return CinnamonResult::Success;
		}
		case HookType::HardwareBreakpoint:
		{
			HardwareBreakpointHook* hwbpHook = static_cast<HardwareBreakpointHook*>(hook);
				
			CONTEXT context = { 0 };
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			if (!GetThreadContext(hwbpHook->ThreadHandle, &context))
			{
				CloseHandle(hwbpHook->ThreadHandle);
				return CinnamonResult::GetThreadContextFailed;
			}

			bool found = false;
			for (int i = 0; i < 4; i++)
				if (*(reinterpret_cast<size_t*>(&context.Dr0) + i) == reinterpret_cast<size_t>(hwbpHook->HookFunctionAddress))
					found = true;
				
			if (!found)
				return CinnamonResult::HookNotFound;
				
			for (int i = 0; i < 4; i++)
			{
				if (context.Dr7 == 1 << (2 * i) | 0x100)
					context.Dr7 |= 1 << (2 * i) | 0x100;
				*(reinterpret_cast<size_t*>(&context.Dr0) + i) = static_cast<size_t>(0x00000000);
			}
				
			DWORD suspendCount = 0xdeadc0de;
			if (GetThreadId(hwbpHook->ThreadHandle) != GetCurrentThreadId())
				suspendCount = SuspendThread(hwbpHook->ThreadHandle);
				
			if (!SetThreadContext(hwbpHook->ThreadHandle, &context))
				return CinnamonResult::SetThreadContextFailed;
				
			if (suspendCount <= 0)
				ResumeThread(hwbpHook->ThreadHandle);

			VirtualFree(hwbpHook->CodeCaveAddress, 0, MEM_RELEASE);

			removeHook(name);

			return CinnamonResult::Success;
		}
	}
	
	return CinnamonResult::UnknownHookType;
}

void Cinnamon::UninstallAllHooks()
{
	for (const auto& hook : hooks)
		UninstallHook(hook->Name);
}
