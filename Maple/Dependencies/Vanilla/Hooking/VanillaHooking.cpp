#include "VanillaHooking.h"

#include <TlHelp32.h>

#include <hde32.h>

VanillaHook* VanillaHooking::findHook(const std::string& name)
{
	for (const auto& hook : hooks)
		if (hook->Name == name)
			return hook;

	return nullptr;
}

void VanillaHooking::removeHook(const std::string& name)
{
	for (size_t i = 0; i < hooks.size(); i++)
	{
		if (hooks[i]->Name == name)
		{
			delete hooks[i];
			hooks.erase(hooks.begin() + i);

			break;
		}
	}
}

LONG __stdcall VanillaHooking::hwbrkHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		for (const auto& hook : hooks)
		{
			if (hook->Type == VanillaHookType::HardwareBreakpoint)
			{
				VanillaHardwareBreakpointHook* hwbpHook = static_cast<VanillaHardwareBreakpointHook*>(hook);
				if (hwbpHook->FunctionAddress == static_cast<uintptr_t>(ExceptionInfo->ContextRecord->Eip))
				{
					ExceptionInfo->ContextRecord->Eip = static_cast<DWORD>(hwbpHook->DetourFunctionAddress);
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void* __fastcall VanillaHooking::hkBaseThreadInitThunk(ULONG Unknown, PVOID StartAddress, PVOID ThreadParameter)
{
	if (const HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId()))
	{
		for (const auto& hook : hooks)
		{
			if (hook->Type == VanillaHookType::HardwareBreakpoint)
			{
				VanillaHardwareBreakpointHook* hwbpHook = static_cast<VanillaHardwareBreakpointHook*>(hook);
				installHWBPHook(hwbpHook->FunctionAddress, hwbpHook->ThreadHandle, hThread);
			}
		}

		CloseHandle(hThread);
	}

	return m_oBaseThreadInitThunk(Unknown, StartAddress, ThreadParameter);
}

VanillaResult VanillaHooking::installHWBPHook(uintptr_t functionAddress, HANDLE hookThread, HANDLE hThread)
{
	const HANDLE thread = hThread == nullptr ? hookThread : hThread;
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (!GetThreadContext(thread, &context))
	{
		CloseHandle(thread);
		return VanillaResult::GetThreadContextFailed;
	}

	for (int i = 0; i < 4; i++)
		if (*(reinterpret_cast<size_t*>(&context.Dr0) + i) == functionAddress)
			return VanillaResult::HookAlreadyInstalled;

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
		return VanillaResult::NoFreeIndex;

	context.Dr7 |= 1 << (2 * index) | 0x100;
	*(reinterpret_cast<size_t*>(&context.Dr0) + index) = functionAddress;

	DWORD suspendCount = 0xdeadc0de;
	if (GetThreadId(thread) != GetCurrentThreadId())
		suspendCount = SuspendThread(hookThread);

	if (!SetThreadContext(thread, &context))
		return VanillaResult::SetThreadContextFailed;

	if (suspendCount <= 0)
		ResumeThread(thread);

	return VanillaResult::Success;
}

std::vector<uint8_t> VanillaHooking::getFunctionPrologue(uintptr_t functionAddress, unsigned int minimumBytes)
{
	hde32s hde;

	unsigned int functionPrologueLength = 0;
	while (functionPrologueLength < minimumBytes)
		functionPrologueLength += hde32_disasm(reinterpret_cast<void*>(functionAddress + functionPrologueLength), &hde);

	std::vector<uint8_t> functionPrologue;
	for (unsigned int i = 0; i < functionPrologueLength; i++)
		functionPrologue.push_back(*reinterpret_cast<uint8_t*>(functionAddress + i));

	return functionPrologue;
}

uintptr_t VanillaHooking::installTrampoline(uintptr_t functionAddress, const std::vector<uint8_t>& functionPrologue)
{
	const uintptr_t trampolineAddress = reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr, functionPrologue.size() + 0x5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	for (size_t i = 0; i < functionPrologue.size(); i++)
		*reinterpret_cast<uint8_t*>(trampolineAddress + i) = functionPrologue[i];

	relocateRelativeAddresses(functionAddress, trampolineAddress, functionPrologue.size());

	*reinterpret_cast<uint8_t*>(trampolineAddress + functionPrologue.size()) = 0xE9;

	const intptr_t relativeReturnAddress = (static_cast<intptr_t>(functionAddress) + functionPrologue.size()) - (static_cast<intptr_t>(trampolineAddress) + functionPrologue.size() + 0x5);
	*reinterpret_cast<intptr_t*>(trampolineAddress + functionPrologue.size() + 0x1) = relativeReturnAddress;

	return trampolineAddress;
}

void VanillaHooking::relocateRelativeAddresses(uintptr_t oldLocation, uintptr_t newLocation, unsigned int length)
{
	hde32s hde;
	unsigned int currentLength = 0;
	while (currentLength != length)
	{
		const unsigned int instructionLength = hde32_disasm(reinterpret_cast<void*>(newLocation + currentLength), &hde);
		if (hde.opcode == 0xE9) //relative jmp
		{
			const intptr_t destinationAddress = static_cast<intptr_t>(oldLocation) + currentLength + instructionLength + *reinterpret_cast<int*>(newLocation + currentLength + 0x1);
			const intptr_t relocatedDestinationAddress = destinationAddress - (static_cast<intptr_t>(newLocation) + currentLength + instructionLength);

			*reinterpret_cast<intptr_t*>(newLocation + currentLength + 0x1) = relocatedDestinationAddress;
		}
		//todo: add support for call instruction and other jmp variants

		currentLength += instructionLength;
	}
}

uintptr_t VanillaHooking::installInlineHook(uintptr_t functionAddress, uintptr_t detourFunctionAddress, const std::vector<uint8_t>& functionPrologue)
{
	const uintptr_t trampolineAddress = installTrampoline(functionAddress, functionPrologue);

	const intptr_t relativeAddress = static_cast<intptr_t>(detourFunctionAddress) - (static_cast<intptr_t>(functionAddress) + functionPrologue.size());

	DWORD oldProtect;
	VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), functionPrologue.size(), PAGE_EXECUTE_READWRITE, &oldProtect);

	for (unsigned int i = 0; i < functionPrologue.size(); i++)
		*reinterpret_cast<uint8_t*>(functionAddress + i) = 0x90;

	*reinterpret_cast<uint8_t*>(functionAddress + functionPrologue.size() - 0x5) = 0xE9;
	*reinterpret_cast<intptr_t*>(functionAddress + functionPrologue.size() - 0x4) = relativeAddress;

	VirtualProtect(reinterpret_cast<LPVOID>(functionAddress), functionPrologue.size(), oldProtect, &oldProtect);

	return trampolineAddress;
}

VanillaResult VanillaHooking::InstallHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourFunctionAddress, uintptr_t* originalFunction, VanillaHookType type)
{
	if (findHook(name))
		return VanillaResult::HookAlreadyInstalled;
	
	const std::vector<uint8_t> functionPrologue = getFunctionPrologue(functionAddress, type == VanillaHookType::UndetectedInline ? 6 : 5);

	switch (type)
	{
		case VanillaHookType::Inline:
		case VanillaHookType::UndetectedInline:
		{
			const uintptr_t trampolineAddress = installInlineHook(functionAddress, detourFunctionAddress, functionPrologue);

			*originalFunction = trampolineAddress;

			hooks.push_back(new VanillaInlineHook(name, functionAddress, trampolineAddress, functionPrologue));

			return VanillaResult::Success;
		}
		case VanillaHookType::HardwareBreakpoint:
		{
			if (exceptionHandlerHandle == reinterpret_cast<PVOID>(0x00DE00FF))
				exceptionHandlerHandle = AddVectoredExceptionHandler(FALSE, hwbrkHandler);

			if (!findHook("BaseThreadInitThunk"))
			{
				void* baseThreadInitThunkPointer = GetProcAddress(GetModuleHandleA("kernel32.dll"), "BaseThreadInitThunk");
				InstallHook("BaseThreadInitThunk", reinterpret_cast<uintptr_t>(baseThreadInitThunkPointer), reinterpret_cast<uintptr_t>(hkBaseThreadInitThunk), reinterpret_cast<uintptr_t*>(&m_oBaseThreadInitThunk));
			}

			if (m_oBaseThreadInitThunk == reinterpret_cast<fnBaseThreadInitThunk>(0x00000000))
				return VanillaResult::BaseThreadInitThunkFailed;

			const DWORD processId = GetCurrentProcessId();
			const HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
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
								VanillaResult hwbpResult = installHWBPHook(functionAddress, hThread);
								if (hwbpResult == VanillaResult::Success)
								{
									const uintptr_t trampolineAddress = installTrampoline(functionAddress, functionPrologue);
									*originalFunction = trampolineAddress;

									hooks.push_back(new VanillaHardwareBreakpointHook(name, functionAddress, detourFunctionAddress, trampolineAddress, functionPrologue, hThread));
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

			return VanillaResult::CreateToolhelp32SnapshotFailed;
		}
	}

	return VanillaResult::UnknownHookType;
}

VanillaResult VanillaHooking::UninstallHook(const std::string& name)
{
	VanillaHook* hook = findHook(name);
	if (hook)
		return VanillaResult::HookNotInstalled;

	switch (hook->Type)
	{
		case VanillaHookType::Inline:
		case VanillaHookType::UndetectedInline:
		{
			for (size_t i = 0; i < hook->FunctionPrologue.size(); i++)
				*reinterpret_cast<uint8_t*>(hook->FunctionAddress + i) = hook->FunctionPrologue[i];

			VirtualFree(reinterpret_cast<LPVOID>(hook->TrampolineAddress), 0, MEM_RELEASE);

			removeHook(name);

			return VanillaResult::Success;
		}
		case VanillaHookType::HardwareBreakpoint:
		{
			VanillaHardwareBreakpointHook* hwbpHook = static_cast<VanillaHardwareBreakpointHook*>(hook);

			CONTEXT context = { 0 };
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			if (!GetThreadContext(hwbpHook->ThreadHandle, &context))
			{
				CloseHandle(hwbpHook->ThreadHandle);
				return VanillaResult::GetThreadContextFailed;
			}

			bool found = false;
			for (int i = 0; i < 4; i++)
				if (*(reinterpret_cast<size_t*>(&context.Dr0) + i) == hwbpHook->DetourFunctionAddress)
					found = true;

			if (!found)
				return VanillaResult::HookNotInstalled;

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
				return VanillaResult::SetThreadContextFailed;

			if (suspendCount <= 0)
				ResumeThread(hwbpHook->ThreadHandle);

			VirtualFree(reinterpret_cast<LPVOID>(hwbpHook->TrampolineAddress), 0, MEM_RELEASE);

			removeHook(name);

			return VanillaResult::Success;
		}
	}

	return VanillaResult::UnknownHookType;
}

void VanillaHooking::UninstallAllHooks()
{
	for (const auto& hook : hooks)
		UninstallHook(hook->Name);
}
