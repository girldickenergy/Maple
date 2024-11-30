#pragma once

#include <tuple>
#include <vector>

#include "imgui.h"

class Enlighten
{
	static inline uintptr_t hitObjectManager = 0u;
	static inline bool hitObjectManagerLoaded = false;
	static inline bool hitObjectManagerReady = false;
	static inline bool isOverlayCurrentlyRendering = false;
	static inline bool isGamePressScheduled = false;

	static inline int preEmpt;
	static inline float preemtiveDotRadius;
	static inline std::vector<std::tuple<ImVec2, int>> preemptiveDots;
	
	static void initializeOverlay();
	static void initializePreemptiveDots();
public:
	static inline bool Enabled = false;

	static void Initialize();

	static void RenderOverlayBackground();
	static void RenderOverlay();
	static void RenderPreemptiveDots();

	static uintptr_t GetHitObjectManager();
	static bool GetIsOverlayCurrentlyRendering();
	static void ScheduleGamePress();
	static void DrawHitObjectManager();
};
