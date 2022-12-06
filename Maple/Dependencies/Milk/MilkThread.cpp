#include "MilkThread.h"

#include <string>
#include "WinUser.h"
#include <ThemidaSDK.h>

#pragma optimize("", off)

MilkThread::MilkThread(uintptr_t function, bool lazy)
{
	VM_FISH_RED_START
	_milkMemory = MilkMemory();
	_function = function;

	_codeCaveLocation = _milkMemory.FindCodeCave();

	if (!lazy)
		Start();
	VM_FISH_RED_END
}

MilkThread::~MilkThread()
{
	VM_FISH_RED_START
	_codeCaveLocation = nullptr;
	_function = NULL;
	_milkMemory.~MilkMemory();
	VM_FISH_RED_END
}

void MilkThread::prepareCodeCave()
{
	VM_LION_BLACK_START
	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xE9; // rel jmp
	const uintptr_t jumpLocation = _function - 5 - reinterpret_cast<uintptr_t>(_codeCaveLocation);
	*reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(_codeCaveLocation) + 1)) = jumpLocation;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = true;
	VM_LION_BLACK_END
}

void MilkThread::cleanCodeCave()
{
	VM_LION_BLACK_START
	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xCC;
	*(_codeCaveLocation + 1) = 0xCCCCCCCC;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = false;
	VM_LION_BLACK_END
}

bool MilkThread::Start()
{
	VM_LION_BLACK_START
	if (_codeCaveLocation == nullptr)
		return false; //TODO: error logging & crash osu!

	if (!_codeCavePrepared)
		prepareCodeCave();

	auto ret = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_codeCaveLocation),
		nullptr, NULL, nullptr);
	return ret != nullptr;
	VM_LION_BLACK_END
}

void MilkThread::SetFunctionPointer(uintptr_t function)
{
	VM_LION_BLACK_START
	_function = function;
	cleanCodeCave();
	VM_LION_BLACK_END
}