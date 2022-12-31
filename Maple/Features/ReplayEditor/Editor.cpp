#include "Editor.h"
#include "../../Config/Config.h"
#pragma warning(disable: 26812) // Disable enum unscoped warning
#pragma warning(disable: 4996)

#include "../../Sdk/Beatmaps/BeatmapManager.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../Sdk/Player/Ruleset.h"
#include "../../UI/StyleProvider.h"
#include "../../UI/Widgets/Widgets.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include <Vanilla.h>

#include "Math/sRectangle.h"

#include "../../Dependencies/Milk/MilkThread.h"

#include "../../Logging/Logger.h"

using namespace ReplayEditor;

inline bool init = false;


void Editor::CreateHitObjectManager()
{
	if (customHomInstance != 0x00000000)
	{
		Vanilla::RemoveRelocation(std::ref(customHomInstance));
	}

	switch (selectedReplay.PlayMode)
	{
	case PlayModes::Osu:
	{
		typedef void* (__fastcall* fnCreateHom)(void*);

		customHomInstance = Ruleset::CreateHitObjectManager(NULL);
		Vanilla::AddRelocation(std::ref(customHomInstance));
		Logger::Log(LogSeverity::Debug, "HOM Instance Pointer: %X", customHomInstance);
		customHomPlayMode = selectedReplay.PlayMode;
	}
	break;
	}
}

void Editor::LoadBeatmap(std::string beatmapHash)
{
	if (customHomInstance == 0x00000000 || customHomPlayMode != selectedReplay.PlayMode)
		CreateHitObjectManager();

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	uintptr_t beatmapPointer = BeatmapManager::Get().GetBeatmapByChecksum(converter.from_bytes(selectedReplay.BeatmapHash));

	Logger::Log(LogSeverity::Debug, "Beatmap Pointer: %X", beatmapPointer);

	bmap = Beatmap(beatmapPointer);

	HitObjectManager::SetBeatmap(customHomInstance, beatmapPointer, selectedReplay.Mods);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::SetBeatmap");
	HitObjectManager::Load(customHomInstance, true, 0);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::Load");
	HitObjectManager::UpdateSlidersAll(customHomInstance, false);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::UpdateSlidersAll");
	HitObjectManager::UpdateStacking(customHomInstance, 0, -1);
	Logger::Log(LogSeverity::Debug, "Reached after HitObjectManager::UpdateStacking");

	Drawables.clear();
	hitObjects.clear();
	for (auto& object : HitObjectManager::HitObjects)
		if ((selectedReplay.Mods & Mods::HardRock) > Mods::None)
			object.Position = Vector2(object.Position.X, 384 - object.Position.Y);
	clickTimeline.SetHitObjects(&hitObjects);

	float cs = bmap.GetCircleSize();
	if ((selectedReplay.Mods & Mods::HardRock) > Mods::None)
		cs = std::min(10.f, cs * 1.3f);

	Logger::Log(LogSeverity::Debug, "Reached before eventTimeline.SetHomInstance");
	eventTimeline.SetHomInstance(customHomInstance);
	eventTimeline.SetHitObjects(&hitObjects);
	eventTimeline.SetCircleSize(cs);
	eventTimeline.SetOverallDifficulty(bmap.GetOverallDifficulty());
	Logger::Log(LogSeverity::Debug, "after before eventTimeline.SetOverallDifficulty");

	osuPlayfield = OsuPlayfield(drawList, &Editor::selectedReplay, &Editor::bmap, customHomInstance,
	                                          &Time, &Editor::currentFrame, &hitObjects);
	eventTimeline.ParseEvents();

	Logger::Log(LogSeverity::Debug, "Reached before BeatmapManager::Get().Load()");
	Logger::Log(LogSeverity::Debug, "Pointers: VOID %X | UINT %X", reinterpret_cast<void*>(bmap.GetBeatmapOsuPointer()), bmap.GetBeatmapOsuPointer());
	BeatmapManager::Get().Load(reinterpret_cast<void*>(bmap.GetBeatmapOsuPointer()));
	Logger::Log(LogSeverity::Debug, "Reached until after BeatmapManager::Get().Load()");
	AudioEngine::TogglePause();
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
	if (selectedReplay.ReplayFrames.size() <= 1) return;

	if (currentFrame < selectedReplay.ReplayFrames.size() - 1)
	{
		int fC = currentFrame;
		if (fC != 0)
			while (Time < selectedReplay.ReplayFrames[fC].Time)
				fC--;
		while (Time > selectedReplay.ReplayFrames[fC].Time)
			fC++;
		currentFrame = fC;
	}
	else
	{
		int fC = currentFrame;
		while (Time < selectedReplay.ReplayFrames[fC].Time)
			fC--;
		currentFrame = fC;
	}
}

Editor::Editor(SingletonLock)
{
}

void Editor::Initialize()
{
	MilkThread mt = MilkThread(reinterpret_cast<uintptr_t>(TimerThread));
	/*timerThread = std::thread(TimerThread);
	timerThread.detach();*/
}

void Editor::Render()
{
	Vector2 clientBounds = GameBase::GetClientSize();

	ImGui::SetNextWindowSize(ImVec2(clientBounds.X, clientBounds.Y));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("re", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
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
			if (Widgets::Button("Load", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			{
				Pause();
				if (!fileDialogInitialized)
				{
					fileDialog.SetTitle("Select osu! replay");
					fileDialog.SetTypeFilters({ ".osr" });

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
				selectedReplay = ReplayDecoder::Decode(replayPath);
				currentFrame = 0;

				// remove first two replay frames since it's garbage anyways
				selectedReplay.ReplayFrames.erase(selectedReplay.ReplayFrames.begin(),
				                                  selectedReplay.ReplayFrames.begin() + 2);

				std::vector<int> toRemove = std::vector<int>();
				auto begin = selectedReplay.ReplayFrames.begin();
				for (auto it = begin; it != selectedReplay.ReplayFrames.end(); ++it)
				{
					auto& frame = *it;
					if (std::distance(begin, it) > 0)
					{
						auto& previousFrame = *(it - 1);
						if (frame.Time == previousFrame.Time)
							toRemove.push_back(std::distance(begin, it));
					}
				}

				for (auto& idx : toRemove)
					selectedReplay.ReplayFrames.erase(selectedReplay.ReplayFrames.begin() + idx);

				// click timeline
				clickTimeline = ClickTimeline(&Time, drawList, &Editor::selectedReplay, &clientBounds, nullptr);
				clickTimeline.ParseClicks();

				// click overlay
				clickOverlay = ClickOverlay::ClickOverlay(&Time, &clickTimeline.clicks, drawList, &clientBounds);

				// event timeline
				eventTimeline = EventTimeline(&Time, drawList, &Editor::selectedReplay, &clientBounds, nullptr, 0, 0, customHomInstance);
				// PARSE EVENTS AFTER BEATMAP LOADING XDDD

				//bm
				std::string beatmapHash = selectedReplay.BeatmapHash;
				LoadBeatmap(beatmapHash);
			}
		}

		ImGui::SetCursorPos(ImVec2(97 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Save", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(183 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Export", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(269 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Exit", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(355 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		if (Widgets::Button("Options", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			optionsOpen = !optionsOpen;
		/*ImGui::SetCursorPos(ImVec2(441 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		if (Widgets::Button("Tools", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2)))
			Editor::toolsOpen = !Editor::toolsOpen;*/
		ImGui::SetCursorPos(ImVec2(750 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		ImGui::Text("In development, not representative of the final product.");

		ImVec2 OptionsSize = ImVec2(((clientBounds.X * 35.5f) / 100.f) * 0.8f, Widgets::CalcPanelHeight(6)) * StyleProvider::Scale;
		ImGui::SetCursorPos(ImVec2(355 - (OptionsSize.x / 2), topBarHeight) * StyleProvider::Scale);
		if (optionsOpen)
		{
			Widgets::BeginPanel("Options", OptionsSize);
			{
				Widgets::Checkbox("Show Replay Frames", &Config::ReplayEditor::ShowReplayFrames);
				Widgets::SliderInt("Frame Count", &Config::ReplayEditor::FrameCount, 5, 50, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput);
				Widgets::SliderFloat("Timeline Resolution", &Config::ReplayEditor::TimelineResolution, 0.25f, 2.f, 0.1f, 1, "%.1f", ImGuiSliderFlags_ClampOnInput);
				if (Widgets::SliderFloat("Playback rate", &Config::ReplayEditor::PlaybackRate, 0.25f, 2.5f, 0.1f, 1, "%.1f", ImGuiSliderFlags_ClampOnInput))
				{
					AudioEngine::SetCurrentPlaybackRate(Config::ReplayEditor::PlaybackRate * 100.f);
				}
				Widgets::SliderInt("Edit Resync Time", &Config::ReplayEditor::EditResyncTime, 50, 500, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput);
#if _DEBUG
				if (Widgets::Button("Debug Tools"))
					Editor::debugToolsOpen = !Editor::debugToolsOpen;
#endif
			}
			Widgets::EndPanel();
		}

		/*ImVec2 ToolsSize = ImVec2(((clientBounds->Width * 25.5f) / 100.f) * 0.8f, Widgets::CalcPanelHeight(2)) * StyleProvider::Scale;
		ImGui::SetCursorPos(ImVec2(441 - (ToolsSize.x / 2), topBarHeight) * StyleProvider::Scale);
		if (Editor::toolsOpen)
		{
			Widgets::BeginPanel("Tools", ToolsSize);
			{
				Widgets::Button("");
			}
			Widgets::EndPanel();
		}*/

#pragma endregion

		/*ImGui::SetCursorPos(ImVec2(10, ((clientBounds->Height * 22.75f) / 100.f)) * StyleProvider::Scale);
		ImVec2 ToolBoxSize = ImVec2(((clientBounds->Width * 15.5f) / 100.f) * 0.8f, Widgets::CalcPanelHeight(2)) * StyleProvider::Scale;
		Widgets::BeginPanel("Toolbox", ToolBoxSize);
		{
			if (Widgets::Button("Select", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ((ToolBoxSize.y - 4.f) / 2.f) - ((((ToolBoxSize.y - 4.f) / 2.f) * 10.f) / 100.f))))
			{
				Editor::ToolsetState = ToolsetState::Select;
			}
			if (Widgets::Button("Keypress", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ((ToolBoxSize.y - 4.f) / 2.f) - ((((ToolBoxSize.y - 4.f) / 2.f) * 10.f) / 100.f))))
			{

			}
		}
		Widgets::EndPanel();*/

		if (clickTimeline.IsInit)
			clickTimeline.Draw();

		if (eventTimeline.IsInit)
			eventTimeline.Draw();

		if (clickOverlay._isInit)
			clickOverlay.Render();
	}
	ImGui::End();
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
		if (EditorState == Playing) {
			int64_t c = GetTicks();

			double dftDuration = (double)(c - b) * (Config::ReplayEditor::PlaybackRate * 1000.f) / (double)Frequency.QuadPart;

			if (dftDuration >= 1) {
				Time++;
				b = GetTicks();

				if (currentFrame < selectedReplay.ReplayFrames.size() - 1)
				{
					int fC = currentFrame;
					if (fC != 0)
						while (Time < selectedReplay.ReplayFrames[fC].Time)
							fC--;
					while (Time > selectedReplay.ReplayFrames[fC].Time)
						fC++;
					currentFrame = fC;
				}
				else
				{
					int fC = currentFrame;
					while (Time < selectedReplay.ReplayFrames[fC].Time)
						fC--;
					currentFrame = fC;
				}
			}
		}
	}
}