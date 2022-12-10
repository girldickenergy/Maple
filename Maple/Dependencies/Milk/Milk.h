#pragma once
#include "Structs/CRC.h"
#include "MilkMemory.h"
#include "../../Utilities/Architecture/Singleton.hpp"

struct v7fix
{
	uint8_t padding[0x40];
	double speed;
};

struct v8fix
{
	uint8_t padding[4];
	v7fix* v7;
};

struct v9fix
{
	uint8_t padding[0x40];
	float freq;
};

struct v10fix
{
	uint8_t padding[0x20];
	v9fix* v9;
	uint8_t padding2[0x90];
};

class Milk : public Singleton<Milk>
{
	MilkMemory _milkMemory;
	uintptr_t _authStubBaseAddress;
	uintptr_t _firstCRCAddress;
	CRC* _firstCRC;
	static inline uintptr_t _originalJITVtable;
	static inline uintptr_t* _copiedJITVtable;
	bool preparationSuccess;

	using fnGetJit = uintptr_t(__stdcall*)();
	static inline fnGetJit oGetJit;
	static uintptr_t __stdcall getJitHook();

	static inline struct v7fix v7 = {};
	static inline struct v8fix v8 = {};
	static inline struct v9fix v9 = {};
	static inline struct v10fix v10 = {};
	using fnSomeBassFunc = int(__stdcall*)(int a1);
	static inline fnSomeBassFunc oSomeBassFunc;
	static void someBassFuncHook();
	static int __stdcall spoofPlaybackRate(int a1, DWORD ebp, DWORD ret);

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
	bool CheckFunction(uintptr_t function);
	void HookJITVtable(int index, uintptr_t detour, uintptr_t* originalFunction);
};
