#pragma once
#include <Windows.h>

class MilkThread
{
    // MilkMemory _milkMemory;
    uintptr_t m_FunctionPointer;

    uintptr_t* m_CodeCavePointer;
    bool m_CodeCavePrepared;

    void PrepareCodeCave();

public:
    MilkThread(uintptr_t functionPointer);
    MilkThread() = default;

    HANDLE Start();

    void CleanCodeCave();
    void SetFunctionPointer(uintptr_t functionPointer);
};
