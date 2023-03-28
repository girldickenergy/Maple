#include "MilkThread.h"

#include <string>
#include <VirtualizerSDK.h>

MilkThread::MilkThread(uintptr_t function, bool lazy)
{
	VIRTUALIZER_FISH_RED_START

	_codeCavePrepared = false;
	_milkMemory = MilkMemory();
	_function = function;

	_codeCaveLocation = _milkMemory.FindCodeCave();

	if (!lazy)
		[[clang::noinline]] Start();

	VIRTUALIZER_FISH_RED_END
}

MilkThread::~MilkThread()
{
	VIRTUALIZER_FISH_RED_START
	_codeCaveLocation = nullptr;
	_codeCavePrepared = false;
	_function = NULL;
	_milkMemory.~MilkMemory();
	VIRTUALIZER_FISH_RED_END
}

void MilkThread::prepareCodeCave()
{
	VIRTUALIZER_TIGER_BLACK_START

	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xE9; // rel jmp
	const uintptr_t jumpLocation = _function - 5 - reinterpret_cast<uintptr_t>(_codeCaveLocation);
	*reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(_codeCaveLocation) + 1)) = jumpLocation;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = true;

	VIRTUALIZER_TIGER_BLACK_END
}

void MilkThread::CleanCodeCave()
{
	VIRTUALIZER_LION_BLACK_START

	DWORD oldProtection;
	VirtualProtect(_codeCaveLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xCC;
	*(_codeCaveLocation + 1) = 0xCCCCCCCC;
	VirtualProtect(_codeCaveLocation, 5, oldProtection, &oldProtection);

	_codeCavePrepared = false;

	VIRTUALIZER_LION_BLACK_END
}

HANDLE MilkThread::Start()
{
	VIRTUALIZER_LION_BLACK_START
	if (_codeCaveLocation == nullptr)
		return nullptr; //TODO: error logging & crash osu!

	if (!_codeCavePrepared)
		[[clang::noinline]] prepareCodeCave();

	HANDLE ret = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(_codeCaveLocation),
		nullptr, NULL, nullptr);
	VIRTUALIZER_LION_BLACK_END
	return ret;
}

void MilkThread::SetFunctionPointer(uintptr_t function)
{
	VIRTUALIZER_LION_BLACK_START

	_function = function;
	CleanCodeCave();

	VIRTUALIZER_LION_BLACK_END
}