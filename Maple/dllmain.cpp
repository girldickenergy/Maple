#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "xorstr.h"

#include "MapleBase.h"
#include "Modules/IModule.h"
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

    Vector2 __fastcall OnCursorPositionUpdate(Vector2 currentPosition) override
    {
        printf("Current cursor position: %f, %f\n", currentPosition.X, currentPosition.Y);

        return currentPosition + Vector2(100, 100);
    }

    OsuKeys __fastcall OnGameplayKeysUpdate(OsuKeys currentKeys) override
    {
        printf("Current keys: %i\n", static_cast<int>(currentKeys));

        return currentKeys;
    }

    bool __fastcall OnScoreSubmission() override
    {
        printf("Score submission started");

        return true;
    }

    void __fastcall OnPlayerLoad() override
    {
        auto instance = m_GameField->GetInternalInstance();
        float width = instance ? instance->Width : 0.f;
        float height = instance ? instance->Height : 0.f;

        printf("Player loaded: %f, %f\n", width, height);
    }

    void __fastcall OnPlayerExit() override
    {
        printf("Player exited");
    }

    bool __fastcall RequiresCursorPosition() override
    {
        return true;
    }

    bool __fastcall RequiresGameplayKeys() override
    {
        return true;
    }

    bool __fastcall RequiresScoreSubmission() override
    {
        return true;
    }

    std::string __fastcall GetName() override
    {
        return xorstr_("TestModule");
    }
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
	    std::make_shared<GameField>(),
        });

	mapleBase->AddModuleRange({
	    std::make_shared<TestModule>()
        });
    }

    return TRUE;
}
