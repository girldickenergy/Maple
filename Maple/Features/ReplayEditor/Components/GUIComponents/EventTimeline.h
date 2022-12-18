#pragma once
#include <imgui.h>
#include "../../../../Sdk/DataTypes/Structs/sRectangle.h"
#include "../Chiyo/Replays/Replay.h"
#include "../../UI/StyleProvider.h"
#include "../../EditorGlobals.h"
#include "../../../../Sdk/Player/HitObject.h"
#include "../../../../Config/Config.h"
#include "Components/Click.h"
#include "../../../../Sdk/Player/HitObjectManager.h"

namespace ReplayEditor
{
	enum EventType
	{
		Miss,
		Fifty,
		OneHundred,
		ThreeHundred,
		Hit,
		Ignore,
		Notelock
	};

	class Event
	{
	public:
		int startTime;
		EventType eventType;
		Event(int _startTime, EventType _eventType)
		{
			startTime = _startTime;
			eventType = _eventType;
		}
	};

	class EventTimeline
	{
		int* timer;
		Replay* replay;
		std::vector<HitObject>* hitObjects;
		std::vector<Click> clicks;
		std::vector<Event> events;
		std::vector<bool> hits;
		int od;
		int cs;
		uintptr_t homInstance;

		EventType GetHitRange(int delta);
		int TimeToX(int time);
		bool ContainsPoint(HitObject ho, Vector2 point);
		EventType TestTimeMiss(HitObject ho, ReplayFrame frame, int hoIndex);
		bool TestHit(HitObject ho, ReplayFrame frame, int hoIndex);
	public:
		ImDrawList* drawList;
		sRectangle* clientBounds;
		int XToTime(int x);
		std::pair<Vector2, Vector2> EventTimelineLocation;
		bool IsInit;
		EventTimeline();
		EventTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, sRectangle* _clientBounds, std::vector<HitObject>* _hitObjects, int _od, int _cs, uintptr_t _homInstance);
		void SetHitObjects(std::vector<HitObject>* _hitObjects);
		void ParseEvents(std::vector<ReplayFrame> _replayFrames = std::vector<ReplayFrame>(), bool _otherMode = false);
		void Draw();
		void SetOverallDifficulty(int _od);
		void SetCircleSize(int _cs);
		void SetHomInstance(uintptr_t _homInstance);
	};
}