#pragma once
#include "../../../Dependencies/Chiyo/Decoders/ReplayDecoder.h"
#include <Math/Vector2.h>

namespace ReplayEditor
{
	class HitObjectManagerWrapper
	{
		uintptr_t _hitObjectManager;

		int _hitWindow50;
		int _hitWindow100;
		int _hitWindow300;
		int _preempt;
		float _hitObjectRadius;

		void cacheHitObjectManagerValues();
	public:
		HitObjectManagerWrapper();
		void ConstructNewHitObjectManager();
		void SetBeatmapAndLoad(uintptr_t beatmapInstance, Mods mods);

		uintptr_t& GetHitObjectManagerInstance();
		int& GetHitWindow50();
		int& GetHitWindow100();
		int& GetHitWindow300();
		int& GetPreempt();
		float& GetHitObjectRadius();
	};
}