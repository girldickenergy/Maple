#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Vanilla.h"

#include "Logging/Logger.h"
#include "SDK/Vector2.h"

class IModule;
class ISDK;

class MapleBase : public std::enable_shared_from_this<MapleBase>
{
    static inline std::shared_ptr<Storage> m_Storage = nullptr;
    static inline std::shared_ptr<Logger> m_RuntimeLogger = nullptr;
    static inline std::shared_ptr<Vanilla> m_Vanilla = nullptr;
    static inline std::unordered_map<std::string, std::shared_ptr<ISDK>> m_SDKs = {};
    static inline std::vector<std::shared_ptr<IModule>> m_Modules = {};

    static void InitializeStorage(const std::string& directoryName);
    static void InitializeLogging();
    static void InitializeCore();

    static void OnJIT(uintptr_t address, unsigned int size);

    #pragma region Hooks for module callbacks
    typedef void(__fastcall* fnSetMousePosition)(Vector2 position);
    static inline fnSetMousePosition oSetMousePosition = nullptr;
    static void __fastcall SetMousePositionHook(Vector2 pos);

    static inline bool* m_MouseButtonInstantRelease = nullptr;
    static inline bool* m_LeftButtons = nullptr;
    static inline bool* m_RightButtons = nullptr;
    static inline int* m_LeftButton = nullptr;
    static inline int* m_RightButton = nullptr;

    typedef void(__fastcall* fnMouseViaKeyboardControls)();
    static inline fnMouseViaKeyboardControls oMouseViaKeyboardControls = nullptr;
    static void __fastcall MouseViaKeyboardControlsHook();

    static inline bool m_ScoreSubmissionUnsafe = false;

    typedef void(__fastcall* fnScoreSubmit)(uintptr_t instance);
    static inline fnScoreSubmit oScoreSubmit = nullptr;
    static void __fastcall ScoreSubmitHook(uintptr_t instance);

    static inline bool m_IsPlayerLoaded = false;

    typedef int(__fastcall* fnOnPlayerLoadComplete)(uintptr_t instance, bool success);
    static inline fnOnPlayerLoadComplete oOnPlayerLoadComplete = nullptr;
    static int __fastcall OnPlayerLoadCompleteHook(uintptr_t instance, bool success);

    typedef void(__fastcall* fnPlayerDispose)(uintptr_t instance, int disposing);
    static inline fnPlayerDispose oPlayerDispose = nullptr;
    static void __fastcall PlayerDisposeHook(uintptr_t instance, int disposing);

    typedef void(__fastcall* fnSubmitError)(uintptr_t err);
    static inline fnSubmitError oSubmitError;
    static void __fastcall SubmitErrorHook(uintptr_t err);

    static void TryHookSetMousePosition(uintptr_t start = 0u, unsigned int size = 0);
    static void TryHookMouseViaKeyboardControls(uintptr_t start = 0u, unsigned int size = 0);
    static void TryHookScoreSubmit(uintptr_t start = 0u, unsigned int size = 0);
    static void TryHookOnPlayerLoadComplete(uintptr_t start = 0u, unsigned int size = 0);
    static void TryHookPlayerDispose(uintptr_t start = 0u, unsigned int size = 0);

    static void TryHookSubmitError(uintptr_t start = 0u, unsigned int size = 0);

    static inline int* m_PlayerFlag = nullptr;

    static void TryFindPlayerFlag(uintptr_t start = 0u, unsigned int size = 0);
    #pragma endregion

public:
    MapleBase() = default;

    void Initialize();

    void AddSDK(const std::shared_ptr<ISDK>& sdk);
    void AddSDKRange(const std::initializer_list<std::shared_ptr<ISDK>>& sdks);
    std::shared_ptr<ISDK> GetSDK(const std::string& name);

    void AddModule(const std::shared_ptr<IModule>& module);
    void AddModuleRange(const std::initializer_list<std::shared_ptr<IModule>>& modules);

    void RenderModulesGUI();

    void MakeScoreSubmissionUnsafe();
    bool GetIsScoreSubmissionUnsafe();

    std::shared_ptr<Logger> GetRuntimeLogger();
    std::shared_ptr<Vanilla> GetVanilla();
};
