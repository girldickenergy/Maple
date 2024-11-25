#include "MilkThread.h"

#include <string>
#include <Charlotte.h>

#include <VirtualizerSDK.h>

MilkThread::MilkThread(uintptr_t function, bool lazy)
{
	VIRTUALIZER_TIGER_WHITE_START

	_codeCavePrepared = false;
	_milkMemory = MilkMemory();
	_function = function;

	_codeCaveLocation = _milkMemory.FindCodeCave();

	if (!lazy)
		[[clang::noinline]] Start();

	VIRTUALIZER_TIGER_WHITE_END
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
    VIRTUALIZER_TIGER_WHITE_START

	auto& charlotte = Charlotte::Get();

	DWORD oldProtection;
	charlotte.Call<BOOL>(reinterpret_cast<uintptr_t>(VirtualProtect), _codeCaveLocation, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0x68; // push
    const uintptr_t jumpLocation = _function;
	*reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(_codeCaveLocation) + 1)) = jumpLocation;
    *reinterpret_cast<uint8_t *>((reinterpret_cast<uintptr_t>(_codeCaveLocation) + 5)) = 0xC3; // ret
	charlotte.Call<BOOL>(reinterpret_cast<uintptr_t>(VirtualProtect), _codeCaveLocation, 6, oldProtection, &oldProtection);

	_codeCavePrepared = true;

	VIRTUALIZER_TIGER_WHITE_END
}

void MilkThread::CleanCodeCave()
{
    VIRTUALIZER_TIGER_WHITE_START

	auto& charlotte = Charlotte::Get();

	DWORD oldProtection;
	charlotte.Call<BOOL>(reinterpret_cast<uintptr_t>(VirtualProtect), _codeCaveLocation, 6, PAGE_EXECUTE_READWRITE, &oldProtection);
	*reinterpret_cast<uint8_t*>(_codeCaveLocation) = 0xCC;
	*(_codeCaveLocation + 1) = 0xCCCCCCCC;
	*reinterpret_cast<uint8_t*>(_codeCaveLocation + 5) = 0xCC;
	charlotte.Call<BOOL>(reinterpret_cast<uintptr_t>(VirtualProtect), _codeCaveLocation, 6, oldProtection, &oldProtection);

	_codeCavePrepared = false;

	VIRTUALIZER_TIGER_WHITE_END
}

HANDLE MilkThread::Start()
{
        VIRTUALIZER_TIGER_WHITE_START
	if (_codeCaveLocation == nullptr)
		return nullptr; //TODO: error logging & crash osu!

	if (!_codeCavePrepared)
		[[clang::noinline]] prepareCodeCave();

	auto& charlotte = Charlotte::Get();
	auto ret = charlotte.Call<HANDLE>(reinterpret_cast<uintptr_t>(CreateThread), nullptr, NULL, 
		reinterpret_cast<LPTHREAD_START_ROUTINE>(_codeCaveLocation), nullptr, NULL, nullptr);
    
	VIRTUALIZER_TIGER_WHITE_END
	return ret;
}

void MilkThread::SetFunctionPointer(uintptr_t function)
{
	VIRTUALIZER_LION_BLACK_START

	_function = function;
	CleanCodeCave();

	VIRTUALIZER_LION_BLACK_END
}