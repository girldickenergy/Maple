#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "xorstr.h"

#include "MapleBase.h"
#include "Modules/IModule.h"
#include "SDK/InputManager/InputManager.h"

class TestModule : public IModule
{
    std::shared_ptr<InputManager> m_InputManager;

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override
    {
        IModule::OnLoad(mapleBase);

        m_InputManager = std::dynamic_pointer_cast<InputManager>(m_MapleBase->GetSDK(xorstr_("InputManager")));
    }

    void __fastcall OnUpdate() override
    {
        const Vector2 cursorPosition = m_InputManager->GetCursorPosition();

        printf("%f, %f\n", cursorPosition.X, cursorPosition.Y);
    }

    void __fastcall OnRender() override {}
};

std::shared_ptr<MapleBase> mapleBase;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**>(stdout), xorstr_("CONOUT$"), xorstr_("w"), stdout);

	mapleBase = std::make_shared<MapleBase>();
        mapleBase->Initialize();

	mapleBase->AddSDKRange({
	    std::make_pair<std::string, std::shared_ptr<ISDK>>(xorstr_("InputManager"), std::make_shared<InputManager>())
        });

	mapleBase->AddModuleRange({
	    std::make_shared<TestModule>()
        });
    }

    return TRUE;
}
