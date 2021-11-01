#pragma once
#include <stack>
#include "Components/HitObjectOsu.h"
#include "../../Sdk/Player/HitObject.h"
#include "../../Sdk/Mods/ModManager.h"

namespace ReplayEditor
{
	enum EditorState
	{
		Paused,
		Playing
	};

	class Editor
	{
		static inline std::vector<Drawable> Drawables = std::vector<Drawable>();

		static inline int Time;
		static inline EditorState EditorState;
	public:
		static void DrawUI();
		static void ConstructDrawable(HitObject hitObject, float approachRate, Mods mods);
		static void Pause();
		static void Play();
		static void TimerThread();
	};
}