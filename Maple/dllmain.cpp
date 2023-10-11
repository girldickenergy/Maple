#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "xorstr.h"

#include "MapleBase.h"
#include "Modules/IModule.h"
#include "SDK/Osu/GameField.h"
#include "Sdk/Audio/AudioEngine.h"
#include "Sdk/Scoring/Score.h"

class TestModule : public IModule
{
    std::shared_ptr<AudioEngine> m_AudioEngine;
    std::shared_ptr<GameField> m_GameField;
    std::shared_ptr<Score> m_Score;

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override
    {
        IModule::OnLoad(mapleBase);

        m_AudioEngine = std::dynamic_pointer_cast<AudioEngine>(m_MapleBase->GetSDK(xorstr_("AudioEngine")));
        m_GameField = std::dynamic_pointer_cast<GameField>(m_MapleBase->GetSDK(xorstr_("GameField")));
        m_Score = std::dynamic_pointer_cast<Score>(m_MapleBase->GetSDK(xorstr_("Score")));
    }

    Vector2 __fastcall OnCursorPositionUpdate(Vector2 currentPosition) override
    {
        printf("Current cursor position: %f, %f | Current time: %i\n", currentPosition.X, currentPosition.Y, m_AudioEngine->GetTime());

        return currentPosition + Vector2(100, 100);
    }

    OsuKeys __fastcall OnGameplayKeysUpdate(OsuKeys currentKeys) override
    {
        printf("Current keys: %i\n", static_cast<int>(currentKeys));

        return currentKeys;
    }

    bool __fastcall OnScoreSubmission() override
    {
        printf("Score submission started, start time: %i", m_Score->GetStartTime());
        m_Score->SetStartTime(1337);

        return true;
    }

    void __fastcall OnPlayerLoad() override
    {
        printf("Player loaded: %f, %f\n", m_GameField->GetWidth(), m_GameField->GetHeight());
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
            std::make_shared<AudioEngine>(),
	    std::make_shared<GameField>(),
            std::make_shared<Score>()
        });

	mapleBase->AddModuleRange({
	    std::make_shared<TestModule>()
        });
    }

    return TRUE;
}
