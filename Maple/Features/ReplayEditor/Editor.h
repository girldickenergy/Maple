#pragma once
#define NOMINMAX
#include <any>
#include <typeindex>
#include <stack>
#include <thread>
#include <imgui.h>
#include <locale>
#include <codecvt>
#include <string>
#include "Components/GameplayComponents/Drawable.h"
#include "Components/GameplayComponents/Osu/HitObjectOsu.h"
#include "Components/GameplayComponents/Osu/SliderOsu.h"
#include "Components/GameplayComponents/Osu/ApproachCircle.h"
#include "Components/GameplayComponents/Osu/OsuCursor.h"
#include "../../Sdk/Player/HitObject.h"
#include "../../Sdk/Mods/ModManager.h"
#include "../../Sdk/Osu/GameBase.h"
#include "../../Sdk/Osu/GameField.h"
#include "../../UI/Widgets/Widgets.h"
#include "../../UI/Widgets/3rd-party/FileDialog/imfilebrowser.h"
#include "../../Dependencies/Chiyo/Decoders/ReplayDecoder.h"
#include "../../Sdk/Beatmaps/Beatmap.h"
#include "Components/GUIComponents/ClickTimeline.h"
#include "Components/GUIComponents/EventTimeline.h"
#include "Components/GUIComponents/ClickOverlay.h"
#include "Playfields/OsuPlayfield.h"
#include "Drawing/DrawSlider.h"
#include "EditorGlobals.h"
#include "../../Sdk/Audio/AudioEngine.h"

#include "Singleton.h"
#include "Utilities/ReplayHandler.h"
#include "Utilities/HitObjectManagerWrapper.h"

namespace ReplayEditor
{
	enum LoadingStage
	{
		ParsingReplay,
		ParsingClicks,
		LoadingBeatmap,
		ConstructingDrawables
	};

	enum EditorState
	{
		Initializing,
		Paused,
		Playing
	};

	class Editor : public Singleton<Editor>
	{
		static inline bool isVisible = false;

		static inline bool fileDialogInitialized = false;
		static inline ImGui::FileBrowser fileDialog;
		static inline char replayPath[256];

		static inline ReplayHandler _replayHandler;
		static inline HitObjectManagerWrapper _hitObjectManagerWrapper;

		static inline int currentFrame;

		static inline int mouseX;
		static inline int mouseY;

		static inline std::vector<std::any> Drawables = std::vector<std::any>();

		static inline bool optionsOpen;
		static inline bool debugToolsOpen;
		static inline bool toolsOpen;

		// TODO: refactor this into a Tuple<PlayMode, void*> at some point!!!
		static inline PlayModes customHomPlayMode = PlayModes::Osu;

		static inline Beatmap bmap = Beatmap();
		static inline std::vector<HitObject> hitObjects = std::vector<HitObject>();

		static inline ImDrawList* drawList;

		static inline OsuPlayfield osuPlayfield = OsuPlayfield();

		static inline OsuCursor osuCursor = OsuCursor();
		static inline ClickTimeline clickTimeline = ClickTimeline();
		static inline EventTimeline eventTimeline = EventTimeline();
		static inline ClickOverlay clickOverlay = ClickOverlay();

		static inline int Time = -1;
		static inline EditorState EditorState = EditorState::Initializing;
		static inline ImVec2 playfieldOffset;
		static inline ImVec2 playfieldSize;

		static inline std::pair<Vector2, Vector2> dragArea;
		static inline std::vector<ReplayFrame> selectedFrames;

		static void DrawSelectedFrames(ImDrawList* drawList);
		static void HandleSelectionDrag();
		static inline bool Dragging = false;

		static inline ReplayFrame EditingFrame;
		static inline bool IsEditing;

		static void LoadBeatmap(std::string beatmapHash);

		static int64_t GetTicks();

		static void ForceUpdateCursorPosition();
	public:

		Editor(singletonLock);
		static void HandleInputs(int nCode, WPARAM wParam, LPARAM lParam);
		static void Initialize();
		static void Render();
		static void Pause();
		static void Play();
		static void TimerThread();

		static void ToggleVisibility();
		static bool GetIsVisible();

		ReplayHandler& GetReplayHandler();
		HitObjectManagerWrapper& GetHitObjectManagerWrapper();
	};
}