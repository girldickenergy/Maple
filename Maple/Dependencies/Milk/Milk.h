#pragma once
#include "Structs/CRC.h"
#include "MilkMemory.h"
#include "../../Utilities/Architecture/Singleton.hpp"

class Milk : public Singleton<Milk>
{
	MilkMemory _milkMemory;
	uintptr_t _authStubBaseAddress;
	uintptr_t _firstCRCAddress;
	CRC* _firstCRC;
	bool preparationSuccess;

	uintptr_t findAuthStub();
	uintptr_t findFirstCRCAddress();

	bool prepare();
	/**
	 * \brief Bypasses the detection vector where functions would be checked against CRC32.
	 */
	bool doCRCBypass();
public:
	Milk(singletonLock);
	~Milk();

	bool DoBypass();
};