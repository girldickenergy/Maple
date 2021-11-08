#include "Editor.h"
#include "../../Config/Config.h"
#pragma warning(disable: 26812) // Disable enum unscoped warning
#define NOMINMAX

#include "../../Sdk/Beatmaps/Beatmap.h"
#include "../../Sdk/Beatmaps/BeatmapManager.h"
#include "../../Sdk/Player/HitObjectManager.h"
#include "../../UI/StyleProvider.h"
#include "../../UI/Widgets.h"

#define COL(x) (x/255.f)

using namespace ReplayEditor;

inline bool init = false;

void Editor::Initialize()
{
	
}

void Editor::DrawUI()
{
	sRectangle* clientBounds = GameBase::GetClientBounds();

	ImGui::SetNextWindowSize(ImVec2(clientBounds->Width, clientBounds->Height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("re", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		
		// Draw background 65,65,65
		drawList->AddRectFilled(ImVec2(0,0), ImVec2(clientBounds->Width, clientBounds->Height), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(65.f), COL(65.f), COL(65.f), 1.f)));

		// Draw the top bar 
		// Width = Screen Width, Height = 5.75% of screen height * Scale
		float topBarHeight = ((clientBounds->Height * 3.75f) / 100.f) * StyleProvider::Scale;
		drawList->AddRectFilled(ImVec2(0, 0), ImVec2(clientBounds->Width, topBarHeight) , ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

		ImGui::SetCursorPos(ImVec2(11 * StyleProvider::Scale, (topBarHeight*19.f)/100.f));
		Widgets::Button("Load", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(97 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Save", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(183 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Export", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		ImGui::SetCursorPos(ImVec2(269 * StyleProvider::Scale, (topBarHeight * 19.f) / 100.f));
		Widgets::Button("Exit", ImVec2(75 * StyleProvider::Scale, topBarHeight - ((topBarHeight * 19.f) / 100.f) * 2));
		//ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight()))

		ImGui::SetCursorPos(ImVec2(10, ((clientBounds->Height * 18.75f) / 100.f)) * StyleProvider::Scale);
		ImVec2 ToolBoxSize = ImVec2((((clientBounds->Width * 15.5f) / 100.f) * StyleProvider::Scale), Widgets::CalcPanelHeight(2));
		Widgets::BeginPanel("Toolbox", ToolBoxSize);
		{
			Widgets::Button("Select", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ToolBoxSize.y - ((ToolBoxSize.y * 10.f) / 100.f)) * StyleProvider::Scale);
			Widgets::Button("Keypress", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ToolBoxSize.y - ((ToolBoxSize.y * 10.f) / 100.f)) * StyleProvider::Scale);
		}
		Widgets::EndPanel();
	}
	ImGui::End();
}

void Editor::ConstructDrawable(HitObject hitObject, float approachRate, float overallDifficulty, Mods mods)
{
	int preEmpt = approachRate;
	int hitWindow300 = 0, hitWindow100 = 0, hitWindow50 = 0;
	if ((mods & Mods::Easy) > Mods::None) 
		preEmpt = std::max(0.0f, preEmpt / 2.0f);
	if ((mods & Mods::HardRock) > Mods::None) 
		preEmpt = std::min(10.0f, preEmpt * 1.4f);

	if (preEmpt > 5.0f)
		preEmpt = 1200.f + (450.f - 1200.f) * (preEmpt - 5.f) / 5.f;
	else if (preEmpt < 5.0f)
		preEmpt = 1200.f - (1200.f - 1800.f) * (5.f - preEmpt) / 5.f;

	/* x-x Construct HitObject x-x */
	Transformation fade = Transformation();
	if ((mods & Mods::Hidden) > Mods::None)
		fade = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preEmpt, hitObject.StartTime - static_cast<int>(preEmpt * 0.6f));
	else
		fade = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preEmpt, hitObject.StartTime - preEmpt + 400);

	HitObjectOsu hoo = HitObjectOsu(hitObject.StartTime, preEmpt, &Time, hitObject.Position, fade);

	Drawables.push_back(hoo);

	/* x-x Construct ApproachCircle for previous HitObject x-x */
	Transformation fadeAR = Transformation(TransformationType::Fade, 0.f, 0.9f, hitObject.StartTime - preEmpt, std::min(hitObject.StartTime, hitObject.StartTime - preEmpt + 400 * 2));
	Transformation scaleAR = Transformation(TransformationType::Scale, 4.f, 1.f, hitObject.StartTime - preEmpt, hitObject.StartTime);

	ApproachCircle ac = ApproachCircle(&Drawables.back(), fadeAR, scaleAR);

	Drawables.push_back(ac);

	printf("[x] constructed\n");
}

void Editor::Pause()
{
	EditorState = EditorState::Paused;
}

void Editor::Play()
{
	EditorState = EditorState::Playing;
}

void Editor::TimerThread()
{
	while (true)
	{
		if (EditorState == EditorState::Playing)
			Time++;

		Sleep(1);
	}
}