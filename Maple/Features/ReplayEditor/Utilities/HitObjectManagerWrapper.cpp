#include "HitObjectManagerWrapper.h"
#include "../../../Sdk/Player/HitObjectManager.h"
#include "../../../Sdk/Player/Ruleset.h"
#include <Vanilla.h>

ReplayEditor::HitObjectManagerWrapper::HitObjectManagerWrapper()
{
    _hitObjectManager = NULL;
}

void ReplayEditor::HitObjectManagerWrapper::cacheHitObjectManagerValues()
{
    _hitWindow50 = HitObjectManager::GetHitWindow50(_hitObjectManager);
    _hitWindow100 = HitObjectManager::GetHitWindow100(_hitObjectManager);
    _hitWindow300 = HitObjectManager::GetHitWindow300(_hitObjectManager);
    _preempt = HitObjectManager::GetPreEmpt(_hitObjectManager);
    _hitObjectRadius = HitObjectManager::GetHitObjectRadius(_hitObjectManager);
}

void ReplayEditor::HitObjectManagerWrapper::ConstructNewHitObjectManager()
{
    Vanilla::RemoveRelocation(std::ref(_hitObjectManager));

    _hitObjectManager = Ruleset::CreateHitObjectManager(NULL);

    Vanilla::AddRelocation(std::ref(_hitObjectManager));
}

void ReplayEditor::HitObjectManagerWrapper::SetBeatmapAndLoad(uintptr_t beatmapInstance, Mods mods)
{
    HitObjectManager::SetBeatmap(_hitObjectManager, beatmapInstance, mods);
    HitObjectManager::Load(_hitObjectManager, true, 0);

    cacheHitObjectManagerValues();
}

uintptr_t& ReplayEditor::HitObjectManagerWrapper::GetHitObjectManagerInstance()
{
    return _hitObjectManager;
}

int& ReplayEditor::HitObjectManagerWrapper::GetHitWindow50()
{
    return _hitWindow50;
}

int& ReplayEditor::HitObjectManagerWrapper::GetHitWindow100()
{
    return _hitWindow100;
}

int& ReplayEditor::HitObjectManagerWrapper::GetHitWindow300()
{
    return _hitWindow300;
}

int& ReplayEditor::HitObjectManagerWrapper::GetPreempt()
{
    return _preempt;
}

float& ReplayEditor::HitObjectManagerWrapper::GetHitObjectRadius()
{
    return _hitObjectRadius;
}