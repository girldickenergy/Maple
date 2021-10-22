#include "Timewarp.h"

#include <Vanilla.h>

#include "../../Config/Config.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Player/Player.h"
#include "../../Sdk/Player/Ruleset.h"

double __fastcall Timewarp::audioTrackBass_GetPlaybackRateStub(void* instance)
{
	if (*static_cast<uintptr_t*>(audioTrackBass_GetPlaybackRateReturnAddress) >= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) && 
        *static_cast<uintptr_t*>(audioTrackBass_GetPlaybackRateReturnAddress) <= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) + Vanilla::GetModuleSize("clr.dll"))
        return ModManager::ModPlaybackRate();
	
    return oAudioTrackBass_GetPlaybackRate(instance);
}

double __fastcall Timewarp::audioTrackVirtual_GetPlaybackRateStub(void* instance)
{
    if (*static_cast<uintptr_t*>(audioTrackVirtual_GetPlaybackRateReturnAddress) >= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) &&
        *static_cast<uintptr_t*>(audioTrackVirtual_GetPlaybackRateReturnAddress) <= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) + Vanilla::GetModuleSize("clr.dll"))
        return ModManager::ModPlaybackRate();

    return oAudioTrackVirtual_GetPlaybackRate(instance);
}

void Timewarp::patchTickrate()
{
    Vanilla::Explorer["osu.GameBase"]["UpdateTiming"].Method.Compile();
    const uintptr_t entryPoint = reinterpret_cast<uintptr_t>(Vanilla::Explorer["osu.GameBase"]["UpdateTiming"].Method.GetNativeStart());

    const uintptr_t tickrate1 = Vanilla::FindSignature("\xDD\x05\x00\x00\x00\x00\xDC\x25\x00\x00\x00\x00\xD9\xC0\xDD\x05", "xx????xx????xxxx", entryPoint, 0x28E) + 0x10;
    const uintptr_t tickrate2 = Vanilla::FindSignature("\x1D\x00\x00\x00\x00\xDD\x05\x00\x00\x00\x00\xDC\x25", "x????xx????xx", entryPoint, 0x28E) + 0xD;
    const uintptr_t tickrate3 = Vanilla::FindSignature("\xDD\x05\x00\x00\x00\x00\xDD\x05\x00\x00\x00\x00\xDC\x25", "xx????xx????xx", entryPoint, 0x28E) + 0xE;

    *reinterpret_cast<double**>(tickrate1) = &tickrate;
    *reinterpret_cast<double**>(tickrate2) = &tickrate;
    *reinterpret_cast<double**>(tickrate3) = &tickrate;
}

void Timewarp::TimewarpThread()
{
    patchTickrate();
	
	while (true)
	{
        if (Player::IsLoaded() && !Player::IsReplayMode())
        {
            if (Config::Timewarp::Enabled)
            {
                if (round(AudioEngine::GetPlaybackRate()) != round(static_cast<double>(Config::Timewarp::Rate)))
                {
                    tickrate = 1000. / 60. * (1. / GetRateMultiplier());
                    AudioEngine::SetPlaybackRate(round(static_cast<double>(Config::Timewarp::Rate)));
                }

                if (Player::PlayMode() == PlayModes::Catch && roundf(Ruleset::GetCatcherSpeed()) != roundf(static_cast<float>(Config::Timewarp::Rate) / 100.f))
                    Ruleset::SetCatcherSpeed(static_cast<float>(Config::Timewarp::Rate) / 100.f);
            }
            else
            {
                if (round(AudioEngine::GetPlaybackRate()) != round(ModManager::ModPlaybackRate()))
                {
                    tickrate = 1000. / 60.;
                    AudioEngine::SetPlaybackRate(ModManager::ModPlaybackRate());
                }

                if (Player::PlayMode() == PlayModes::Catch && roundf(Ruleset::GetCatcherSpeed()) != roundf(static_cast<float>(ModManager::ModPlaybackRate()) / 100.f))
                    Ruleset::SetCatcherSpeed(static_cast<float>(ModManager::ModPlaybackRate()) / 100.f);
            }
        }
        else tickrate = 1000. / 60.;

        Sleep(100);
	}
}

double Timewarp::GetRateMultiplier()
{
    return static_cast<double>(Config::Timewarp::Rate) / ModManager::ModPlaybackRate();
}

__declspec(naked) double Timewarp::AudioTrackBass_GetPlaybackRateHook(void* instance)
{
    __asm
    {
        add esp, 0x8
        mov[audioTrackBass_GetPlaybackRateReturnAddress], esp
        sub esp, 0x8
        jmp audioTrackBass_GetPlaybackRateStub
    }
}

__declspec(naked) double Timewarp::AudioTrackVirtual_GetPlaybackRateHook(void* instance)
{
    __asm
    {
        add esp, 0x8
        mov[audioTrackVirtual_GetPlaybackRateReturnAddress], esp
        sub esp, 0x8
        jmp audioTrackVirtual_GetPlaybackRateStub
    }
}

void __fastcall Timewarp::AddParameterHook(void* instance, COMString* name, COMString* value)
{
    if (name->Data() == L"st" && Config::Timewarp::Enabled)
    {
        const int newValue = static_cast<int>(std::stod(value->Data().data()) * GetRateMultiplier());

        wchar_t buf[16];
        swprintf_s(buf, 16, L"%d", newValue);

        oAddParameter(instance, name, COMString::CreateString(buf));
    }
    else
		oAddParameter(instance, name, value);
}
