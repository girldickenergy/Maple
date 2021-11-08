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
#define PERC(x, y) ( (x * y) / 100.f)

using namespace ReplayEditor;

inline bool init = false;

Vector2 Editor::ConvertToPlayArea(Vector2 position)
{
	float screen_position_x = (position.X / 512.f * playfieldSize.x) + playfieldOffset.x;
	float screen_position_y = (position.Y / 384.f * playfieldSize.y) + playfieldOffset.y;
	return Vector2(screen_position_x, screen_position_y);
}

void Editor::DrawCursor(ImDrawList* drawList, Vector2 cursorPosition)
{
	Vector2 cursorScreenPosition = ConvertToPlayArea(cursorPosition);
	printf("[x] x->%f y->%f\n", cursorScreenPosition.X, cursorScreenPosition.Y);
	drawList->AddCircleFilled(ImVec2(cursorScreenPosition.X, cursorScreenPosition.Y), 4.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
	drawList->AddCircle(ImVec2(cursorScreenPosition.X, cursorScreenPosition.Y), 12.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 0, 1.5f);
}

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

#pragma region Draw Playarea
		// Draw initial playarea
		float playAreaHeightDisplay = PERC(clientBounds->Height, 80.f);
		float playAreaWidthDisplay = playAreaHeightDisplay * (512.f / 386.f);

		float playAreaPositionX = (clientBounds->Width / 2.f) - (playAreaWidthDisplay / 2.f);
		float playAreaPositionY = PERC(clientBounds->Height, 10.f);
		playfieldOffset = ImVec2(playAreaPositionX, playAreaPositionY);

		playfieldSize = ImVec2(playAreaWidthDisplay, playAreaHeightDisplay);
		printf("[x] off  x->%f y->%f\n", playfieldOffset.x, playfieldOffset.y);
		printf("[x] size x->%f y->%f\n", playfieldSize.x, playfieldSize.y);
		drawList->AddRectFilled(playfieldOffset, ImVec2(playfieldOffset.x + playAreaWidthDisplay, playfieldOffset.y + playAreaHeightDisplay), ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));

		// Draw arrows
		float arrowWidth = PERC(playAreaWidthDisplay, 1.25f);
		float arrowLength = PERC(playAreaWidthDisplay, 10.f);

		float arrow1XOrigin = playAreaPositionX - arrowWidth;
		float arrow1YOrigin = playAreaPositionY - arrowWidth;
		float arrow1XDestin = arrow1XOrigin + arrowLength;
		float arrow1YDestin = arrow1YOrigin + arrowLength;
		ImVec2 arrow1Origin = ImVec2(arrow1XOrigin, arrow1YOrigin);
		ImVec2 arrow1XDesti = ImVec2(arrow1XDestin, arrow1YOrigin);
		ImVec2 arrow1YDesti = ImVec2(arrow1XOrigin, arrow1YDestin);


		printf("[x] arrow1  x->%f y->%f\n", arrow1Origin.x, arrow1Origin.y);
		drawList->AddRectFilled(arrow1Origin, arrow1XDesti, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
		drawList->AddRectFilled(arrow1Origin, arrow1YDesti, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

#pragma endregion
		
#pragma region Draw Topbar
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
#pragma endregion

		ImGui::SetCursorPos(ImVec2(10, ((clientBounds->Height * 18.75f) / 100.f)) * StyleProvider::Scale);
		ImVec2 ToolBoxSize = ImVec2(((clientBounds->Width * 15.5f) / 100.f) * 0.8f, Widgets::CalcPanelHeight(2)) * StyleProvider::Scale;
		Widgets::BeginPanel("Toolbox", ToolBoxSize);
		{
			Widgets::Button("Select", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ((ToolBoxSize.y - 4.f) / 2.f) - ((((ToolBoxSize.y - 4.f) / 2.f) * 10.f) / 100.f)));
			Widgets::Button("Keypress", ImVec2(ToolBoxSize.x - ((ToolBoxSize.x * 10.f) / 100.f), ((ToolBoxSize.y - 4.f) / 2.f) - ((((ToolBoxSize.y - 4.f) / 2.f) * 10.f) / 100.f)));
		}
		Widgets::EndPanel();

		DrawCursor(drawList, Vector2(0, 0));
		DrawCursor(drawList, Vector2(256, 193));
		DrawCursor(drawList, Vector2(512, 386));
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