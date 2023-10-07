#include "MapleBase.h"

#include "xorstr.h"

#include "Modules/IModule.h"
#include "SDK/ISDK.h"

void __fastcall MapleBase::UpdateIsPlayingHook()
{
    for (const std::shared_ptr<IModule>& mod : m_Modules)
        mod->OnUpdate();

    oUpdateIsPlaying();
}

void MapleBase::OnJIT(uintptr_t address, unsigned int size)
{
    for (const std::pair<std::string, std::shared_ptr<ISDK>> sdk : m_SDKs)
        sdk.second->OnJIT(address, size);
}

void MapleBase::Initialize()
{
    m_Vanilla = std::make_shared<Vanilla>();
    m_Vanilla->Initialize(true);
    m_Vanilla->SetJITCallback(OnJIT);

    if (uintptr_t updateIsPlaying = m_Vanilla->GetPatternScanner().FindPattern(xorstr_("FF 15 ?? ?? ?? ?? 0F B6 05 ?? ?? ?? ?? A2 ?? ?? ?? ?? 80 3D")))
    {
        while (*reinterpret_cast<uint16_t*>(updateIsPlaying) != 0x15FF || *reinterpret_cast<uint16_t*>(updateIsPlaying + 0x6) != 0x15FF)
            updateIsPlaying++;

        updateIsPlaying += 2;

        oUpdateIsPlaying = **reinterpret_cast<fnUpdateIsPlaying**>(updateIsPlaying);
        **reinterpret_cast<uintptr_t**>(updateIsPlaying) = reinterpret_cast<uintptr_t>(UpdateIsPlayingHook);
    }

    // todo: error handling
}

void MapleBase::AddSDK(const std::string& name, const std::shared_ptr<ISDK>& sdk)
{
    if (!m_SDKs.contains(name))
    {
        sdk->OnLoad(shared_from_this());
        m_SDKs[name] = sdk;
    }
}

void MapleBase::AddSDKRange(const std::initializer_list<std::pair<std::string, std::shared_ptr<ISDK>>>& sdks)
{
    for (const std::pair<std::string, std::shared_ptr<ISDK>> sdk : sdks)
        AddSDK(sdk.first, sdk.second);
}

std::shared_ptr<ISDK> MapleBase::GetSDK(const std::string& name)
{
    return m_SDKs.contains(name) ? m_SDKs[name] : nullptr;
}

void MapleBase::AddModule(const std::shared_ptr<IModule>& module)
{
    module->OnLoad(shared_from_this());
    m_Modules.push_back(module);
}

void MapleBase::AddModuleRange(const std::initializer_list<std::shared_ptr<IModule>>& modules)
{
    for (const std::shared_ptr<IModule>& module : modules)
        AddModule(module);
}

void MapleBase::RenderModulesGUI()
{
    for (const std::shared_ptr<IModule>& module : m_Modules)
        module->OnRender();
}

std::shared_ptr<Vanilla> MapleBase::GetVanilla()
{
    return m_Vanilla;
}
