#include "OsuCursor.h"
#include "../../../../../Config/Config.h"
#include "../../../EditorGlobals.h"

void ReplayEditor::OsuCursor::RenderTrail()
{
	int countPrevious = *_currentFrame > Config::ReplayEditor::FrameCount ? Config::ReplayEditor::FrameCount : *_currentFrame;
	int countFuture = _currentReplay->ReplayFrames.size() - 1 - *_currentFrame > Config::ReplayEditor::FrameCount ? Config::ReplayEditor::FrameCount : _currentReplay->ReplayFrames.size() - 1 - *_currentFrame;

	for (auto it = _currentReplay->ReplayFrames.begin() + *_currentFrame - countPrevious; it != _currentReplay->ReplayFrames.begin() + *_currentFrame; ++it)
	{
		auto& frame = *it;
		auto& frame2 = *(it + 1);
		Vector2 screenPos = EditorGlobals::ConvertToPlayArea(Vector2(frame.X, frame.Y));
		Vector2 screenPos2 = EditorGlobals::ConvertToPlayArea(Vector2(frame2.X, frame2.Y));

		float opacity = std::lerp(0.0f, 0.65f, 1.f - static_cast<float>(std::distance(it, _currentReplay->ReplayFrames.begin() + *_currentFrame)) / countPrevious);
		_drawList->AddLine(ImVec2(screenPos.X, screenPos.Y), ImVec2(screenPos2.X, screenPos2.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), opacity)), 2.f);
		if (Config::ReplayEditor::ShowReplayFrames)
			_drawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 4.f, ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), opacity)));
	}

	for (auto it = _currentReplay->ReplayFrames.begin() + *_currentFrame; it != _currentReplay->ReplayFrames.begin() + *_currentFrame + countFuture; ++it)
	{
		auto& frame = *it;
		auto& frame2 = *(it + 1);
		Vector2 screenPos = EditorGlobals::ConvertToPlayArea(Vector2(frame.X, frame.Y));
		Vector2 screenPos2 = EditorGlobals::ConvertToPlayArea(Vector2(frame2.X, frame2.Y));

		float opacity = std::lerp(0.0f, 0.65f, 1.f - static_cast<float>(std::distance(_currentReplay->ReplayFrames.begin() + *_currentFrame, it)) / countFuture);
		_drawList->AddLine(ImVec2(screenPos.X, screenPos.Y), ImVec2(screenPos2.X, screenPos2.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), opacity)));
		if (Config::ReplayEditor::ShowReplayFrames)
			_drawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 2.f, ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), opacity)));
	}
}

void ReplayEditor::OsuCursor::RenderCursor()
{
	auto& currentFrame = _currentReplay->ReplayFrames[*_currentFrame];
	Vector2 cursorScreenPosition = EditorGlobals::ConvertToPlayArea(Vector2(currentFrame.X, currentFrame.Y));
	ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	/*if ((*_currentPressedKeys & OsuKeys::K1) > OsuKeys::None)
		col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), 1.0f));
	else if ((*_currentPressedKeys & OsuKeys::K2) > OsuKeys::None)
		col = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(223.f), COL(148.f), COL(86.f), 1.0f));
	else if (*_currentPressedKeys == OsuKeys::None)
		col = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));*/

	_drawList->AddCircleFilled(ImVec2(cursorScreenPosition.X, cursorScreenPosition.Y), 4.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
	_drawList->AddCircle(ImVec2(cursorScreenPosition.X, cursorScreenPosition.Y), 12.f, col, 0, 1.5f);
}

ReplayEditor::OsuCursor::OsuCursor()
{
	_currentFrame = nullptr;
	_currentReplay = nullptr;
	_drawList = nullptr;
	_currentPressedKeys = nullptr;
}

ReplayEditor::OsuCursor::OsuCursor(int* currentFrame, Replay* currentReplay, ImDrawList* drawList, OsuKeys* currentPressedKeys)
{
	_currentFrame = currentFrame;
	_currentReplay = currentReplay;
	_drawList = drawList;
	_currentPressedKeys = currentPressedKeys;
}

void ReplayEditor::OsuCursor::Render()
{
	if (_currentFrame == nullptr || _currentReplay == nullptr || _drawList == nullptr || *_currentFrame <= 0)
		return;
	RenderTrail();
	RenderCursor();
}