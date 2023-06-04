#include "ClickTimeline.h"

#include "../../Editor.h"

ReplayEditor::ClickTimeline::ClickTimeline()
{ }

ReplayEditor::ClickTimeline::ClickTimeline(int* _timer, ImDrawList* _drawList, Replay* _replay, Vector2 _clientBounds, std::vector<HitObject>* _hitObjects)
{
	timer = _timer;
	drawList = _drawList;
	replay = _replay;
	clientBounds = _clientBounds;
	hitObjects = _hitObjects;

	currentlyDragging = false;
	draggingIndex = 0;
	dragMode = DragMode::Middle;
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
			if (click.StartTime > *timer) {
				c = click;
				break;
			}
	}
	else if (state == GetClickState::Backward)
	{
		std::vector<Click> cl = std::vector<Click>();
		for (auto& clck : clicks)
			if (clck.StartTime < *timer)
				cl.push_back(clck);
		c = cl.back();
	}
	return c.StartTime;
}

void ReplayEditor::ClickTimeline::SetHitObjects(std::vector<HitObject>* _hitObjects)
{
	hitObjects = _hitObjects;
}

void ReplayEditor::ClickTimeline::ParseClicks()
{
	clicks.clear();

	// TODO: this is very ugly code and bad practice, but it makes both keys at the same time work. make this prettier
	for (int i = 0; i < replay->ReplayFrames.size(); i++)
	{
		auto& frame = replay->ReplayFrames[i];
		if ((frame.OsuKeys & OsuKeys::K1) > OsuKeys::None)
		{
			Click c = Click(frame.Time, Vector2(frame.X, frame.Y), 0, OsuKeys::K1);
			while ((replay->ReplayFrames[++i].OsuKeys & OsuKeys::K1) > OsuKeys::None)
				c.Duration = replay->ReplayFrames[i].Time - c.StartTime;
			if (c.Duration == 0)
				c.Duration = replay->ReplayFrames[i].Time - c.StartTime;

			c.OriginalDuration = c.Duration;
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
				c.Duration = replay->ReplayFrames[i].Time - c.StartTime;
			if (c.Duration == 0)
				c.Duration = replay->ReplayFrames[i].Time - c.StartTime;

			c.OriginalDuration = c.Duration;
			clicks.push_back(c);
		}
	}
	//// remove all clicks with a duration of 0
	//std::vector<int> toRemove = std::vector<int>();
	//for (int i = 0; i < clicks.size(); i++)
	//	if (clicks[i].Duration == 0)
	//		toRemove.push_back(i);
	//for (auto& rem : toRemove)
	//	clicks.erase(clicks.begin() + rem);

	IsInit = true;
}

void ReplayEditor::ClickTimeline::HandleMouse(Vector2 mousePosition, bool released, bool rightClick)
{
	const auto clickTimelineHeight = PERC(clientBounds.Y, 4.f) * StyleProvider::Scale;
	const auto resolution = Config::ReplayEditor::TimelineResolution;
	const auto centerX = clientBounds.X / 2;
	const auto mouseClickImVec2 = ImVec2(mousePosition.X, mousePosition.Y);

	// Find dragged click event
	if (!currentlyDragging)
		for (auto it = clicks.begin(); it != clicks.end(); ++it)
		{
			auto click = *it;
			auto xPosition = TimeToX(click.StartTime);

			// If the click event is not currently being drawn, don't check for it
			if (xPosition * resolution >= -(clientBounds.X * 2) && xPosition < clientBounds.X)
			{
				// StartX, StartY, EndX, EndY
				ImRect clickBoundingBox;
				if (click._keys == OsuKeys::K1)
					clickBoundingBox = ImRect(
						centerX + xPosition,
						clientBounds.Y - clickTimelineHeight,
						centerX + ((click.Duration / 2) / resolution) + xPosition,
						clientBounds.Y - (clickTimelineHeight / 2));
				else if (click._keys == OsuKeys::K2)
					clickBoundingBox = ImRect(
						centerX + xPosition,
						clientBounds.Y - (clickTimelineHeight / 2),
						centerX + ((click.Duration / 2) / resolution) + xPosition,
						clientBounds.Y);

				// If the mouse click is not within this click event
				if (!clickBoundingBox.Contains(mouseClickImVec2))
					continue;

				// Check how this event should be manipulated.
				// The click bar should be split in 33.3 percentiles
				// where if the user clicks on the left 33.3% it will drag from the left
				// if the user dragged on the middle 33.3% it will move and if the user
				// dragged on the right 33.3% it will drag from the right.
				const auto boundingBoxCenterImVec2 = clickBoundingBox.GetCenter();
				const auto boundingBoxCenter = Vector2(boundingBoxCenterImVec2.x, boundingBoxCenterImVec2.y);
				const auto boundingBoxLeftImVec2 = clickBoundingBox.GetTL();
				const auto boundingBoxLeft = Vector2(boundingBoxLeftImVec2.x, boundingBoxLeftImVec2.y);
				const auto boundingBoxRightImVec2 = clickBoundingBox.GetBR();
				const auto boundingBoxRight = Vector2(boundingBoxRightImVec2.x, boundingBoxRightImVec2.y);

				const auto sizeImVec2 = clickBoundingBox.GetSize();
				const auto middleLeftMinX = boundingBoxCenter.X - PERC(sizeImVec2.x, 33.3f);
				const auto middleRightMaxX = boundingBoxCenter.X + PERC(sizeImVec2.x, 33.3f);
				const auto leftBoundingBox = ImRect(boundingBoxLeft.X, boundingBoxLeft.Y, middleLeftMinX, boundingBoxRight.Y);
				const auto middleBoundingBox = ImRect(middleLeftMinX, boundingBoxLeft.Y, middleRightMaxX, boundingBoxRight.Y);
				const auto rightBoundingBox = ImRect(middleRightMaxX, boundingBoxLeft.Y, boundingBoxRight.X, boundingBoxRight.Y);

				// The click has landed on the left bounding box
				if (leftBoundingBox.Contains(mouseClickImVec2))
				{
					dragMode = DragMode::Left;
				}
				// The click has landed on the middle bounding box
				else if (middleBoundingBox.Contains(mouseClickImVec2))
				{
					dragMode = DragMode::Middle;
				}
				// The click has landed on the right bounding box
				else if (rightBoundingBox.Contains(mouseClickImVec2))
				{
					dragMode = DragMode::Right;
				}
				// The click landed nowhere near (how did we get here?)
				else
					continue;

				currentlyDragging = true;
				draggingIndex = std::distance(clicks.begin(), it);
				break;
			}
		}
	else
	{
		auto& click = clicks[draggingIndex];
		const auto time = XToTime(mousePosition.X);
		switch (dragMode)
		{
		case DragMode::Left:
		{
			auto end = click.StartTime + click.Duration;

			click.StartTime = time;
			click.Duration = end - click.StartTime;
			break;
		}
		case DragMode::Middle: {
			click.StartTime = time - (click.Duration / 2);
			break;
		}
		case DragMode::Right: {
			click.Duration = time - click.StartTime;
			break;
		}
		}

		click._edited = true;
	}

	if (rightClick)
		clicks[draggingIndex]._keys = OsuKeys::None;

	if (released && currentlyDragging)
	{
		auto& click = clicks[draggingIndex];
		currentlyDragging = false;
		dragMode = DragMode::Middle;

		auto& replayHandler = Editor::Get().GetReplayHandler();
		auto& replayFrames = replayHandler.GetReplay()->ReplayFrames;

		// Check if the click landed exactly on a replay frame
		auto closestReplayFrame = replayHandler.GetFrameClosestToTime(click.StartTime);
		if (closestReplayFrame.Time == click.StartTime)
		{
			// If the frame has the exact same time as the click we only have to change the keys on the frame
			auto frameIndex = replayHandler.GetIndexOfFrame(closestReplayFrame);
			replayFrames[frameIndex].OsuKeys = click._keys;
		}
		else
		{
			// Grab the two closest replay frames
			auto closestReplayFrames = replayHandler.GetTwoClosestReplayFrames(click.StartTime);
			// Create an interpolated frame from the two closest replay frames
			auto interpolatedFrame = replayHandler.InterpolateReplayFrames(std::get<0>(closestReplayFrames),
				std::get<1>(closestReplayFrames), click.StartTime);
			interpolatedFrame.OsuKeys = click._keys;

			// Grab the index of the first frame
			auto firstFrameIndex = replayHandler.GetIndexOfFrame(std::get<0>(closestReplayFrames));

			// Insert the interpolated frame into the replay
			replayFrames.insert(replayFrames.begin() + firstFrameIndex + 1, interpolatedFrame);
		}

		// Find all replayframes affected by the edit and reset their keypresses
		auto searchStartTime = (click.StartTime > click.OriginalStartTime) ? click.OriginalStartTime : click.StartTime;
		auto searchDuration = (click.Duration > click.OriginalDuration) ? click.Duration : click.OriginalDuration;
		auto affectedFrames = replayHandler.GetReplayFramesWithinTimeFrame(searchStartTime, searchStartTime + searchDuration);

		for (auto const& frame : affectedFrames)
		{
			auto index = replayHandler.GetIndexOfFrame(frame);

			auto foundClick = Click();
			foundClick._keys = OsuKeys::None;
			for (auto it = clicks.begin(); it != clicks.end(); ++it)
			{
				auto& currentClick = *it;
				if (frame.Time >= currentClick.StartTime && frame.Time < currentClick.StartTime + currentClick.Duration)
				{
					if (foundClick._keys == OsuKeys::None)
						foundClick = currentClick;
					else
						foundClick._keys = foundClick._keys | currentClick._keys;
				}
			}
			replayFrames[index].OsuKeys = foundClick._keys;
		}

		// Update the original times, so that the next edit will also work as intended
		click.OriginalStartTime = click.StartTime;
		click.OriginalDuration = click.Duration;

		if (rightClick)
			clicks.erase(clicks.begin() + draggingIndex);

		draggingIndex = 0;

		// Reparse all clicks from the replay frames so there's no discrepencies between the ClickTimeline and the ReplayFrames
		ParseClicks();
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
		auto var = TimeToX(click.StartTime);
		if (var * Config::ReplayEditor::TimelineResolution >= -(clientBounds.X * 2) && var < clientBounds.X)
			if (click._keys == OsuKeys::K1)
				drawList->AddRectFilled(ImVec2((clientBounds.X / 2) + var, clientBounds.Y - clickTimelineHeight),
					ImVec2((clientBounds.X / 2) + ((static_cast<float>(click.Duration) / 2) / Config::ReplayEditor::TimelineResolution) + var,
						clientBounds.Y - (clickTimelineHeight / 2)),
					ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), 1.0f)), 8.f);
			else if (click._keys == OsuKeys::K2)
				drawList->AddRectFilled(ImVec2((clientBounds.X / 2) + var, clientBounds.Y - (clickTimelineHeight / 2)),
					ImVec2((clientBounds.X / 2) + ((static_cast<float>(click.Duration) / 2) / Config::ReplayEditor::TimelineResolution) + var,
						clientBounds.Y),
					ImGui::ColorConvertFloat4ToU32(ImVec4(COL(223.f), COL(148.f), COL(86.f), 1.0f)), 8.f);
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