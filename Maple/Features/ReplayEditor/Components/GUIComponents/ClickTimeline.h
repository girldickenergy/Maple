#pragma once
#include <imgui.h>
#include <Math/sRectangle.h>
#include "../Chiyo/Replays/Replay.h"
#include "../../UI/StyleProvider.h"
#include "../../EditorGlobals.h"
#include "../../../../Sdk/Player/HitObject.h"
#include "../../../../Config/Config.h"
#include "Click.h"
#include <iostream>

namespace ReplayEditor
{
	enum GetClickState : int
	{
		Forward,
		Backward
	};

	class ClickTimeline
	{
		int* timer;
		ImDrawList* drawList;
		Replay* replay;
		Vector2 clientBounds;
		std::vector<HitObject>* hitObjects;
		int TimeToX(int time);
		int XToTime(int x);
	public:
		std::vector<Click> clicks;
		int GetClick(GetClickState state);
		bool IsInit;
		std::pair<Vector2, Vector2> ClickTimelineLocation;
		ClickTimeline();
		ClickTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, Vector2 _clientBounds, std::vector<HitObject>* _hitObjects);
		void SetHitObjects(std::vector<HitObject>* _hitObjects);
		void ParseClicks();
		void HandleMouse(Vector2 _mousePos, bool _released);
		void Draw();
	};
}