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

	// todo: key never released bad
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
	if (!currentlyDragging) {
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

		// If no click has been clicked, but the user still clicked within the clicktimeline
		if (!currentlyDragging)
		{
			auto addingTop = mousePosition.Y >= clientBounds.Y - clickTimelineHeight && mousePosition.Y < clientBounds.Y - (clickTimelineHeight / 2);
			auto addingBottom = mousePosition.Y >= clientBounds.Y - (clickTimelineHeight / 2) && mousePosition.Y < clientBounds.Y;

			if (!addingTop && !addingBottom)
				return;

			auto addingStartTime = XToTime(mousePosition.X);

			auto keys = addingTop ? OsuKeys::K1 : OsuKeys::K2;
			auto newClick = Click(addingStartTime, Vector2(), 50, keys);
			clicks.push_back(newClick);

			dragMode = DragMode::Right;
			draggingIndex = clicks.size() - 1;
			currentlyDragging = true;
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
		currentlyDragging = false;

		// Update clicks
		//switch (dragMode)
		//{
		//case DragMode::Left:
		//{
		//	auto& click = clicks[draggingIndex];
		//	int comparrasionIndex = draggingIndex;
		//	while (--comparrasionIndex > 0)
		//	{
		//		auto& previousClick = clicks[comparrasionIndex];
		//		if (previousClick._keys != click._keys)
		//			continue;

		//		if (previousClick.StartTime >= click.StartTime) {
		//			clicks.erase(clicks.begin() + comparrasionIndex);
		//			draggingIndex--;
		//		}
		//		else if (previousClick.StartTime + previousClick.Duration >= click.StartTime)
		//		{
		//			auto newDuration = (click.StartTime + click.Duration) - previousClick.StartTime;
		//			previousClick.Duration = newDuration;
		//			clicks.erase(clicks.begin() + draggingIndex);
		//			draggingIndex = comparrasionIndex;
		//		}
		//		else
		//			break;
		//	}

		//	break;
		//}
		//case DragMode::Middle: {
		//	auto& click = clicks[draggingIndex];
		//	// Determine if the click has been dragged to the right or to the left
		//	bool draggedLeft = (click.StartTime < click.OriginalStartTime) ? true : false;
		//	int comparrasionIndex = draggingIndex;

		//	while ((draggedLeft) ? --comparrasionIndex > 0 : ++comparrasionIndex < clicks.size())
		//	{
		//		auto& toCheckClick = clicks[comparrasionIndex];
		//		if (toCheckClick._keys != click._keys)
		//			continue;
		//		auto clickEndTime = click.StartTime + click.Duration;
		//		auto toCheckClickEndTime = toCheckClick.StartTime + toCheckClick.Duration;

		//		if (draggedLeft)
		//		{
		//			if (click.StartTime < toCheckClickEndTime)
		//			{
		//				auto maxEndtime = (std::max)(clickEndTime, toCheckClickEndTime);
		//				toCheckClick.Duration = maxEndtime - toCheckClick.StartTime;

		//				clicks.erase(clicks.begin() + draggingIndex);
		//				draggingIndex = comparrasionIndex;
		//			}
		//		}
		//	}
		//	break;
		//}
		//					 /*case DragMode::Right: {
		//	click.Duration = time - click.StartTime;
		//	break;
		//}*/
		//}

		auto& click = clicks[draggingIndex];

		auto& replayHandler = Editor::Get().GetReplayHandler();
		auto& replayFrames = replayHandler.GetReplay()->ReplayFrames;

		// interpolate if needed

		/*if (click.Duration > click.OriginalDuration)
		{
			auto replayFramesAffected = replayHandler.GetReplayFramesWithinTimeFrame(click.StartTime, click.StartTime + click.Duration);
			for (const auto& frame : replayFramesAffected)
			{
				auto index = replayHandler.GetIndexOfFrame(frame);
				replayFrames[index].OsuKeys = replayFrames[index].OsuKeys | click._keys;
			}
		}
		else if (click.Duration < click.OriginalDuration)
		{
			auto replayFramesAffected = replayHandler.GetReplayFramesWithinTimeFrame(click.OriginalStartTime, click.StartTime);
			for (const auto& frame : replayFramesAffected)
			{
				auto index = replayHandler.GetIndexOfFrame(frame);
				replayFrames[index].OsuKeys = replayFrames[index].OsuKeys & ~(click._keys | OsuKeys::M1 | OsuKeys::M2);
			}
		}*/

		if (rightClick)
			clicks.erase(clicks.begin() + draggingIndex);

		bool frameStartExists = replayHandler.DoesFrameExistOnTime(click.StartTime);
		bool frameEndExists = replayHandler.DoesFrameExistOnTime(click.StartTime + click.Duration);

		if (!frameStartExists)
		{
			auto closestTwoFrames = replayHandler.GetTwoClosestReplayFrames(click.StartTime);
			auto interpolatedFrame = replayHandler.InterpolateReplayFrames(std::get<0>(closestTwoFrames), std::get<1>(closestTwoFrames), click.StartTime);

			replayFrames.insert(replayFrames.begin() + replayHandler.GetIndexOfFrame(std::get<1>(closestTwoFrames)), interpolatedFrame);
		}

		if (!frameEndExists)
		{
			auto closestTwoFrames = replayHandler.GetTwoClosestReplayFrames(click.StartTime + click.Duration);
			auto interpolatedFrame = replayHandler.InterpolateReplayFrames(std::get<0>(closestTwoFrames), std::get<1>(closestTwoFrames), click.StartTime + click.Duration);

			replayFrames.insert(replayFrames.begin() + replayHandler.GetIndexOfFrame(std::get<1>(closestTwoFrames)), interpolatedFrame);
		}

		for (auto& frame : replayFrames)
		{
			frame.OsuKeys = OsuKeys::None;
			for (auto& click : clicks)
			{
				auto clickEndTime = click.StartTime + click.Duration;
				if (click.StartTime <= frame.Time && clickEndTime >= frame.Time)
					frame.OsuKeys = frame.OsuKeys | click._keys;
			}
		}

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