#include "ClickOverlay.h"

ReplayEditor::ClickOverlay::ClickOverlay()
{ 
	_isInit = false;
}

ReplayEditor::ClickOverlay::ClickOverlay(int* timer, std::vector<Click>* clicks, ImDrawList* drawList, Vector2* clientBounds)
{
	_timer = timer;
	_clicks = clicks;
	_drawList = drawList;
	_clientBounds = clientBounds;
	_isInit = true;
}

void ReplayEditor::ClickOverlay::Render()
{
	/// @brief The button size, both x and y
	float buttonSize = PERC(_clientBounds->Y, 4.f) * StyleProvider::Scale;
	float offset = 2.0f;

	ImVec2 buttonOneStart = ImVec2(_clientBounds->X - buttonSize - (offset * 2),
		(_clientBounds->Y / 2) - (buttonSize / 2));
	ImVec2 buttonOneEnd = ImVec2(_clientBounds->X - (offset * 2),
		(_clientBounds->Y / 2) + (buttonSize / 2));
	_drawList->AddRect(buttonOneStart, buttonOneEnd,
		ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 1.f)), 6.f, 0, 2.f);

	ImVec2 buttonTwoStart = ImVec2(_clientBounds->X - buttonSize - (offset * 2),
		(_clientBounds->Y / 2) + (buttonSize / 2) + offset);
	ImVec2 buttonTwoEnd = ImVec2(_clientBounds->X - (offset * 2),
		(_clientBounds->Y / 2) + (buttonSize * 1.5f) + offset);
	_drawList->AddRect(buttonTwoStart, buttonTwoEnd,
		ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 1.f)), 6.f, 0, 2.f);

	for (auto& click : *_clicks)
		if (click._startTime <= *_timer && click._startTime + click._duration >= *_timer) {
			_currentOsuKeys = click._keys;
			if ((click._keys & OsuKeys::K1) > OsuKeys::None)
				_drawList->AddRectFilled(buttonOneStart, buttonOneEnd,
					ImGui::ColorConvertFloat4ToU32(ImVec4(COL(232.f), COL(93.f), COL(155.f), 1.0f)), 6.f, 0);
			else if ((click._keys & OsuKeys::K2) > OsuKeys::None)
				_drawList->AddRectFilled(buttonTwoStart, buttonTwoEnd,
					ImGui::ColorConvertFloat4ToU32(ImVec4(COL(223.f), COL(148.f), COL(86.f), 1.0f)), 6.f, 0);
		}
}