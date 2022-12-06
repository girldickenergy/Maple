#pragma once
#include "MilkMemory.h"

class MilkThread
{
	MilkMemory _milkMemory;
	uintptr_t _function;

	uint32_t* _codeCaveLocation;
	bool _codeCavePrepared;

	void prepareCodeCave();
	void cleanCodeCave();
public:
	MilkThread(uintptr_t function, bool lazy = false);
	~MilkThread();

	bool Start();

	void SetFunctionPointer(uintptr_t function);
};