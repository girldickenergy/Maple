#include "ExceptionHandler.h"

#include <psapi.h>
#include <map>

#include "xorstr.hpp"
#include "../../Logging/Logger.h"

[[clang::optnone]] LONG __stdcall ExceptionHandler::pVectoredExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;

    DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    PVOID exceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

    auto ctx = ExceptionInfo->ContextRecord;

    const char* exceptionMessageFormat = std::string(xorstr_("Maple has encountered a crash!\n\nExceptionCode: 0x%08X ExceptionAddress: %p\nEAX: 0x%08X ECX: 0x%08X EDX: 0x%08X EBX: 0x%08X ESP: 0x%08X EBP: 0x%08X ESI: 0x%08X EDI: 0x%08X EIP: 0x%08X\n\nModules: \n%s\n\nFrame: \n%s\n\n")).c_str();

    MEMORY_BASIC_INFORMATION32 mbi{};

    LPVOID address = nullptr;
    bool foundHeader = false;
    auto moduleMap = std::map<std::string, std::vector<MEMORY_BASIC_INFORMATION32>>();

    auto currentModule = std::vector<MEMORY_BASIC_INFORMATION32>();
    std::string moduleName = "";
    while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
    {
        address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
        if (mbi.BaseAddress == NULL)
            continue;

        // Search for only PE Headers in memory
        if (mbi.AllocationProtect == PAGE_EXECUTE_WRITECOPY &&
            mbi.State == MEM_COMMIT &&
            mbi.Protect == PAGE_READONLY &&
            mbi.Type == MEM_IMAGE &&
            mbi.RegionSize == 0x1000)
        {
            PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(mbi.BaseAddress);
            if (dosHeader->e_magic != 0x5a4d)
                continue;

            char szDll[2048];
            DWORD result = GetMappedFileNameA(GetCurrentProcess(), address, szDll, sizeof(szDll));
            if (result == 0)
                continue;
            currentModule.emplace_back(mbi);
            moduleName = std::string(szDll, result);
            foundHeader = true;
        }

        if (foundHeader)
        {
            if (mbi.State == MEM_FREE && mbi.Protect == PAGE_NOACCESS) {
                foundHeader = false;
                moduleMap.insert(std::make_pair(moduleName, currentModule));

                currentModule.clear();
                moduleName = "";
                continue;
            }

            currentModule.emplace_back(mbi);
        }
    }

    const char* moduleFormat = std::string(xorstr_("%s loaded at 0x%08X (size 0x%08X)\n")).c_str();

    char moduleBuffer[2048];
    std::ostringstream modules;

    for (auto const& module : moduleMap)
    {
        DWORD base = module.second[0].BaseAddress;
        // Calculate size of module
        uint32_t size = 0;
        for (int i = 0; i < module.second.size(); i++)
        {
            auto region = module.second[i];
            size += region.RegionSize;
        }
        int stringLength = sprintf(moduleBuffer, moduleFormat, module.first.c_str(), base, size);
        modules << std::string(moduleBuffer, stringLength);
    }

    const int MAX_CALLERS = 62;
    PVOID callers[MAX_CALLERS];
    int count = CaptureStackBackTrace(0, MAX_CALLERS, callers, nullptr);

    const char* frameFormat = std::string(xorstr_("0x%08X called from 0x%08X (%s)\n")).c_str();

    char frameBuffer[2048];
    std::ostringstream stackFrame;
    for (int i = 0; i < count; i++) {
        DWORD caller = reinterpret_cast<DWORD>(callers[i]);
        std::string moduleName = "";
        // Find the caller in modules
        for (auto const& module : moduleMap)
        {
            DWORD base = module.second[0].BaseAddress;
            // Calculate size of module
            uint32_t size = 0;
            for (int i = 0; i < module.second.size(); i++)
            {
                auto region = module.second[i];
                size += region.RegionSize;
            }

            if (caller >= base && base + size >= caller)
            {
                moduleName = module.first;
                break;
            }
        }

        int stringLength = sprintf(frameBuffer, frameFormat, i, callers[i], moduleName.c_str());
        stackFrame << std::string(frameBuffer, stringLength);
    }

    char crashReportBuffer[1024 + modules.str().size() + stackFrame.str().size()];
    int stringLength = sprintf(crashReportBuffer, exceptionMessageFormat, exceptionCode, exceptionAddress,
        ctx->Eax, ctx->Ecx, ctx->Edx, ctx->Ebx, ctx->Esp, ctx->Ebp, ctx->Esi, ctx->Edi,
        ctx->Eip, modules.str().c_str(), stackFrame.str().c_str());

    [[clang::noinline]] Logger::WriteCrashReport(std::string(crashReportBuffer, stringLength));
    MessageBoxA(nullptr, xorstr_("Maple has encountered a crash and has created a crash report, please send it to us!\nYou can access this crash report next time you load Maple by navigating into the 'Misc' tab, then into the 'Logging' section and clicking 'Copy crash report to clipboard'."), nullptr, MB_OK | MB_ICONERROR | MB_TOPMOST);

    ExitProcess(0xB00BB00B);

    return EXCEPTION_CONTINUE_SEARCH;
}

void ExceptionHandler::Setup()
{
    [[clang::noinline]] AddVectoredExceptionHandler(false, pVectoredExceptionHandler);
}
