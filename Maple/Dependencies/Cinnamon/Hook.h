#pragma once

#include <string>
#include <vector>
#include <Windows.h>

enum class HookType : int
{
	ByteCodePatch			= 0x0000,
	HardwareBreakpoint		= 0x1000,
	UndetectedByteCodePatch = 0x2000
};

/**
* \brief Base class for all types of hooks, instantiate this for a basic Bytecode patch hook
*/
class Hook
{
public:
	std::string Name;
	void* FunctionAddress;
	void* CodeCaveAddress;
	std::vector<unsigned char> Prologue;
	HookType Type;

	Hook(std::string name, void* functionAddress, void* codeCaveAddress, std::vector<unsigned char> prologue, HookType type)
	{
		Name = name;
		FunctionAddress = functionAddress;
		CodeCaveAddress = codeCaveAddress;
		Prologue = prologue;
		Type = type;
	}
};

class ByteCodePatchHook : public Hook
{
public:
	ByteCodePatchHook(std::string name, void* functionAddress, void* codeCaveAddress, std::vector<unsigned char> prologue) : Hook(name, functionAddress, codeCaveAddress, prologue, HookType::ByteCodePatch) {}
};

class HardwareBreakpointHook : public Hook
{
public:
	void* HookFunctionAddress;
	HANDLE ThreadHandle;
	
	HardwareBreakpointHook(std::string name, void* functionAddress, void* hookFunctionAddress, void* codeCaveAddress, std::vector<unsigned char> prologue, HANDLE threadHandle) : Hook(name, functionAddress, codeCaveAddress, prologue, HookType::HardwareBreakpoint)
	{
		HookFunctionAddress = hookFunctionAddress;
		ThreadHandle = threadHandle;
	}
};