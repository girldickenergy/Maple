#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "xorstr.h"

#include "MapleBase.h"
#include "Modules/IModule.h"
#include "SDK/InputManager/InputManager.h"
#include "SDK/Osu/GameField.h"

class TestModule : public IModule
{
    std::shared_ptr<GameField> m_GameField;

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override
    {
        IModule::OnLoad(mapleBase);

        m_GameField = std::dynamic_pointer_cast<GameField>(m_MapleBase->GetSDK(xorstr_("GameField")));
    }

    void __fastcall OnUpdate() override
    {
        auto instance = m_GameField->GetInternalInstance();
        float width = instance ? instance->Width : 0.f;
        float height = instance ? instance->Height : 0.f;

        printf("%f, %f\n", width, height);
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
	    std::make_pair<std::string, std::shared_ptr<ISDK>>(xorstr_("GameField"), std::make_shared<GameField>()),
	    std::make_pair<std::string, std::shared_ptr<ISDK>>(xorstr_("InputManager"), std::make_shared<InputManager>())
        });

	mapleBase->AddModuleRange({
	    std::make_shared<TestModule>()
        });
    }

    return TRUE;
}
