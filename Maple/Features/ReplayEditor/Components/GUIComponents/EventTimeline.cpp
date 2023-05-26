#include "EventTimeline.h"

int ReplayEditor::EventTimeline::TimeToX(int time)
{
	return static_cast<int>((static_cast<float>(time) / replay->ReplayLength) * clientBounds.X);
}

int ReplayEditor::EventTimeline::XToTime(int x)
{
	if (clientBounds.X < 0)
		return 0;
	return static_cast<int>((static_cast<float>(x) / clientBounds.X) * replay->ReplayLength);
}

ReplayEditor::EventTimeline::EventTimeline()
{ }

ReplayEditor::EventTimeline::EventTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, Vector2 _clientBounds, std::vector<HitObject>* _hitObjects, int _od, int _cs, uintptr_t _homInstance)
{
	timer = _timer;
	drawList = _drawList;
	replay = _replay;
	clientBounds = _clientBounds;
	hitObjects = _hitObjects;
	od = _od;
	cs = _cs;
	homInstance = _homInstance;
	hits = std::vector<bool>(420, false);
}

void ReplayEditor::EventTimeline::SetHitObjects(std::vector<HitObject>* _hitObjects)
{
	hitObjects = _hitObjects;
	hits = std::vector<bool>(hitObjects->size(), false);
}

void ReplayEditor::EventTimeline::ParseEvents(std::vector<std::pair<int, HitObjectScoring>> hits)
{
	events = std::vector<ReplayEditor::Event>();
	events.clear();

	for (auto current = hits.begin(); current != hits.end(); ++current)
	{
		switch (current->second)
		{
			case HitObjectScoring::Miss:
				events.emplace_back(current->first, EventType::Miss);
				break;
			case HitObjectScoring::Fifty:
				events.emplace_back(current->first, EventType::Fifty);
				break;
			case HitObjectScoring::OneHundred:
				events.emplace_back(current->first, EventType::OneHundred);
				break;
			case HitObjectScoring::ThreeHundred:
				events.emplace_back(current->first, EventType::ThreeHundred);
				break;
		}
	}

	IsInit = true;
}

void ReplayEditor::EventTimeline::Draw()
{
	float eventTimelineHeight = (PERC(clientBounds.Y, 3.f) * StyleProvider::Scale) - 2;
	float clickTimelineHeight = PERC(clientBounds.Y, 4.f) * StyleProvider::Scale;

	// Upper border 
	drawList->AddRectFilled(ImVec2(0, clientBounds.Y - clickTimelineHeight - eventTimelineHeight - 2), ImVec2(clientBounds.X, clientBounds.Y - clickTimelineHeight - eventTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

	drawList->AddRectFilled(ImVec2(0, clientBounds.Y - clickTimelineHeight - eventTimelineHeight), ImVec2(clientBounds.X, clientBounds.Y - clickTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(71.f), COL(71.f), COL(71.f), 1.f)));

	// Under border
	drawList->AddRectFilled(ImVec2(0, clientBounds.Y - clickTimelineHeight - 2), ImVec2(clientBounds.X, clientBounds.Y - clickTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

	EventTimelineLocation = std::make_pair(Vector2(0, clientBounds.Y - clickTimelineHeight - eventTimelineHeight), Vector2(clientBounds.X, clientBounds.Y - clickTimelineHeight));

	// The click timeline should have twice resolution! i.e: it's 1920 wide, so the time resolution should be 1920*2 ms, or 1920 in either direction.
	if (hitObjects == nullptr) return;

	for (auto& e : events)
	{
		if (e.eventType != EventType::ThreeHundred) {
			int x = EventTimeline::TimeToX(e.startTime);
			ImU32 col;
			switch (e.eventType)
			{
			case EventType::Miss:
				col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(0.f), COL(0.f), 1.f));
				break;
			case EventType::Fifty:
				col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(230.f), COL(170.f), 1.f));
				break;
			case EventType::OneHundred:
				col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(255.f), COL(0.f), 1.f));
				break;
			}
			drawList->AddRectFilled(ImVec2(x - .5f, clientBounds.Y - clickTimelineHeight - eventTimelineHeight), ImVec2(x + .5f, clientBounds.Y - clickTimelineHeight - 2), col);
		}
	}

	int timeX = EventTimeline::TimeToX(*timer);
	drawList->AddRect(ImVec2(timeX - 2.f, clientBounds.Y - clickTimelineHeight - eventTimelineHeight), ImVec2(timeX + 2.f, clientBounds.Y - clickTimelineHeight - 2), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 0.5f)));
}

void ReplayEditor::EventTimeline::SetOverallDifficulty(int _od)
{
	od = _od;
}

void ReplayEditor::EventTimeline::SetCircleSize(int _cs)
{
	cs = _cs;
}

void ReplayEditor::EventTimeline::SetHomInstance(uintptr_t _homInstance)
{
	homInstance = _homInstance;
}