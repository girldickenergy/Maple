#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "xorstr.h"

#include "MapleBase.h"
#include "Modules/IModule.h"
#include "SDK/Osu/GameField.h"
#include "Sdk/Audio/AudioEngine.h"
#include "Sdk/Mods/ModManager.h"
#include "Sdk/Osu/GameBase.h"
#include "Sdk/Scoring/Score.h"

class TestModule : public IModule
{
    std::shared_ptr<AudioEngine> m_AudioEngine;
    std::shared_ptr<GameBase> m_GameBase;
    std::shared_ptr<GameField> m_GameField;
    std::shared_ptr<Score> m_Score;
    std::shared_ptr<ModManager> m_ModManager;

public:
    void __fastcall OnLoad(const std::shared_ptr<MapleBase>& mapleBase) override
    {
        IModule::OnLoad(mapleBase);

        m_AudioEngine = std::dynamic_pointer_cast<AudioEngine>(m_MapleBase->GetSDK(xorstr_("AudioEngine")));
        m_GameBase = std::dynamic_pointer_cast<GameBase>(m_MapleBase->GetSDK(xorstr_("GameBase")));
        m_GameField = std::dynamic_pointer_cast<GameField>(m_MapleBase->GetSDK(xorstr_("GameField")));
        m_Score = std::dynamic_pointer_cast<Score>(m_MapleBase->GetSDK(xorstr_("Score")));
        m_ModManager = std::dynamic_pointer_cast<ModManager>(m_MapleBase->GetSDK(xorstr_("ModManager")));
    }

    CursorState __fastcall OnCursorPositionUpdate(CursorState state) override
    {
        printf("Current cursor position: %f, %f\n", state.CursorPosition.X, state.CursorPosition.Y);

        return state;
    }

    KeyState __fastcall OnGameplayKeysUpdate(KeyState state) override
    {
        printf("M1: %i, M2: %i\n", state.LeftMouseButtonDown, state.RightMouseButtonDown);
        state.RightMouseButtonDown = true;

        return state;
    }

    bool __fastcall OnScoreSubmission() override
    {
        printf("Score submission started");
        //m_Score->SetStartTime(1337);

        return true;
    }

    void __fastcall OnPlayerLoad() override
    {
        printf("Player loaded\n");
    }

    bool __fastcall RequiresCursorPosition() override
    {
        return true;
    }

    bool __fastcall RequiresGameplayKeys() override
    {
        return true;
    }

    const char* __fastcall GetName() override
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
	mapleBase = std::make_shared<MapleBase>();
        mapleBase->Initialize();

	mapleBase->AddSDKRange({
            std::make_shared<AudioEngine>(),
            std::make_shared<GameBase>(),
	    std::make_shared<GameField>(),
            std::make_shared<Score>(),
            std::make_shared<ModManager>()
        });

	mapleBase->AddModuleRange({
	    std::make_shared<TestModule>()
        });
    }

    return TRUE;
}
