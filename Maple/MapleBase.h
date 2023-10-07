#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Vanilla.h"

class IModule;
class ISDK;

class MapleBase : public std::enable_shared_from_this<MapleBase>
{
    static inline std::unordered_map<std::string, std::shared_ptr<ISDK>> m_SDKs = {};
    static inline std::vector<std::shared_ptr<IModule>> m_Modules = {};
    std::shared_ptr<Vanilla> m_Vanilla = nullptr;

    typedef void(__fastcall* fnUpdateIsPlaying)();
    static inline fnUpdateIsPlaying oUpdateIsPlaying = nullptr;
    static void __fastcall UpdateIsPlayingHook();

    static void OnJIT(uintptr_t address, unsigned int size);

public:
    MapleBase() = default;

    void Initialize();

    void AddSDK(const std::string& name, const std::shared_ptr<ISDK>& sdk);
    void AddSDKRange(const std::initializer_list<std::pair<std::string, std::shared_ptr<ISDK>>>& sdks);
    std::shared_ptr<ISDK> GetSDK(const std::string& name);

    void AddModule(const std::shared_ptr<IModule>& module);
    void AddModuleRange(const std::initializer_list<std::shared_ptr<IModule>>& modules);

    void RenderModulesGUI();

    std::shared_ptr<Vanilla> GetVanilla();
};
