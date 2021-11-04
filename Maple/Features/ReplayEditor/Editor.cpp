#include "Editor.h"
#include "../../Config/Config.h"
#pragma warning(disable: 26812) // Disable enum unscoped warning
#define NOMINMAX

using namespace ReplayEditor;

inline bool init = false;

void Editor::Initialize()
{
	
}

void Editor::DrawUI()
{
	// TODO: THIS ENTIRE FUNCTION IS A UNIT TEST
	//			DO NOT DEPLOY THIS!
	if (!init)
	{
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(TimerThread), nullptr, 0, nullptr);

		HitObject ho = HitObject(HitObjectType::Normal, 5000, 5000, Vector2(256, 256), Vector2(256, 256), 0, 0);
		HitObject ho1 = HitObject(HitObjectType::Normal, 6000, 6000, Vector2(200, 256), Vector2(200, 256), 0, 0);
		HitObject ho2 = HitObject(HitObjectType::Normal, 7000, 7000, Vector2(400, 256), Vector2(400, 256), 0, 0);
		HitObject ho3 = HitObject(HitObjectType::Normal, 8000, 8000, Vector2(256, 256), Vector2(256, 256), 0, 0);
		HitObject ho4 = HitObject(HitObjectType::Normal, 9000, 9000, Vector2(200, 256), Vector2(200, 256), 0, 0);

		ConstructDrawable(ho, 10.f, Mods::None);
		ConstructDrawable(ho1, 10.f, Mods::None);
		ConstructDrawable(ho2, 10.f, Mods::None);
		ConstructDrawable(ho3, 10.f, Mods::None);
		ConstructDrawable(ho4, 10.f, Mods::None);

		init = true;
	}

	if (Config::AimAssist::DrawDebugOverlay && init) {
		if (EditorState != EditorState::Playing)
			Play();
		ImGui::SetNextWindowSize(ImVec2(GameBase::GetClientBounds()->Width, GameBase::GetClientBounds()->Height));
		ImGui::Begin("re", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			for (auto& drawable : Drawables)
			{
				if (drawable.NeedsToDraw())
				{
					drawable.DoTransformations();
					Vector2 screenPos = GameField::FieldToDisplay(drawable.GetPosition());
					drawList->AddCircleFilled(ImVec2(screenPos.X, screenPos.Y), 55.f * drawable.GetScale(), ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 255.f, 0.f, drawable.GetOpacity())));
				}
			}
		}
		ImGui::End();
	}
}

void Editor::ConstructDrawable(HitObject hitObject, float approachRate, Mods mods)
{
	int preEmpt = approachRate;
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