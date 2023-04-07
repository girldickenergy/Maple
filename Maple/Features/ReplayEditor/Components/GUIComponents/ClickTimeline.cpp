
#include "ClickTimeline.h"

ReplayEditor::ClickTimeline::ClickTimeline()
{ }

ReplayEditor::ClickTimeline::ClickTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, Vector2 _clientBounds, std::vector<HitObject>* _hitObjects)
{
	timer = _timer;
	drawList = _drawList;
	replay = _replay;
	clientBounds = _clientBounds;
	hitObjects = _hitObjects;
}

int ReplayEditor::ClickTimeline::TimeToX(int time)
{
	auto width = static_cast<float>(clientBounds.X);
	auto msPerScreen = (width * 2) * Config::ReplayEditor::TimelineResolution;
	auto pixelsPerMs = width / msPerScreen;

	auto screenX = (time - *timer) * pixelsPerMs;
	return static_cast<int>(screenX);
}

int ReplayEditor::ClickTimeline::XToTime(int x)
{
	//var inputXPosition; // assuming 0 for middle of screen
	//var currentTime;

	//var relativeTime = inputXPosition * msPerPixel; // or `/ pixelsPerMs`, whichever one you have
	//var absTime = currentTime + relativeTime;

	auto width = static_cast<float>(clientBounds.X);
	auto msPerScreen = (width * 2) * Config::ReplayEditor::TimelineResolution;
	auto pixelsPerMs = width / msPerScreen;

	auto relativeTime = (x - clientBounds.X / 2) / pixelsPerMs;

	return static_cast<int>(*timer + relativeTime);
}

int ReplayEditor::ClickTimeline::GetClick(GetClickState state)
{
	Click c = Click();
	if (state == GetClickState::Forward)
	{
		for (auto& click : clicks)
			if (click._startTime > *timer) {
				c = click;
				break;
			}
	}
	else if (state == GetClickState::Backward)
	{
		std::vector<Click> cl = std::vector<Click>();
		for (auto& clck : clicks)
			if (clck._startTime < *timer)
				cl.push_back(clck);
		c = cl.back();
	}
	return c._startTime;
}

void ReplayEditor::ClickTimeline::SetHitObjects(std::vector<HitObject>* _hitObjects)
{
	hitObjects = _hitObjects;
}

void ReplayEditor::ClickTimeline::ParseClicks()
{
	// TODO: this is very ugly code and bad practice, but it makes both keys at the same time work. make this prettier
	for (int i = 0; i < replay->ReplayFrames.size(); i++)
	{
		auto& frame = replay->ReplayFrames[i];
		if ((frame.OsuKeys & OsuKeys::K1) > OsuKeys::None)
		{
			Click c = Click(frame.Time, Vector2(frame.X, frame.Y), 0, OsuKeys::K1);
			while ((replay->ReplayFrames[++i].OsuKeys & OsuKeys::K1) > OsuKeys::None)
				c._duration = replay->ReplayFrames[i].Time - c._startTime;
			if (c._duration == 0)
				c._duration = replay->ReplayFrames[i].Time - c._startTime;

			clicks.push_back(c);
		}
	}

	for (int i = 0; i < replay->ReplayFrames.size(); i++)
	{
		auto& frame = replay->ReplayFrames[i];
		if ((frame.OsuKeys & OsuKeys::K2) > OsuKeys::None)
		{
			Click c = Click(frame.Time, Vector2(frame.X, frame.Y), 0, OsuKeys::K2);
			while ((replay->ReplayFrames[++i].OsuKeys & OsuKeys::K2) > OsuKeys::None)
				c._duration = replay->ReplayFrames[i].Time - c._startTime;
			if (c._duration == 0)
				c._duration = replay->ReplayFrames[i].Time - c._startTime;

			clicks.push_back(c);
		}
	}

	// remove all clicks with a duration of 0
	std::vector<int> toRemove = std::vector<int>();
	for (int i = 0; i < clicks.size(); i++)
		if (clicks[i]._duration == 0)
			toRemove.push_back(i);
	for (auto& rem : toRemove)
		clicks.erase(clicks.begin() + rem);

	IsInit = true;
}

void ReplayEditor::ClickTimeline::HandleMouse(Vector2 _mousePos, bool _released)
{
	float clickTimelineHeight = PERC(clientBounds.Y, 4.f) * StyleProvider::Scale;

	static bool secondCall = false;

	static Click previousClick = Click();
	static int clickIndex = 0;
	static Click nextClick = Click();
	static bool left, mid, right;
	// Find dragged click
	if (!secondCall)
		for (int i = 0; i < clicks.size(); i++)
		{
			auto& click = clicks[i];
			auto var = TimeToX(click._startTime);
			if (var * Config::ReplayEditor::TimelineResolution >= -(clientBounds.X * 2) && var < clientBounds.X)
				if (click._keys == OsuKeys::K1)
				{
					if (i > 0)
					{
						int intermediary = 1;
						while ((i - intermediary > 0) && clicks[i - intermediary]._keys != click._keys)
							intermediary++;
						previousClick = clicks[i - intermediary];
					}
					if (i < clicks.size() - 1)
					{
						int intermediary = 1;
						while ((i + intermediary < clicks.size() - 1) && clicks[i + intermediary]._keys != click._keys)
							intermediary++;
						nextClick = clicks[i + intermediary];
					}

					Vector2 clickTopLeft = Vector2((clientBounds.X / 2) + var, clientBounds.Y - clickTimelineHeight);
					Vector2 clickBottomRight = Vector2((clientBounds.X / 2) + ((static_cast<float>(click._duration) / 2) /
						Config::ReplayEditor::TimelineResolution) + var, clientBounds.Y - (clickTimelineHeight / 2));
					if (_mousePos.X >= clickTopLeft.X && _mousePos.X <= clickBottomRight.X &&
						_mousePos.Y >= clickTopLeft.Y && _mousePos.Y <= clickBottomRight.Y)
					{
						Vector2 barSize = clickBottomRight - clickTopLeft;
						Vector2 middle = Vector2(clickTopLeft.X + (barSize.X / 2), clickTopLeft.Y + (barSize.Y / 2));
						// If drag left
						if (_mousePos.Distance(clickTopLeft) <= 20.f)
						{
							clickIndex = i;
							left = true;
							mid = false;
							right = false;
							break;
						}
						// If drag center
						else if (_mousePos.Distance(middle) <= 30.f)
						{
							clickIndex = i;
							left = false;
							mid = true;
							right = false;
							break;
						}
						// If drag right
						else if (_mousePos.Distance(clickBottomRight) <= 20.f)
						{
							clickIndex = i;
							left = false;
							mid = false;
							right = true;
							break;
						}
					}
				}
			if (click._keys == OsuKeys::K2)
			{
				if (i > 0)
				{
					int intermediary = 1;
					while ((i - intermediary > 0) && clicks[i - intermediary]._keys != click._keys)
						intermediary++;
					previousClick = clicks[i - intermediary];
				}
				if (i < clicks.size() - 1)
				{
					int intermediary = 1;
					while ((i + intermediary < clicks.size() - 1) && clicks[i + intermediary]._keys != click._keys)
						intermediary++;
					nextClick = clicks[i + intermediary];
				}

				//ImVec2((clientBounds->Width / 2) + var, clientBounds->Height - (clickTimelineHeight / 2)),
				//	ImVec2((clientBounds->Width / 2) + ((static_cast<float>(click.duration) / 2) / Config::ReplayEditor::TimelineResolution) + var, clientBounds->Height)

				Vector2 clickTopLeft = Vector2((clientBounds.X / 2) + var, clientBounds.Y - (clickTimelineHeight / 2));
				Vector2 clickBottomRight = Vector2((clientBounds.X / 2) + ((static_cast<float>(click._duration) / 2) /
					Config::ReplayEditor::TimelineResolution) + var, clientBounds.Y);
				if (_mousePos.X >= clickTopLeft.X && _mousePos.X <= clickBottomRight.X &&
					_mousePos.Y >= clickTopLeft.Y && _mousePos.Y <= clickBottomRight.Y)
				{
					Vector2 barSize = clickBottomRight - clickTopLeft;
					Vector2 middle = Vector2(clickTopLeft.X + (barSize.X / 2), clickTopLeft.Y + (barSize.Y / 2));
					// If drag left
					if (_mousePos.Distance(clickTopLeft) <= 20.f)
					{
						clickIndex = i;
						left = true;
						mid = false;
						right = false;
						break;
					}
					// If drag center
					else if (_mousePos.Distance(middle) <= 30.f)
					{
						clickIndex = i;
						left = false;
						mid = true;
						right = false;
						break;
					}
					// If drag right
					else if (_mousePos.Distance(clickBottomRight) <= 20.f)
					{
						clickIndex = i;
						left = false;
						mid = false;
						right = true;
						break;
					}
				}
			}
		}

	if (left)
	{
		auto& click = clicks[clickIndex];
		secondCall = true;
		bool p = false;
		if (previousClick._startTime != 0 && previousClick._duration != 0)
			p = true;

		int end = click._startTime + click._duration;
		if ((p && XToTime(_mousePos.X) > previousClick._startTime + previousClick._duration) &&
			XToTime(_mousePos.X) < end)
		{
			click._startTime = XToTime(_mousePos.X);
			click._duration = end - click._startTime;
		}
		click._edited = true;
	}
	else if (mid)
	{
		auto& click = clicks[clickIndex];
		secondCall = true;
		bool p = false;
		bool n = false;
		if (previousClick._startTime != 0 && previousClick._duration != 0)
			p = true;
		if (nextClick._startTime != 0 && nextClick._duration != 0)
			n = true;

		if ((p && XToTime(_mousePos.X) - (click._duration / 2) > previousClick._startTime + previousClick._duration) &&
			(n && XToTime(_mousePos.X) + (click._duration / 2) < nextClick._startTime))
		{
			click._startTime = XToTime(_mousePos.X) - (click._duration / 2);
		}
		click._edited = true;
	}
	else if (right)
	{
		auto& click = clicks[clickIndex];
		secondCall = true;
		bool n = false;
		if (nextClick._startTime != 0 && nextClick._duration != 0)
			n = true;

		if (XToTime(_mousePos.X) > click._startTime &&
			(n && XToTime(_mousePos.X) < nextClick._startTime))
		{
			click._duration = XToTime(_mousePos.X) - click._startTime;
		}
		click._edited = true;
	}

	if (_released) {
		secondCall = false;
		left = false;
		mid = false;
		right = false;
	}
}

void ReplayEditor::ClickTimeline::Draw()
{
	float clickTimelineHeight = PERC(clientBounds.Y, 4.f) * StyleProvider::Scale;
	drawList->AddRectFilled(ImVec2(0, clientBounds.Y - clickTimelineHeight), ImVec2(clientBounds.X, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(71.f), COL(71.f), COL(71.f), 1.f)));

	ClickTimelineLocation = std::make_pair(Vector2(0, clientBounds.Y - clickTimelineHeight), Vector2(clientBounds.X, clientBounds.Y));

	// The click timeline should have twice resolution! i.e: it's 1920 wide, so the time resolution should be 1920*2 ms, or 1920 in either direction.
	if (hitObjects == nullptr) return;

	for (auto& click : clicks) {
		auto var = TimeToX(click._startTime);
		if (var * Config::ReplayEditor::TimelineResolution >= -(clientBounds.X * 2) && var < clientBounds.X)
			if (click._keys == OsuKeys::K1)
				drawList->AddRectFilled(ImVec2((clientBounds.X / 2) /* - (click.duration / 2)*/ + var, clientBounds.Y - clickTimelineHeight),
					ImVec2((clientBounds.X / 2) + ((static_cast<float>(click._duration) / 2) / Config::ReplayEditor::TimelineResolution) + var, clientBounds.Y - (clickTimelineHeight / 2)), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), 1.0f)), 8.f);
			else if (click._keys == OsuKeys::K2)
				drawList->AddRectFilled(ImVec2((clientBounds.X / 2) /* - (click.duration / 2)*/ + var, clientBounds.Y - (clickTimelineHeight / 2)),
					ImVec2((clientBounds.X / 2) + ((static_cast<float>(click._duration) / 2) / Config::ReplayEditor::TimelineResolution) + var, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(223.f), COL(148.f), COL(86.f), 1.0f)), 8.f);
	}

	for (auto& ho : *hitObjects) {
		auto var = TimeToX(ho.StartTime);
		if (var * Config::ReplayEditor::TimelineResolution >= -(clientBounds.X * 2) && var < clientBounds.X)
			drawList->AddRectFilled(ImVec2((clientBounds.X / 2) + var, clientBounds.Y - clickTimelineHeight),
				ImVec2((clientBounds.X / 2) + 2 + var, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(150.f), COL(150.f), COL(150.f), 1.0f)), 4.f);
	}

	// White tracer in the middle to indicate relative current position
	drawList->AddRectFilled(ImVec2((clientBounds.X / 2) - 2, clientBounds.Y - clickTimelineHeight),
		ImVec2((clientBounds.X / 2) + 2, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 1.f)), 8.f);
}