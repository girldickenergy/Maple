#include <thread>
#include <windows.h>

#include "MapleBase.h"
#include "Utilities/DependencyInjection/DependencyContainer.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
	    auto mapleBase = new MapleBase();
        DependencyContainer::Cache("MapleBase", mapleBase);
        std::thread initializationThread(&MapleBase::Initialize, mapleBase);
        initializationThread.detach();
    }
	
    return TRUE;
}
