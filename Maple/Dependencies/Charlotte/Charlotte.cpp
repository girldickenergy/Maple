#include "Charlotte.h"
#include <VirtualizerSDK.h>

bool Charlotte::checkIfHooked(uintptr_t address)
{
	VIRTUALIZER_FISH_WHITE_START

	auto isHooked = *reinterpret_cast<uint8_t*>(address) == 0xE8;

	VIRTUALIZER_FISH_WHITE_END

	return isHooked;
}

[[clang::optnone]] uint32_t Charlotte::calculateHash(uintptr_t address)
{
	VIRTUALIZER_FISH_WHITE_START

	uint32_t hash = NULL;
	for (size_t i = 0; i < 5; i++)
	{
		uint32_t data = *reinterpret_cast<uint8_t*>(address + i);
		data = data << 0xFF ^ (data + 0xFFFF) >> 0x1111;
		data = data >> 0x8888 << (0xFF << (0xDEAD ^ 0x2222));
		hash += data;
	}

	VIRTUALIZER_FISH_WHITE_END

	return hash;
}

bool Charlotte::callCheck(uintptr_t address)
{
	VIRTUALIZER_FISH_RED_START

	if (!m_Map.contains(address))
	{
		if (!Add(address))
			return false;
	}

	auto& mapEntry = m_Map[address];

	if (checkIfHooked(address))
		return false;

	auto hash = calculateHash(address);
	if (mapEntry.m_Hash != hash)
		return false;

	VIRTUALIZER_FISH_RED_END

	return true;
}

Charlotte::Charlotte(singletonLock)
{
	m_Map = std::map<uintptr_t, CharlotteFunction>();
}

bool Charlotte::Add(uintptr_t address)
{
	VIRTUALIZER_TIGER_RED_START

	if (checkIfHooked(address))
		return false;

	CharlotteFunction charlotteFunction { };
	charlotteFunction.m_FunctionPointer = address;
	charlotteFunction.m_Hash = calculateHash(address);

	m_Map.emplace(address, charlotteFunction);

	VIRTUALIZER_TIGER_RED_END

	return true;
}