#include "Editor.h"
#pragma warning(disable: 26812) // Disable enum unscoped warning
#define NOMINMAX

using namespace ReplayEditor;

void Editor::DrawUI()
{
	Transformation tf = Transformation(TransformationType::Fade, 0.f, 1.f, 0, 100);
	HitObjectOsu hoo = HitObjectOsu(100, 50, &Time, Vector2(5.f, 1.f), tf);
	
	Transformation scale = Transformation(TransformationType::Scale, 0.f, 2.f, 10, 100);
	hoo.PushTransformation(scale);
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

	Drawables.emplace_back(hitObject.StartTime, preEmpt, &Time, hitObject.Position, fade);

	/* x-x Construct ApproachCircle for previous HitObject x-x */
	Transformation fadeAR = Transformation(TransformationType::Fade, 0.f, 0.9f, hitObject.StartTime - preEmpt, std::min(hitObject.StartTime, hitObject.StartTime - preEmpt + 400 * 2));
	Transformation scaleAR = Transformation(TransformationType::Scale, 4.f, 1.f, hitObject.StartTime - preEmpt, hitObject.StartTime);

	Drawables.emplace_back(&Drawables.back(), fadeAR, scaleAR);
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
	}
}