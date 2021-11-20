#include "Timewarp.h"

#include <ThemidaSDK.h>
#include <Vanilla.h>


#include "../../Communication/Communication.h"
#include "../../Config/Config.h"
#include "../../Sdk/Audio/AudioEngine.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Player/Player.h"
#include "../../Sdk/Player/Ruleset.h"
#include "../../Utilities/Security/Security.h"

double __fastcall Timewarp::getCurrentPlaybackRateStub()
{
    if (*static_cast<uintptr_t*>(getCurrentPlaybackRateReturnAddress) >= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) &&
        *static_cast<uintptr_t*>(getCurrentPlaybackRateReturnAddress) <= reinterpret_cast<uintptr_t>(GetModuleHandleA("clr.dll")) + Vanilla::GetModuleSize("clr.dll"))
        return ModManager::ModPlaybackRate();

    return oGetCurrentPlaybackRate();
}

void Timewarp::Initialize()
{
    Vanilla::Explorer["osu.GameBase"]["UpdateTiming"].Method.Compile();
    const uintptr_t entryPoint = reinterpret_cast<uintptr_t>(Vanilla::Explorer["osu.GameBase"]["UpdateTiming"].Method.GetNativeStart());

    const uintptr_t tickrate1 = Vanilla::FindSignature("\xDD\x05\x00\x00\x00\x00\xDC\x25\x00\x00\x00\x00\xD9\xC0\xDD\x05", "xx????xx????xxxx", entryPoint, 0x28E) + 0x10;
    const uintptr_t tickrate2 = Vanilla::FindSignature("\x1D\x00\x00\x00\x00\xDD\x05\x00\x00\x00\x00\xDC\x25", "x????xx????xx", entryPoint, 0x28E) + 0xD;
    const uintptr_t tickrate3 = Vanilla::FindSignature("\xDD\x05\x00\x00\x00\x00\xDD\x05\x00\x00\x00\x00\xDC\x25", "xx????xx????xx", entryPoint, 0x28E) + 0xE;
    const uintptr_t tickrate4 = Vanilla::FindSignature("\xDD\x1D\x00\x00\x00\x00\xDD\x05\x00\x00\x00\x00\xDC\x35", "xx????xx????xx", entryPoint, 0x28E) + 0xE;

    *reinterpret_cast<double**>(tickrate1) = &tickrate;
    *reinterpret_cast<double**>(tickrate2) = &tickrate;
    *reinterpret_cast<double**>(tickrate3) = &tickrate;
    *reinterpret_cast<double**>(tickrate4) = &tickrate;
}

void Timewarp::UpdateCatcherSpeed()
{
    VM_SHARK_BLACK_START

    DWORD check1 = 0x2F47C114;
    CHECK_CODE_INTEGRITY(check1, 0xC0CEA1FA);
    if (check1 == 0x2F47C114)
        Security::CorruptMemory();

    Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle, true);

    VM_SHARK_BLACK_END
	
    if (Player::PlayMode() == PlayModes::Catch)
        Ruleset::SetCatcherSpeed(static_cast<float>(Config::Timewarp::Enabled ? Config::Timewarp::Rate : ModManager::ModPlaybackRate()) / 100.f);
}

double Timewarp::GetRateMultiplier()
{
    return static_cast<double>(Config::Timewarp::Rate) / ModManager::ModPlaybackRate();
}

void __fastcall Timewarp::SetCurrentPlaybackRateHook(double rate)
{
    if (Config::Timewarp::Enabled && GameBase::Mode() == OsuModes::Play && Player::Instance() && !Player::IsReplayMode())
    {
        rate = Config::Timewarp::Rate;
        tickrate = 1000. / 60. * (1. / GetRateMultiplier());
    }
    else tickrate = 1000. / 60.;

    oSetCurrentPlaybackRate(rate);
}

double __fastcall Timewarp::GetCurrentPlaybackRateHook()
{
    __asm
    {
        mov[getCurrentPlaybackRateReturnAddress], esp
        jmp getCurrentPlaybackRateStub
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
