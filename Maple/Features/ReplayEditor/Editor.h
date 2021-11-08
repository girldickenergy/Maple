#pragma once
#include <stack>
#include "Components/Drawable.h"
#include "Components/HitObjectOsu.h"
#include "Components/ApproachCircle.h"
#include "../../Sdk/Player/HitObject.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Dependencies/ImGui/imgui.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Osu/GameField.h"

namespace ReplayEditor
{
	enum EditorState
	{
		Initializing,
		Paused,
		Playing
	};

	class Editor
	{
		static inline std::vector<Drawable> Drawables = std::vector<Drawable>();

		static inline int Time;
		static inline EditorState EditorState = EditorState::Initializing;
		static inline ImVec2 playfieldOffset;
		static inline ImVec2 playfieldSize;

		static Vector2 ConvertToPlayArea(Vector2 position);
		static void DrawCursor(ImDrawList* drawList, Vector2 cursorPosition);
	public:
		static void Initialize();
		static void DrawUI();
		static void ConstructDrawable(HitObject hitObject, float approachRate, float overallDifficulty, Mods mods);
		static void Pause();
		static void Play();
		static void TimerThread();
	};
}