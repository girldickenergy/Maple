#pragma once

#include "../../Vanilla.h"

class JitHelpers
{
	//currently unused but may be required later
	//typedef void(__thiscall* reset_t)(void* pMethodDesc);
	//static inline reset_t reset = reinterpret_cast<reset_t>(reinterpret_cast<uintptr_t>(GetModuleHandle(L"clr.dll")) + 0x3367EA);

	typedef void* (__thiscall* doPreStub_t)(void* pMethodDesc, void* pDispatchingMT);
	static inline doPreStub_t doPreStub = nullptr;

	typedef void* (__thiscall* getWrappedMethodDesc_t)(void* pMethodDesc);
	static inline getWrappedMethodDesc_t getWrappedMethodDesc = nullptr;

	typedef void* (__thiscall* getNativeCode_t)(void* pMethodDesc);
	static inline getNativeCode_t getNativeCode = reinterpret_cast<getNativeCode_t>(Vanilla::FindSignature("\x56\x8B\xF1\x83\x3D\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x8A\x46\x03\xA8\x08", "xxxxx?????xx????xxxxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll")));

	static bool hasPrecode(void* pMethodDesc)
	{
		return (*(reinterpret_cast<byte*>(pMethodDesc) + 3) & 0x2) != 0;
	}

	static bool isUnboxingStub(void* pMethodDesc)
	{
		return (*(reinterpret_cast<byte*>(pMethodDesc) + 3) & 0x4) != 0;
	}
public:
	static void CompilePrecode(void* precodePointer, void* pMethodDesc)
	{
		if (!doPreStub)
		{
			uintptr_t result = Vanilla::FindSignature("\x6A\x00\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xD9\x33\x00\x89\x00\xD4", "x?x????x????xxx?x?x", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll"));
			if (!result)
				result = Vanilla::FindSignature("\x6A\x40\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\xD9\x33", "xxx????x????xxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll"));

			doPreStub = reinterpret_cast<doPreStub_t>(result);
		}

		if (!getWrappedMethodDesc)
		{
			uintptr_t result = Vanilla::FindSignature("\xF6\x41\x03\x04\x0F\x85\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0\x0F\x84", "xxxxxx????x????xxxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll"));
			if (!result)
				result = Vanilla::FindSignature("\xF6\x41\x03\x04\x0F\x84", "xxxxxx", (uintptr_t)GetModuleHandleA("clr.dll"), Vanilla::GetModuleSize("clr.dll"));

			getWrappedMethodDesc = reinterpret_cast<getWrappedMethodDesc_t>(result);
		}

		if (!hasPrecode(pMethodDesc))
			return; //not a precode stub.

		if (GetFunctionPointer(pMethodDesc) != nullptr)
			return; //method is already jitted.

		DWORD precodeDword = reinterpret_cast<DWORD>(precodePointer);
		if (*reinterpret_cast<BYTE*>(precodeDword) == 0xB8) //assuming it's first precode instruction (mov eax, pMethodDesc)
		{
			if (*reinterpret_cast<BYTE*>(precodeDword + 0x5) == 0x90) //check if it's a remoting precode
			{
				DWORD precodeRemotingThunkOffset = *reinterpret_cast<DWORD*>(precodeDword + 0x7);
				DWORD precodeRemotingThunk = precodeDword + 0xB + precodeRemotingThunkOffset;

				__asm
				{
					mov eax, pMethodDesc
					call precodeRemotingThunk
				}
			}

			doPreStub(pMethodDesc, nullptr);
			if (isUnboxingStub(pMethodDesc)) //UnboxingStub methods can't be compiled using regular MethodDesc, convert to WrappedMethodDesc instead.
			{
				void* pUnboxingStub = getWrappedMethodDesc(pMethodDesc);
				doPreStub(pUnboxingStub, nullptr);
			}
		}
	}

	static void* GetFunctionPointer(void* pMethodDesc)
	{
		return getNativeCode(pMethodDesc);
	}
};