#include "Editor.h"
#include "../../Config/Config.h"
#pragma warning(disable: 26812) // Disable enum unscoped warning
#pragma warning(disable: 4996)

#include "../../Sdk/Beatmaps/BeatmapManager.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Player/Ruleset.h"
#include "../../UI/StyleProvider.h"
#include "../../UI/Widgets/Widgets.h"

#include "../ReplayBot/ReplayBot.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include <Vanilla.h>

#include "Math/sRectangle.h"

#include "../../Dependencies/Milk/MilkThread.h"

#include "../../Logging/Logger.h"

#include <xorstr.hpp>
using namespace ReplayEditor;

inline bool init = false;

void Editor::LoadBeatmap(std::string beatmapHash)
{
	_hitObjectManagerWrapper.ConstructNewHitObjectManager();
	auto& instance = _hitObjectManagerWrapper.GetHitObjectManagerInstance();

    uintptr_t beatmapPointer = BeatmapManager::Get().GetBeatmapByChecksum(_replayHandler.GetReplayBeatmapHash());

	bmap = Beatmap(beatmapPointer);
	bmap.Update();

	_hitObjectManagerWrapper.SetBeatmapAndLoad(beatmapPointer, _replayHandler.GetReplayMods());
	// UpdateStacking gets called in ApplyStacking, same with UpdateSlidersAll

	/*HitObjectManager::UpdateSlidersAll(instance, false);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::UpdateSlidersAll");*/

	/*HitObjectManager::UpdateStacking(instance, 0, -1);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::UpdateStacking");*/

	Drawables.clear();
	hitObjects.clear();

	HitObjectManager::CacheHitObjects(instance);
	hitObjects = HitObjectManager::HitObjects;
	HitObjectManager::HitObjects.clear();

	for (auto& object : hitObjects)
		if ((_replayHandler.GetReplayMods() & Mods::HardRock) > Mods::None)
			object.Position = Vector2(object.Position.X, 384 - object.Position.Y);
	clickTimeline.SetHitObjects(&hitObjects);

	float cs = bmap.GetCircleSize();
	if ((_replayHandler.GetReplayMods() & Mods::HardRock) > Mods::None)
		cs = std::min(10.f, cs * 1.3f);

	eventTimeline.SetHitObjects(&hitObjects);
	eventTimeline.SetCircleSize(cs);
	eventTimeline.SetOverallDifficulty(bmap.GetOverallDifficulty());

	osuPlayfield = OsuPlayfield(drawList, &Editor::bmap,
		&Time, &Editor::currentFrame, &hitObjects);

	eventTimeline.ParseEvents(osuPlayfield.GetDrawables());
	//BeatmapManager::Get().Load(reinterpret_cast<void*>(bmap.GetBeatmapOsuPointer()));

	BeatmapManager::Get().SetCurrent(bmap.GetBeatmapOsuPointer());

	/*AudioEngine::LoadAudio(bmap.GetBeatmapOsuPointer(), false, false, true, false);
	Logger::Log(LogSeverity::Debug, "Reached after AudioEngine::LoadAudio()");*/
}

void Editor::DrawSelectedFrames(ImDrawList* drawList)
{
	if (selectedFrames.size() > 0)
	{
		for (int i = 0; i < selectedFrames.size(); i++)
		{
			Vector2 screenPos = EditorGlobals::ConvertToPlayArea(Vector2(selectedFrames[i].X, selectedFrames[i].Y));
			Vector2 screenPos2 = Vector2(0, 0);

			if (i < selectedFrames.size() - 1)
				screenPos2 = EditorGlobals::ConvertToPlayArea(Vector2(selectedFrames[i + 1].X, selectedFrames[i + 1].Y));

			if (screenPos2.X != 0 && screenPos2.Y != 0)
				drawList->AddLine(ImVec2(screenPos.X, screenPos.Y), ImVec2(screenPos2.X, screenPos2.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(135.f), COL(255.f), 1.f)), 1.f);
			if (Config::ReplayEditor::ShowReplayFrames)
				drawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 2.f, ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(135.f), COL(255.f), 1.f)));
		}
	}
}

void Editor::ForceUpdateCursorPosition()
{
    auto& frames = _replayHandler.GetReplay()->ReplayFrames;

    if (frames.size() <= 1)
        return;

    if (currentFrame < frames.size() - 1)
    {
        int fC = currentFrame;
        if (fC != 0)
            while (Time < frames[fC].Time)
                fC--;
        while (Time > frames[fC].Time)
            fC++;
        currentFrame = fC;
    }
    else
    {
        int fC = currentFrame;
        while (Time < frames[fC].Time)
            fC--;
        currentFrame = fC;
    }
}

void Editor::Initialize()
{
	MilkThread mt = MilkThread(reinterpret_cast<uintptr_t>(TimerThread));
	_hitObjectManagerWrapper = HitObjectManagerWrapper();
	/*timerThread = std::thread(TimerThread);
	timerThread.detach();*/
}

void Editor::Render()
{
	Vector2 clientBounds = GameBase::GetClientSize();

	const ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SetNextWindowSize(ImVec2(clientBounds.X, clientBounds.Y));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin(xorstr_("re"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (Editor::drawList != drawList)
			Editor::drawList = drawList;

		drawList->AddRectFilled(ImVec2(0, 0), ImVec2(clientBounds.X, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(65.f), COL(65.f), COL(65.f), 1.f)));

		if (IsEditing)
			drawList->AddRectFilled(ImVec2(0, 0), ImVec2(clientBounds.X, clientBounds.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(65.f), COL(65.f), 0.2f)));

		if (osuPlayfield._isInit && customHomPlayMode == PlayModes::Osu)
			osuPlayfield.Render();

		DrawSelectedFrames(drawList);
		if (Dragging)
			drawList->AddRect(ImVec2(dragArea.first.X, dragArea.first.Y), ImVec2(dragArea.second.X, dragArea.second.Y), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), 0.65f)), 2.f, 0, 2.f);

#pragma region "Draw Topbar"
		float topBarHeight = ((clientBounds.Y * 3.75f) / 100.f) * StyleProvider::Scale;
		drawList->AddRectFilled(ImVec2(0, 0), ImVec2(clientBounds.X, topBarHeight), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

		ImGui::SetCursorPos(ImVec2(11 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		{
			if (Widgets::Button(xorstr_("Load"), ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			{
				Pause();
				if (!fileDialogInitialized)
				{
					fileDialog.SetTitle(xorstr_("Select osu! replay"));
					fileDialog.SetTypeFilters({ xorstr_(".osr") });

					fileDialogInitialized = true;
				}

				fileDialog.Open();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
			fileDialog.Display();
			ImGui::PopStyleVar();
			if (fileDialog.HasSelected())
			{
				Time = 0;
				strcpy_s(replayPath, fileDialog.GetSelected().string().c_str());
				fileDialog.ClearSelected();

				// parse
				_replayHandler = ReplayHandler(ReplayDecoder::Decode(replayPath));

				currentFrame = 0;

				// click timeline
				clickTimeline = ClickTimeline(&Time, drawList, _replayHandler.GetReplay(), clientBounds, nullptr);
				clickTimeline.ParseClicks();

				// click overlay
				clickOverlay = ClickOverlay(&Time, &clickTimeline.clicks, drawList, &clientBounds);

				// event timeline
				eventTimeline = EventTimeline(&Time, drawList, _replayHandler.GetReplay(), clientBounds, nullptr, 0, 0);
				// PARSE EVENTS AFTER BEATMAP LOADING XDDD

				//bm
				std::string beatmapHash = _replayHandler.GetReplayBeatmapHash();;
				LoadBeatmap(beatmapHash);
			}
		}

		ImGui::SetCursorPos(ImVec2(97 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button(xorstr_("Save"), ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(183 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button(xorstr_("Export"), ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(269 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		if (Widgets::Button(xorstr_("Exit"), ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
		{
			isVisible = false;
			ReplayBot::LoadFromReplayEditor(*_replayHandler.GetReplay());
		}
		ImGui::SetCursorPos(ImVec2(355 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		if (Widgets::Button(xorstr_("Options"), ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			optionsOpen = !optionsOpen;
		/*ImGui::SetCursorPos(ImVec2(441 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		if (Widgets::Button("Tools", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			Editor::toolsOpen = !Editor::toolsOpen;*/
		ImGui::SetCursorPos(ImVec2(750 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		ImGui::Text(xorstr_("In development, not representative of the final product."));
		//ImGui::SetCursorPos(ImVec2(355 - (OptionsSize.x / 2), topBarHeight) * StyleProvider::Scale);

#pragma endregion

		if (clickTimeline.IsInit)
			clickTimeline.Draw();

		if (eventTimeline.IsInit)
			eventTimeline.Draw();

		if (clickOverlay._isInit)
			clickOverlay.Render();
	}
	ImGui::End();

	if (optionsOpen)
	{
		ImGui::PushFont(StyleProvider::FontDefaultBold);
		const ImVec2 panelHeaderLabelSize = ImGui::CalcTextSize(xorstr_("Options"));
		const float panelHeaderHeight = panelHeaderLabelSize.y + StyleProvider::Padding.y * 2;
		ImGui::PopFont();

		ImGui::PushFont(StyleProvider::FontDefault);
		const float panelContentHeight = Widgets::CalcPanelHeight(5, 1);
		const ImVec2 windowSize = ImVec2(((clientBounds.X * 35.5f) / 100.f) * 0.8f, panelHeaderHeight + panelContentHeight) + StyleProvider::Padding * 2;

		ImGui::SetNextWindowSize(windowSize);
		ImGui::Begin(xorstr_("re_options"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		{
			const float optionsWidth = ImGui::GetWindowWidth();
			Widgets::BeginPanel(xorstr_("Options"), ImVec2(optionsWidth, panelContentHeight));
			{
				Widgets::Checkbox(xorstr_("Show Replay Frames"), &Config::ReplayEditor::ShowReplayFrames);
				Widgets::SliderInt(xorstr_("Frame Count"), &Config::ReplayEditor::FrameCount, 5, 50, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
				Widgets::SliderFloat(xorstr_("Timeline Resolution"), &Config::ReplayEditor::TimelineResolution, 0.25f, 2.f, 0.1f, 1, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);
				if (Widgets::SliderFloat(xorstr_("Playback rate"), &Config::ReplayEditor::PlaybackRate, 0.25f, 2.5f, 0.1f, 1, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput))
				{
					//AudioEngine::SetCurrentPlaybackRate(Config::ReplayEditor::PlaybackRate * 100.f);
				}
				Widgets::SliderInt(xorstr_("Edit Resync Time"), &Config::ReplayEditor::EditResyncTime, 50, 500, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
			}
			Widgets::EndPanel();
		}
		ImGui::PopFont();
		ImGui::End();
	}
}

void Editor::Pause()
{
	EditorState = Paused;
}

int64_t Editor::GetTicks()
{
	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);
	return ticks.QuadPart;
}

void Editor::Play()
{
	EditorState = Playing;
}

void Editor::TimerThread()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    int64_t b = GetTicks();
    while (true)
    {
        if (EditorState == Playing)
        {
            int64_t c = GetTicks();

            double dftDuration = (double)(c - b) * (Config::ReplayEditor::PlaybackRate * 1000.f) / (double)Frequency.QuadPart;

            if (dftDuration >= 1)
            {
                Time++;
                b = GetTicks();
                auto& frames = _replayHandler.GetReplay()->ReplayFrames;

                if (currentFrame < frames.size() - 1)
                {
                    int fC = currentFrame;
                    if (fC != 0)
                        while (Time < frames[fC].Time)
                            fC--;
                    while (Time > frames[fC].Time)
                        fC++;
                    currentFrame = fC;
                }
                else
                {
                    int fC = currentFrame;
                    while (Time < frames[fC].Time)
                        fC--;
                    currentFrame = fC;
                }
            }
        }
    }
}

void ReplayEditor::Editor::ToggleVisibility()
{
	isVisible = !isVisible;
}

bool ReplayEditor::Editor::GetIsVisible()
{
	return isVisible;
}

ReplayHandler& ReplayEditor::Editor::GetReplayHandler()
{
	return _replayHandler;
}

HitObjectManagerWrapper& ReplayEditor::Editor::GetHitObjectManagerWrapper()
{
	return _hitObjectManagerWrapper;
}