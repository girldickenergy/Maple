#include "MilkThread.h"

#include <string>

MilkThread::MilkThread(const uintptr_t functionPointer)
{
    m_FunctionPointer = functionPointer;
    // TODO: make a wrapper classes so there's no direct dependency from milkthread to milkmemory (clean code uwuuwu)
    m_CodeCavePointer = nullptr;
    m_CodeCavePrepared = false;
}

void MilkThread::PrepareCodeCave()
{
    DWORD oldProtection;
    VirtualProtect(m_CodeCavePointer, 6, PAGE_EXECUTE_READWRITE, &oldProtection);

    *reinterpret_cast<uint8_t*>(m_CodeCavePointer) = 0x68; // push
    *(m_CodeCavePointer + 1) = m_FunctionPointer;
    *reinterpret_cast<uint8_t*>((m_CodeCavePointer + 5)) = 0xC3; // ret

    VirtualProtect(m_CodeCavePointer, 6, oldProtection, &oldProtection);

    m_CodeCavePrepared = true;
}

void MilkThread::CleanCodeCave()
{
    DWORD oldProtection;
    VirtualProtect(m_CodeCavePointer, 6, PAGE_EXECUTE_READWRITE, &oldProtection);

    *reinterpret_cast<uint8_t*>(m_CodeCavePointer) = 0xCC;
    *(m_CodeCavePointer + 1) = 0xCCCCCCCC;
    *reinterpret_cast<uint8_t*>(m_CodeCavePointer + 5) = 0xCC;

    VirtualProtect(m_CodeCavePointer, 6, oldProtection, &oldProtection);

    m_CodeCavePrepared = false;
}

HANDLE MilkThread::Start()
{
    if (m_CodeCavePointer == nullptr)
        return nullptr; // TODO: error logging & crash osu!

    if (!m_CodeCavePrepared) [[clang::noinline]]
        PrepareCodeCave();

    return CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(m_CodeCavePointer), nullptr, NULL, nullptr);
}

void MilkThread::SetFunctionPointer(const uintptr_t functionPointer)
{
    m_FunctionPointer = functionPointer;
    CleanCodeCave();
}