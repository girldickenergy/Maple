#include "../Editor.h"

void ReplayEditor::Editor::HandleSelectionDrag()
{
	selectedFrames.clear();
	int allFrames = Editor::selectedReplay.ReplayFrames.size() - 1;

	if (allFrames < 0)
		return;

	int countOne = Editor::currentFrame > Config::ReplayEditor::FrameCount ? Config::ReplayEditor::FrameCount : Editor::currentFrame;
	int countTwo = allFrames - Editor::currentFrame > Config::ReplayEditor::FrameCount ? Config::ReplayEditor::FrameCount : allFrames - Editor::currentFrame;

	std::vector<ReplayFrame> previousFrames = { Editor::selectedReplay.ReplayFrames.begin() + Editor::currentFrame - countOne, Editor::selectedReplay.ReplayFrames.begin() + Editor::currentFrame + countTwo };

	for (auto& frame : previousFrames)
	{
		Vector2 pos = EditorGlobals::ConvertToPlayArea(Vector2(frame.X, frame.Y));
		if (pos.X >= dragArea.first.X && pos.X <= dragArea.second.X &&
			pos.Y >= dragArea.first.Y && pos.Y <= dragArea.second.Y)
			selectedFrames.push_back(frame);
	}
}

void ReplayEditor::Editor::HandleInputs(int nCode, WPARAM wParam, LPARAM lParam)
{
	static UINT lastMessage;
	static bool isLeftMouseDown;
	static bool isShiftDown;
	static bool isCtrlDown;
	static bool clickWithinClickTimeline;
	static Vector2 firstPoint;
	static Vector2 currentPoint;
	MSG* pMsg = reinterpret_cast<MSG*>(lParam);
	if (pMsg->message == WM_RBUTTONUP && lastMessage != WM_RBUTTONUP)
	{
		// Move all frames to cursor pos + offset from real main frame
		if (IsEditing)
		{
			const auto mousePosition = EditorGlobals::DisplayToField(Vector2(mouseX, mouseY));
			const auto currentFramePosition = Vector2(selectedReplay.ReplayFrames[currentFrame].X, selectedReplay.ReplayFrames[currentFrame].Y);
			const auto displacement = Vector2(mousePosition.X - currentFramePosition.X, mousePosition.Y - currentFramePosition.Y);

			auto begin = selectedReplay.ReplayFrames.begin() + currentFrame;
		 
			for (auto it = begin - (std::min)(50, currentFrame);
				it != begin + (std::min)(50, static_cast<int>(selectedReplay.ReplayFrames.size() - currentFrame)); ++it)
			{
				auto& frame = *it;

				// note: 150 time in ms
				const auto dist = std::abs(selectedReplay.ReplayFrames[currentFrame].Time - frame.Time);
				const auto editAmount = dist <= Config::ReplayEditor::EditResyncTime ? (1.f - 
					(dist / static_cast<float>(Config::ReplayEditor::EditResyncTime))) : 0.f;

				frame.X += (editAmount * displacement.X);

				frame.Y += (editAmount * displacement.Y);
			}

#pragma region "Old Logic"
			/*auto begin = selectedReplay.ReplayFrames.begin() + currentFrame;
			int prior = (std::min)(20, currentFrame);
			int future = (std::min)(20, static_cast<int>(selectedReplay.ReplayFrames.size() - currentFrame));

			std::vector<ReplayFrame> allFrames = std::vector<ReplayFrame>{ begin - prior, begin + future };
			for (int i = 0; i < allFrames.size(); i++)
			{
				auto& frame = allFrames[i];
				float editAmount = (i % (allFrames.size() / 2)) / static_cast<float>((allFrames.size() / 2));
				if (i == allFrames.size() / 2) {
					editAmount = 1.0f;
				}
				else if (i > allFrames.size() / 2)
					editAmount = std::lerp(1.0f, 0.0f, editAmount);

				frame.X -= selectedReplay.ReplayFrames[currentFrame].X * editAmount;
				frame.Y -= selectedReplay.ReplayFrames[currentFrame].Y * editAmount;

				auto newPos = EditorGlobals::DisplayToField(Vector2(mouseX, mouseY));
				frame.X += (newPos.X * editAmount);
				frame.Y += (newPos.Y * editAmount);
			}

			for (auto& frame : selectedReplay.ReplayFrames)
				for (auto& editedFrame : allFrames)
					if (frame.Time == editedFrame.Time) {
						frame.X = editedFrame.X;
						frame.Y = editedFrame.Y;
					}*/
#pragma endregion
		}
	}
	if (pMsg->message == WM_LBUTTONDOWN && lastMessage != WM_LBUTTONDOWN) {
		isLeftMouseDown = true;
		firstPoint = Vector2(-999, -999);

		auto xPos = (int)((short)LOWORD(pMsg->lParam));
		auto yPos = (int)((short)HIWORD(pMsg->lParam));

		// Check if click position is within EventTimeline coordinates.
		if (xPos >= eventTimeline.EventTimelineLocation.first.X && xPos <= eventTimeline.EventTimelineLocation.second.X &&
			yPos >= eventTimeline.EventTimelineLocation.first.Y && yPos <= eventTimeline.EventTimelineLocation.second.Y)
		{
			Time = eventTimeline.XToTime(xPos);
			if (Editor::EditorState != EditorState::Playing)
				Editor::ForceUpdateCursorPosition();
			AudioEngine::SeekTo(Time, false, false);
		}

		// Check if click position is within ClickTimeline coordinates.
		if (xPos >= clickTimeline.ClickTimelineLocation.first.X && xPos <= clickTimeline.ClickTimelineLocation.second.X &&
			yPos >= clickTimeline.ClickTimelineLocation.first.Y && yPos <= clickTimeline.ClickTimelineLocation.second.Y)
		{
			clickTimeline.HandleMouse(Vector2(mouseX, mouseY), false);
			clickWithinClickTimeline = true;
			/*Time = eventTimeline.XToTime(xPos);
			if (Editor::EditorState != EditorState::Playing)
				Editor::ForceUpdateCursorPosition();*/
		}

		if (!Dragging && selectedFrames.size() > 0) {
			selectedFrames.clear();
			dragArea = std::make_pair(Vector2(0, 0), Vector2(0, 0));
		}
	}
	if (pMsg->message == WM_LBUTTONUP && lastMessage != WM_LBUTTONUP) {
		firstPoint = Vector2(-999, -999);
		Dragging = false;
		isLeftMouseDown = false;

		// TODO: make this work at some point
		//if (clickWithinClickTimeline)
		//{
		//	clickTimeline.HandleMouse(Vector2(mouseX, mouseY), true);
		//	clickWithinClickTimeline = false;

		//	// apply edited clicks to replay frames
		//	for (auto& click : clickTimeline.clicks)
		//	{
		//		OsuKeys realKeys = click.keys;
		//		bool found = false;
		//		ReplayFrame closestFrame = ReplayFrame();
		//		int closestIndex = 0;
		//		closestFrame.Time = 999999999;
		//		for (int i = 0; i < selectedReplay.ReplayFrames.size() - 1; i++)
		//		{
		//			auto& frame = selectedReplay.ReplayFrames[i];

		//			if (std::abs(click.startTime - frame.Time) < std::abs(click.startTime - closestFrame.Time)) {
		//				closestFrame = frame;
		//				closestIndex = i;
		//			}
		//			
		//			if (realKeys == OsuKeys::K1)
		//				realKeys = (OsuKeys::M1 & OsuKeys::K1);
		//			if (realKeys == OsuKeys::K2)
		//				realKeys = (OsuKeys::M2 & OsuKeys::K2);

		//			if (frame.Time == click.startTime) {
		//				found = true;
		//				if (click.edited)
		//					frame.OsuKeys = realKeys;
		//				break;
		//			}
		//		}
		//		if (!found && closestFrame.Time != -1)
		//		{
		//			bool before = false;
		//			if (closestFrame.Time - click.startTime > 0)
		//				before = true;
		//			int idx = (before ? closestIndex - 1 : closestIndex + 1);

		//			ReplayFrame prevFrame;
		//			ReplayFrame futFrame;
		//			if (idx > 1)
		//				prevFrame = selectedReplay.ReplayFrames[idx - 1];
		//			if (idx < selectedReplay.ReplayFrames.size() - 1)
		//				futFrame = selectedReplay.ReplayFrames[idx + 1];

		//			int difference = futFrame.Time - prevFrame.Time;

		//			float frameCount = difference / 16.67f; // Get required frames for 60 FPS between both replay frames!
		//			if (frameCount > 0.000000f)
		//			{
		//				float roundedFrameCount = std::ceilf(frameCount); // Rounded up, just to make sure :)
		//				for (int j = 0; j < roundedFrameCount; j++)
		//				{
		//					if (before)
		//					{
		//						int lerpedX = std::lerp(prevFrame.X, closestFrame.X, PERC(roundedFrameCount, j) * 100);
		//						int lerpedY = std::lerp(prevFrame.Y, closestFrame.Y, PERC(roundedFrameCount, j) * 100);
		//						int time = closestFrame.Time - (difference / j);
		//						ReplayFrame frame;
		//						frame.X = lerpedX;
		//						frame.Y = lerpedY;
		//						frame.Time = time;
		//						frame.TimeDiff = std::abs(frame.Time - prevFrame.Time);
		//						frame.OsuKeys = realKeys;

		//						selectedReplay.ReplayFrames.insert(selectedReplay.ReplayFrames.begin() +
		//							idx - (roundedFrameCount - j), frame);
		//					}
		//					else
		//					{
		//						int lerpedX = std::lerp(closestFrame.X, futFrame.X, PERC(roundedFrameCount, j) * 100);
		//						int lerpedY = std::lerp(closestFrame.Y, futFrame.Y, PERC(roundedFrameCount, j) * 100);
		//						int time = closestFrame.Time + (difference / (roundedFrameCount - j));
		//						ReplayFrame frame;
		//						frame.X = lerpedX;
		//						frame.Y = lerpedY;
		//						frame.Time = time;
		//						frame.TimeDiff = std::abs(futFrame.Time - frame.Time);
		//						frame.OsuKeys = realKeys;

		//						selectedReplay.ReplayFrames.insert(selectedReplay.ReplayFrames.begin() +
		//							idx + j, frame);
		//					}
		//				}
		//			}
		//		}
		//	}

		//	// why the fuck do i have to reinstantiate it when re-parsing? when i don't do this, it's all misses. fix!!!
		//	sRectangle* clientBounds = eventTimeline.clientBounds;
		//	ImDrawList* drawList = eventTimeline.drawList;
		//	Editor::eventTimeline = EventTimeline(&Time, drawList, &Editor::selectedReplay,
		//		clientBounds, &hitObjects, Editor::bmap.GetOverallDifficulty(), Editor::bmap.GetCircleSize(), customHomInstance);

		//	Editor::eventTimeline.ParseEvents();

		//	Editor::clickTimeline = ClickTimeline(&Time, drawList, &Editor::selectedReplay, clientBounds, &hitObjects);
		//	Editor::clickTimeline.ParseClicks();

		//	Editor::ForceUpdateCursorPosition();
		//}
		//else
		Editor::HandleSelectionDrag();
	}
	if (pMsg->message == WM_MOUSEMOVE) {
		mouseX = (int)((short)LOWORD(pMsg->lParam));
		mouseY = (int)((short)HIWORD(pMsg->lParam));
		if (isLeftMouseDown && Editor::EditorState != EditorState::Playing)
		{
			Vector2 clientBounds = GameBase::GetClientSize();
			Dragging = true;
			auto xPos = (int)((short)LOWORD(pMsg->lParam));
			auto yPos = (int)((short)HIWORD(pMsg->lParam));
			if (firstPoint.X == -999 && firstPoint.Y == -999)
				firstPoint = Vector2(xPos, yPos);
			currentPoint = Vector2(xPos, yPos);

			Vector2 realFirstPoint = Vector2((std::min)(firstPoint.X, currentPoint.X), (std::min)(firstPoint.Y, currentPoint.Y));
			Vector2 realSecondPoint = Vector2((std::max)(firstPoint.X, currentPoint.X), (std::max)(firstPoint.Y, currentPoint.Y));

			float topBarHeight = ((clientBounds.Y * 3.75f) / 100.f) * StyleProvider::Scale;
			std::pair<Vector2, Vector2> et = eventTimeline.EventTimelineLocation;

			if (realFirstPoint.Y > topBarHeight && realSecondPoint.Y > topBarHeight &&
				realFirstPoint.Y < et.first.Y && realSecondPoint.Y < et.first.Y) {
				if (realFirstPoint.X < EditorGlobals::PlayfieldOffset.X)
					realFirstPoint.X = EditorGlobals::PlayfieldOffset.X;
				if (realFirstPoint.Y < EditorGlobals::PlayfieldOffset.Y)
					realFirstPoint.Y = EditorGlobals::PlayfieldOffset.Y;
				if (realSecondPoint.X > EditorGlobals::PlayfieldOffset.X + EditorGlobals::PlayfieldSize.X)
					realSecondPoint.X = EditorGlobals::PlayfieldOffset.X + EditorGlobals::PlayfieldSize.X;
				if (realSecondPoint.Y > EditorGlobals::PlayfieldOffset.Y + EditorGlobals::PlayfieldSize.Y)
					realSecondPoint.Y = EditorGlobals::PlayfieldOffset.Y + EditorGlobals::PlayfieldSize.Y;

				dragArea = std::make_pair(realFirstPoint, realSecondPoint);
			}
			else
				dragArea = std::make_pair(Vector2(0, 0), Vector2(0, 0));
		}
		if (isLeftMouseDown && clickWithinClickTimeline)
		{
			clickTimeline.HandleMouse(Vector2(mouseX, mouseY), false);
		}
	}
	if (pMsg->message == WM_MOUSEWHEEL && lastMessage != WM_MOUSEWHEEL)
	{
		MSLLHOOKSTRUCT* mouselparam = (MSLLHOOKSTRUCT*)lParam;
		short zDelta = HIWORD(mouselparam->mouseData);
		if (isShiftDown)
		{
			HitObject ho = hitObjects[0];
			if (zDelta > 0)
			{
				for (auto& object : hitObjects)
					if (object.StartTime > Time) {
						ho = object;
						break;
					}
			}
			else if (zDelta < 0)
			{
				std::vector<HitObject> hos = std::vector<HitObject>();
				for (auto& object : hitObjects)
					if (object.StartTime < Time)
						hos.push_back(object);
				ho = hos.back();
			}

			Time = ho.StartTime;
			AudioEngine::SeekTo(Time, false, false);
			if (Editor::EditorState != EditorState::Playing)
				Editor::ForceUpdateCursorPosition();
		}
		else if (isCtrlDown) {
			if (zDelta > 0)
				Time = clickTimeline.GetClick(GetClickState::Forward);
			else
				Time = clickTimeline.GetClick(GetClickState::Backward);

			AudioEngine::SeekTo(Time, false, false);
			if (Editor::EditorState != EditorState::Playing)
				Editor::ForceUpdateCursorPosition();
		}
		else {
			if (zDelta > 0)
				if (currentFrame < selectedReplay.ReplayFrames.size() - 1)
					Time = selectedReplay.ReplayFrames[currentFrame + 1].Time;
			if (zDelta < 0)
				if (currentFrame > 0)
					Time = selectedReplay.ReplayFrames[currentFrame - 1].Time;
			AudioEngine::SeekTo(Time, false, false);
			if (Editor::EditorState != EditorState::Playing)
				Editor::ForceUpdateCursorPosition();
		}
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SHIFT && lastMessage != WM_KEYDOWN)
		isShiftDown = true;

	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_SHIFT && lastMessage != WM_KEYUP)
		isShiftDown = false;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL && lastMessage != WM_KEYDOWN)
		isCtrlDown = true;

	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_CONTROL && lastMessage != WM_KEYUP)
		isCtrlDown = false;

	if (pMsg->message == WM_KEYUP && pMsg->wParam == 'E' && lastMessage != WM_KEYUP) {
		if (IsEditing)
		{
			// why the fuck do i have to reinstantiate it when re-parsing? when i don't do this, it's all misses. fix!!!
			Vector2* clientBounds = eventTimeline.clientBounds;
			ImDrawList* drawList = eventTimeline.drawList;
			Editor::eventTimeline = EventTimeline(&Time, drawList, &Editor::selectedReplay,
				clientBounds, &hitObjects, Editor::bmap.GetOverallDifficulty(), Editor::bmap.GetCircleSize(), customHomInstance);
				
			Editor::eventTimeline.ParseEvents();

			Editor::ForceUpdateCursorPosition();
		}
		IsEditing = !IsEditing;
	}

	if (pMsg->message == WM_KEYUP && pMsg->wParam == 'X' && lastMessage != WM_KEYUP)
	{
		ReplayFrame& frame = Editor::selectedReplay.ReplayFrames[Editor::currentFrame];
		OsuKeys keys = frame.OsuKeys;

		keys = static_cast<OsuKeys>(static_cast<int>(keys) ^ static_cast<int>(OsuKeys::K1));

		keys = static_cast<OsuKeys>(static_cast<int>(keys) ^ static_cast<int>(OsuKeys::M1));

		frame.OsuKeys = keys;

		// why the fuck do i have to reinstantiate it when re-parsing? when i don't do this, it's all misses. fix!!!
		Vector2* clientBounds = eventTimeline.clientBounds;
		ImDrawList* drawList = eventTimeline.drawList;
		Editor::eventTimeline = EventTimeline(&Time, drawList, &Editor::selectedReplay,
			clientBounds, &hitObjects, Editor::bmap.GetOverallDifficulty(), Editor::bmap.GetCircleSize(), customHomInstance);

		Editor::eventTimeline.ParseEvents();

		Editor::clickTimeline = ClickTimeline(&Time, drawList, &Editor::selectedReplay, clientBounds, &hitObjects);
		Editor::clickTimeline.ParseClicks();

		Editor::ForceUpdateCursorPosition();
	}

	if (pMsg->message == WM_KEYUP && pMsg->wParam == 'C' && lastMessage != WM_KEYUP)
	{
		ReplayFrame& frame = Editor::selectedReplay.ReplayFrames[Editor::currentFrame];
		OsuKeys keys = frame.OsuKeys;

		keys = static_cast<OsuKeys>(static_cast<int>(keys) ^ static_cast<int>(OsuKeys::K2));

		keys = static_cast<OsuKeys>(static_cast<int>(keys) ^ static_cast<int>(OsuKeys::M2));

		frame.OsuKeys = keys;

		// why the fuck do i have to reinstantiate it when re-parsing? when i don't do this, it's all misses. fix!!!
		Vector2* clientBounds = eventTimeline.clientBounds;
		ImDrawList* drawList = eventTimeline.drawList;
		Editor::eventTimeline = EventTimeline(&Time, drawList, &Editor::selectedReplay,
			clientBounds, &hitObjects, Editor::bmap.GetOverallDifficulty(), Editor::bmap.GetCircleSize(), customHomInstance);
			
		Editor::eventTimeline.ParseEvents();

		Editor::clickTimeline = ClickTimeline(&Time, drawList, &Editor::selectedReplay, clientBounds, &hitObjects);
		Editor::clickTimeline.ParseClicks();

		Editor::ForceUpdateCursorPosition();
	}

	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_SPACE && lastMessage != WM_KEYUP) {
		if (!Dragging && selectedFrames.size() > 0) {
			selectedFrames.clear();
			dragArea = std::make_pair(Vector2(0, 0), Vector2(0, 0));
		}

		Editor::EditorState == EditorState::Playing ? Editor::Pause() : Editor::Play();

		AudioEngine::TogglePause();
	}

	lastMessage = pMsg->message;
}