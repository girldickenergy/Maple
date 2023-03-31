#include "ExceptionHandler.h"

#include <psapi.h>

#pragma comment(lib, "Dbghelp.lib")
#include <DbgHelp.h>

#include "xorstr.hpp"
#include "../../Logging/Logger.h"

[[clang::optnone]] LONG __stdcall ExceptionHandler::pVectoredExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;

    const DWORD exceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    const PVOID exceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;

    const auto ctx = ExceptionInfo->ContextRecord;

    const char* exceptionHeaderFormat = xorstr_("Maple has encountered a crash!\n\nExceptionCode: 0x%08X ExceptionAddress: %p\nEAX: 0x%08X ECX: 0x%08X EDX: 0x%08X EBX: 0x%08X ESP: 0x%08X EBP: 0x%08X ESI: 0x%08X EDI: 0x%08X EIP: 0x%08X");
    const char* moduleFormat = xorstr_("%s loaded at 0x%08X (size 0x%08X)");
    const char* frameFormat = xorstr_("at %s (0x%08X) in %s");

    std::ostringstream crashReport;

    char headerBuffer[4096];
    const int headerStringLength = sprintf_s(headerBuffer, exceptionHeaderFormat, exceptionCode, exceptionAddress,
        ctx->Eax, ctx->Ecx, ctx->Edx, ctx->Ebx, ctx->Esp, ctx->Ebp, ctx->Esi, ctx->Edi, ctx->Eip);
    crashReport << std::string(headerBuffer, headerStringLength) << std::endl;

    crashReport << std::string(xorstr_("\nModules:\n"));

    DWORD moduleCount;
    HMODULE modules[1024];
	if (EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &moduleCount))
	{
        for (int i = 0; i < moduleCount / sizeof(HMODULE); i++)
        {
            MODULEINFO moduleInfo;
            if (GetModuleInformation(GetCurrentProcess(), modules[i], &moduleInfo, sizeof(MODULEINFO)))
            {
                char moduleFileName[2048];
                std::string moduleFileNameString;
                const int moduleFileNameLength = GetMappedFileNameA(GetCurrentProcess(), moduleInfo.lpBaseOfDll, moduleFileName, sizeof(moduleFileName));
                if (moduleFileNameLength > 0)
                    moduleFileNameString = std::string(moduleFileName, moduleFileNameLength);
                else
                    moduleFileNameString = xorstr_("<unknown>");

                char moduleBuffer[4096];
                const int moduleStringLength = sprintf_s(moduleBuffer, moduleFormat, moduleFileNameString.c_str(), moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
                crashReport << std::string(moduleBuffer, moduleStringLength) << std::endl;
            }
        }
	}
	
    crashReport << std::string(xorstr_("\nFrame:\n"));

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    const auto pSymbol = reinterpret_cast<PSYMBOL_INFO>(buffer);

    CONTEXT ctxCopy;
    memcpy(&ctxCopy, ctx, sizeof(CONTEXT));

    STACKFRAME stack = {};

    DWORD64 displacement = 0;
    stack.AddrPC.Offset = (*ctx).Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;

    SymInitialize(GetCurrentProcess(), NULL, TRUE);

    for (unsigned int frame = 0; ; frame++)
    {
        if (!StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stack, &ctxCopy, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
            break;

        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(GetCurrentProcess(), stack.AddrPC.Offset, &displacement, pSymbol);

        HMODULE hModule;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)stack.AddrPC.Offset, &hModule);

        char moduleFileName[2048];
        std::string moduleFileNameString;
        const int moduleFileNameLength = GetMappedFileNameA(GetCurrentProcess(), hModule, moduleFileName, sizeof(moduleFileName));
        if (moduleFileNameLength > 0)
            moduleFileNameString = std::string(moduleFileName, moduleFileNameLength);
        else
            moduleFileNameString = xorstr_("<unknown>");

        char frameBuffer[4096];
        const int frameStringLength = sprintf_s(frameBuffer, frameFormat, pSymbol->NameLen > 0 ? pSymbol->Name : xorstr_("<unknown>"), stack.AddrPC.Offset, moduleFileNameString.c_str());
        crashReport << std::string(frameBuffer, frameStringLength) << std::endl;
    }

    [[clang::noinline]] Logger::WriteCrashReport(crashReport.str());

    MessageBoxA(nullptr, xorstr_("Maple has encountered a crash and created a crash log, please send it to us!\nYou can access this crash log next time you load Maple by navigating into the 'Misc' tab, then into the 'Logging' section and clicking 'Copy crash report to clipboard'."), nullptr, MB_OK | MB_ICONERROR | MB_TOPMOST);

    ExitProcess(0xB00BB00B);

    return EXCEPTION_CONTINUE_SEARCH;
}

void ExceptionHandler::Setup()
{
    [[clang::noinline]] AddVectoredExceptionHandler(false, pVectoredExceptionHandler);
}
