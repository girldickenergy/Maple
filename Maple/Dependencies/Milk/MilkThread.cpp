#include "MilkThread.h"

#include <string>

#include "WinUser.h"

MilkThread::MilkThread(uintptr_t function, bool lazy)
{
	_milkMemory = MilkMemory();
	_function = function;

	_codeCaveLocation = _milkMemory.FindCodeCave();

	if (!lazy)
		Start();
}

MilkThread::~MilkThread()
{
	_codeCaveLocation = nullptr;
	_function = NULL;
	_milkMemory.~MilkMemory();
}

void MilkThread::prepareCodeCave()
{
	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xE9; // rel jmp
	const uintptr_t jumpLocation = _function - 5 - reinterpret_cast<uintptr_t>(_codeCaveLocation);
	*reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(_codeCaveLocation) + 1)) = jumpLocation;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = true;
}

void MilkThread::cleanCodeCave()
{
	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xCC;
	*(_codeCaveLocation + 1) = 0xCCCCCCCC;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = false;
}

void MilkThread::Start()
{
	if (_codeCaveLocation == nullptr)
		return; //TODO: error logging & crash osu!

	if (!_codeCavePrepared)
		prepareCodeCave();

	CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_codeCaveLocation),
		nullptr, NULL, nullptr);
}

void MilkThread::SetFunctionPointer(uintptr_t function)
{
	_function = function;
	cleanCodeCave();
}