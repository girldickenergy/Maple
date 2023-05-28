#pragma once
#include <any>
#include <imgui.h>
#include <typeindex>
#include "../EditorGlobals.h"
#include "../Drawing/DrawSlider.h"
#include "../../../Sdk/Osu/GameBase.h"
#include "../../../Sdk/Osu/GameField.h"
#include "../../../Sdk/Beatmaps/Beatmap.h"
#include "../../../Sdk/Player/HitObject.h"
#include "../../../Sdk/Player/HitObjectManager.h"
#include "../../../Dependencies/Chiyo/Replays/Replay.h"
#include "../Components/GameplayComponents/Drawable.h"
#include "../Components/GameplayComponents/Transformation.h"
#include "../Components/GameplayComponents/Osu/OsuCursor.h"
#include "../Components/GameplayComponents/Osu/ApproachCircle.h"
#include "../Components/GameplayComponents/Osu/HitObjectOsu.h"
#include "../Components/GameplayComponents/Osu/SliderOsu.h"
#include "../Components/GameplayComponents/HitObjectScoring.h"

#include <Math/sRectangle.h>

namespace ReplayEditor
{
	/// @class OsuPlayfield
	/// @brief The class that houses the osu! playfield for the replay editor.
	class OsuPlayfield
	{
		/// @brief Pointer to the ImGui draw list context from the editor instance.
		ImDrawList* _drawList;
	
		/// @brief Pointer to the current selected replay.
		Replay* _replay;

		/// @brief Pointer to the current loaded beatmap.
		Beatmap* _beatmap;

		/// @brief Pointer to the self constructed hit object manager.
		uintptr_t _hitObjectManager;

		/// @brief Pointer to the timer from the editor instance.
		int* _timer;

		/// @brief Pointer to the current frame field from the editor instance.
		int* _currentFrame;

		/// @brief Pointer to vector holding all the currently loaded hit objects.
		std::vector<HitObject>* _hitObjects;

		/// @brief The client bounds struct from the gamebase.
		Vector2 _clientBounds;

		/// @brief Vector holding all the drawables meant to be displayed on screen.
		std::vector<OsuDrawable*> _drawables;

		/// @brief Osu cursor instance.
		OsuCursor _osuCursor;

		/// @brief Calculates the offset/position and size of the playarea on screen.
		void CalculatePlayareaCoordinates();

		/// @brief Calculates and constructs all the drawables that will be shown on screen.
		void ConstructDrawables();

		/// @brief Renders the background for the play area.
		void RenderPlayarea();

		/// @brief Renders the drawables that have to be drawn to the draw list context.
		void RenderDrawables();

		/// @brief Gets the hit range of the supplied delta.
		/// @param delta Delta between actual user hit input and the HitObject start time.
		HitObjectScoring getHitRange(int delta);

		/// @brief Tests whether or not the hit was a 300, 100, 50, NoteLock, etc...
		/// @param hitObject The HitObject the hit should be tested on.
		/// @param replayFrame The ReplayFrame of when the hit occured.
		/// @param hitObjectIndex The Index of the HitObject.
		HitObjectScoring testTimeMiss(HitObject hitObject, ReplayFrame replayFrame, int hitObjectIndex);

		/// @brief Tests whether or not the HitObject was hit in the ReplayFrame.
		/// @param hitObject The HitObject that should be tested.
		/// @param replayFrame The ReplayFrame where the hit occured.
		/// @param hitObjectIndex The Index of the HitObject.
		bool testHit(HitObject hitObject, ReplayFrame replayFrame, int hitObjectIndex);
	public:
		/// @brief A boolean value that can be checked to see if osu playfield has been initialized with values.
		bool _isInit;

		/// @brief Empty constructor for first initialization.
		OsuPlayfield();

		/// @brief Creates an instance of the osu playfield.
		/// @param drawList Pointer to the ImGui draw list context from the editor instance.
		/// @param replay Pointer to the current selected replay.
		/// @param beatmap Pointer to the current loaded beatmap.
		/// @param hitObjectManager Pointer to the self constructed hit object manager.
		/// @param timer Pointer to the timer from the editor instance.
		/// @param currentFrame Pointer to the current frame field from the editor instance.
		/// @param hitObjects Pointer to vector holding all the currently loaded hit objects.
		OsuPlayfield(ImDrawList* drawList, Replay* replay, Beatmap* beatmap, uintptr_t hitObjectManager, int* timer, int* currentFrame, std::vector<HitObject>* hitObjects);

		/// @brief Iterate through all HitObjects and calculate their Hit status.
		void CalculateHits();

		/// @brief Returns a vector of pointers to all osu drawables
		std::vector<ReplayEditor::OsuDrawable*> GetDrawables();

		/// @brief Renders the osu playfield instance to the editor screen.
		void Render();
	};
}