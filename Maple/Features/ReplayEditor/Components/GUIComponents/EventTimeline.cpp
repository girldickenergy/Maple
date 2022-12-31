#include "EventTimeline.h"

ReplayEditor::EventType ReplayEditor::EventTimeline::GetHitRange(int delta)
{
	if (delta <= HitObjectManager::GetHitWindow300(homInstance)) return ThreeHundred;
	if (delta <= HitObjectManager::GetHitWindow100(homInstance)) return OneHundred;
	if (delta <= HitObjectManager::GetHitWindow50(homInstance))  return Fifty;

	return Miss;
}

int ReplayEditor::EventTimeline::TimeToX(int time)
{
	return static_cast<int>((static_cast<float>(time) / replay->ReplayLength) * clientBounds->X);
}

int ReplayEditor::EventTimeline::XToTime(int x)
{
	return static_cast<int>((static_cast<float>(x) / clientBounds->X) * replay->ReplayLength);
}

ReplayEditor::EventType ReplayEditor::EventTimeline::TestTimeMiss(HitObject ho, ReplayFrame frame, int hoIndex)
{
	auto preempt = HitObjectManager::GetPreEmpt(reinterpret_cast<void*>(homInstance));
	if ((ho.IsType(HitObjectType::Normal) || ho.IsType(HitObjectType::Slider)) && hoIndex > 0)
	{
		auto& previousObj = (*hitObjects)[hoIndex - 1];
		if (previousObj.StackCount > 0 &&
			(frame.Time >= previousObj.StartTime - preempt &&
				frame.Time <= previousObj.EndTime + 240) &&
			!previousObj.IsHit)
			return EventType::Ignore;
	}

	int hitWindow50 = HitObjectManager::GetHitWindow50(homInstance);

	bool isNextCircle = true;

	for (auto it = hitObjects->begin(); it != hitObjects->begin() + hoIndex; ++it)
	{
		auto& h = *it;
		if (h.StartTime + hitWindow50 <= frame.Time || h.IsHit)
			continue;
		if (h.StartTime < ho.StartTime && std::distance(it, hitObjects->begin() + hoIndex) != hoIndex)
			isNextCircle = false;
		break;
	}

	/*for (int i = 0; i < hitObjects->size(); i++)
	{
		auto& h = (*hitObjects)[i];
		if (h.StartTime + hitWindow50 <= frame.Time || h.IsHit)
			continue;
		if (h.StartTime < ho.StartTime && i != hoIndex)
			isNextCircle = false;
		break;
	}*/

	if (isNextCircle && std::abs(ho.StartTime - frame.Time) < 400)
		return EventType::Hit;

	return EventType::Notelock;
}

bool ReplayEditor::EventTimeline::TestHit(HitObject ho, ReplayFrame frame, int hoIndex)
{
	auto radius = HitObjectManager::GetHitObjectRadius(homInstance);
	int hitWindow50 = HitObjectManager::GetHitWindow50(homInstance);
	auto preempt = HitObjectManager::GetPreEmpt(homInstance);
	if ((ho.StartTime - preempt <= frame.Time && ho.StartTime + hitWindow50 >= frame.Time && !ho.IsHit) &&
		(Vector2(frame.X, frame.Y).DistanceSquared(ho.Position) <= radius * radius))
		return true;
	return false;
}

ReplayEditor::EventTimeline::EventTimeline()
{ }

ReplayEditor::EventTimeline::EventTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, Vector2* _clientBounds, std::vector<HitObject>* _hitObjects, int _od, int _cs, uintptr_t _homInstance)
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

void ReplayEditor::EventTimeline::ParseEvents(std::vector<ReplayFrame> _replayFrames, bool _otherMode)
{
	auto rf = (_replayFrames.empty() ? replay->ReplayFrames : _replayFrames);
	std::vector<bool> used = std::vector<bool>(rf.size(), false);
	events = std::vector<ReplayEditor::Event>();
	events.clear();

	auto previousObject = hitObjects->begin();
	auto replayFrame = rf.begin();

	/*for (auto it = hitObjects->begin(); it != hitObjects->begin() + hoIndex; ++it)
	{
		auto& h = *it;
		if (h.StartTime + hitWindow50 <= frame.Time || h.IsHit)
			continue;
		if (h.StartTime < ho.StartTime && std::distance(it, hitObjects->begin() + hoIndex) != hoIndex)
			isNextCircle = false;
		break;
	}*/
	if (!_otherMode)
	{
		auto frame = rf.begin() + 1; // First frame cannot be click
		for (auto current = hitObjects->begin(); current != hitObjects->end(); ++current) {
			auto& ho = *current;

			if (ho.IsType(HitObjectType::Spinner)) continue;

			ho.IsHit = false;

			// We can advance replay frames until we're sure the current object isn't clicked
			// This won't miss future objects because of notelock
			// Otherwise, we check against the next object starting from the next frame
			// This may not work for double clicking if both keys go down on the same frame, I guess?
			for (; frame != rf.end() && frame->Time <= ho.StartTime + HitObjectManager::GetHitWindow50(homInstance); ++frame) {
				const auto prevFrame = *(frame - 1);
				OsuKeys ok = static_cast<OsuKeys>((int)frame->OsuKeys & ~(int)prevFrame.OsuKeys);
				if (((ok & OsuKeys::K1) > OsuKeys::None || (ok & OsuKeys::K2) > OsuKeys::None) && TestHit(ho, *frame, ho.Count)) {
					int accuracy = std::abs(frame->Time - ho.StartTime);
					EventType determinedRange = EventTimeline::GetHitRange(accuracy);

					events.emplace_back(frame->Time, determinedRange);
					if (determinedRange == EventType::OneHundred) ho.Is100 = true;
					else if (determinedRange == EventType::Fifty) ho.Is50 = true;

					ho.IsHit = true;

					// Frame used
					++frame;
					break;
				}
			}
		}
	}
	
	for(auto& ho : *hitObjects) 
		if (!ho.IsHit && !ho.IsType(HitObjectType::Spinner))
			events.emplace_back(ho.StartTime, EventType::Miss);
		else if (ho.IsType(HitObjectType::Spinner))
			events.emplace_back(ho.StartTime, EventType::ThreeHundred);
			
	IsInit = true;
}

void ReplayEditor::EventTimeline::Draw()
{
	float eventTimelineHeight = (PERC(clientBounds->Y, 3.f) * StyleProvider::Scale) - 2;
	float clickTimelineHeight = PERC(clientBounds->Y, 4.f) * StyleProvider::Scale;
	
	// Upper border 
	drawList->AddRectFilled(ImVec2(0, clientBounds->Y - clickTimelineHeight - eventTimelineHeight - 2), ImVec2(clientBounds->X, clientBounds->Y - clickTimelineHeight - eventTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

	drawList->AddRectFilled(ImVec2(0, clientBounds->Y - clickTimelineHeight - eventTimelineHeight), ImVec2(clientBounds->X, clientBounds->Y - clickTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(71.f), COL(71.f), COL(71.f), 1.f)));

	// Under border
	drawList->AddRectFilled(ImVec2(0, clientBounds->Y - clickTimelineHeight - 2), ImVec2(clientBounds->X, clientBounds->Y - clickTimelineHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

	EventTimelineLocation = std::make_pair(Vector2(0, clientBounds->Y - clickTimelineHeight - eventTimelineHeight), Vector2(clientBounds->X, clientBounds->Y - clickTimelineHeight));

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
					col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(0.f), COL(255.f), 1.f));
					break;
				case EventType::OneHundred:
					col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(255.f), COL(0.f), 1.f));
					break;
			}
			drawList->AddRectFilled(ImVec2(x - .5f, clientBounds->Y - clickTimelineHeight - eventTimelineHeight), ImVec2(x + .5f, clientBounds->Y - clickTimelineHeight - 2), col);
		}
	}

	int timeX = EventTimeline::TimeToX(*timer);
	drawList->AddRect(ImVec2(timeX - 2.f, clientBounds->Y - clickTimelineHeight - eventTimelineHeight), ImVec2(timeX + 2.f, clientBounds->Y - clickTimelineHeight - 2), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 0.5f)));
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