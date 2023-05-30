#pragma once
#include "../Drawable.h"
#include "../HitObjectScoring.h"
#include "../../../EditorGlobals.h"

namespace ReplayEditor
{
	class OsuDrawable : public Drawable
	{
		using Drawable::Drawable;

		HitObjectScoring _hitObjectScoring;
		int _time;
	public:
		OsuDrawable();
		OsuDrawable(DrawableType drawableType, int* timer, int time, Vector2 position, Transformation transformation = Transformation(), int index = 0);
	
		HitObjectScoring GetHitObjectScoring();
		void SetHitObjectScoring(HitObjectScoring hitObjectScoring);

		void SetTime(int time);
		int GetTime();

		/// @brief Returns the color of the drawable based on the HitObjectScoring.
		ImU32 GetHitColor();
	};
}