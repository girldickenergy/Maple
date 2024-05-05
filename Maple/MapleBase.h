#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "Vanilla.h"
#include "Numerics/Vector2.h"

#include "Logging/Logger.h"

class IModule;
class ISDK;

class MapleBase : public std::enable_shared_from_this<MapleBase>
{
    static inline std::shared_ptr<Storage> m_Storage = nullptr;
    static inline std::shared_ptr<Logger> m_RuntimeLogger = nullptr;
    static inline std::shared_ptr<Vanilla> m_Vanilla = nullptr;
    static inline std::unordered_map<uint32_t, std::shared_ptr<ISDK>> m_SDKs = {};
    static inline std::vector<std::shared_ptr<IModule>> m_Modules = {};

    static void InitializeStorage(const std::string& directoryName);
    static void InitializeLogging();
    static void InitializeCore();

    static void OnJIT(void* methodDesc, uintptr_t functionAddress, size_t functionSize);

    #pragma region Hooks for module callbacks
    static inline bool m_IsPlayerLoaded = false;

    typedef void(__fastcall* fnLoadComplete)(uintptr_t instance, bool success, bool runSuccessFunction);
    static inline fnLoadComplete oLoadComplete = nullptr;
    static void __fastcall LoadCompleteHook(uintptr_t instance, bool success, bool runSuccessFunction);

    static inline bool m_ScoreSubmissionUnsafe = false;

    typedef void(__fastcall* fnHandleScoreSubmission)(uintptr_t instance);
    static inline fnHandleScoreSubmission oHandleScoreSubmission = nullptr;
    static void __fastcall HandleScoreSubmissionHook(uintptr_t instance);

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

    static void TryHookLoadComplete(void* methodDesc = nullptr, uintptr_t functionAddress = 0u, size_t functionSize = 0u);
    static void TryHookHandleScoreSubmission(void* methodDesc = nullptr, uintptr_t functionAddress = 0u, size_t functionSize = 0u);
    static void TryHookSetMousePosition(void* methodDesc = nullptr, uintptr_t functionAddress = 0u, size_t functionSize = 0u);
    static void TryHookMouseViaKeyboardControls(void* methodDesc = nullptr, uintptr_t functionAddress = 0u, size_t functionSize = 0u);

    static inline int* m_PlayerFlag = nullptr;

    static void TryFindPlayerFlag(void* methodDesc = nullptr, uintptr_t functionAddress = 0u, size_t functionSize = 0u);
    #pragma endregion

public:
    MapleBase() = default;

    void Initialize();

    void AddSDK(const std::shared_ptr<ISDK>& sdk);
    void AddSDKRange(const std::initializer_list<std::shared_ptr<ISDK>>& sdks);
    std::shared_ptr<ISDK> GetSDK(const char* name);

    void AddModule(const std::shared_ptr<IModule>& module);
    void AddModuleRange(const std::initializer_list<std::shared_ptr<IModule>>& modules);

    void RenderModulesGUI();

    void MakeScoreSubmissionUnsafe();
    bool GetIsScoreSubmissionUnsafe();

    std::shared_ptr<Logger> GetRuntimeLogger();
    std::shared_ptr<Vanilla> GetVanilla();
};
