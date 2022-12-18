#define NOMINMAX
#include "OsuPlayfield.h"

void ReplayEditor::OsuPlayfield::CalculatePlayareaCoordinates()
{
	if (_clientBounds == nullptr) return;

	float playAreaHeight = PERC(_clientBounds->Height, 80.f);
	float playAreaWidth = playAreaHeight * (512.f / 386.f);

	Vector2 playAreaPosition = Vector2((_clientBounds->Width / 2.f) - (playAreaWidth / 2.f), PERC(_clientBounds->Height, 10.f));

	EditorGlobals::PlayfieldSize = Vector2(playAreaWidth, playAreaHeight);
	EditorGlobals::PlayfieldOffset = playAreaPosition;
}

void ReplayEditor::OsuPlayfield::ConstructDrawables()
{
	for (auto& hitObject : *_hitObjects)
	{
		if (hitObject.IsType(HitObjectType::Spinner)) continue;

		int preempt = _beatmap->GetApproachRate();
		float cs = _beatmap->GetCircleSize();
		
		if ((_replay->Mods & Mods::Easy) > Mods::None)
			preempt = std::max(0.f, preempt / 2.0f);
		if ((_replay->Mods & Mods::HardRock) > Mods::None)
		{
			preempt = std::min(10.f, preempt * 1.4f);
			cs = std::min(10.f, cs * 1.3f);
		}

		if (preempt > 5.f)
			preempt = 1200.f + (450.f - 1200.f) * (preempt - 5.f) / 5.f;
		else if (preempt < 5.f)
			preempt = 1200.f - (1200.f - 1800.f) * (5.f - preempt) / 5.f;

		Transformation fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime - preempt + 400);
		Transformation fadeOut = Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.StartTime + HitObjectManager::GetHitWindow100(_hitObjectManager),
			hitObject.StartTime + HitObjectManager::GetHitWindow50(_hitObjectManager));

		if ((_replay->Mods & Mods::Hidden) > Mods::None)
		{
			fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime - static_cast<int>(static_cast<double>(preempt) * 0.6f));
			fadeOut = Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.StartTime - static_cast<int>((static_cast<double>(preempt * 0.6f))),
				hitObject.StartTime - static_cast<int>((static_cast<double>(preempt * 0.3f))));
		}

		HitObjectOsu hitObjectOsu = HitObjectOsu(hitObject.StartTime, preempt, _timer, hitObject.Position, fadeIn, hitObject.Count);
		hitObjectOsu.PushTransformation(fadeOut);

		_drawables.push_back(hitObjectOsu);
		
		Transformation fadeAr = Transformation(TransformationType::Fade, 0.f, 0.9f, hitObject.StartTime - preempt, std::min(hitObject.StartTime, hitObject.StartTime - preempt + 400 * 2));
		Transformation scaleAr = Transformation(TransformationType::Scale, 4.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime);

		ApproachCircle approachCircle = ApproachCircle(_timer, hitObject.Position, fadeAr, scaleAr, hitObject.Count);
		if ((_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
			approachCircle.PushTransformation(fadeOut);

		if ((_replay->Mods & Mods::Hidden) <= Mods::None || (_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
			_drawables.push_back(approachCircle);

		if (!hitObject.IsType(HitObjectType::Slider)) continue;
		
		if ((_replay->Mods & Mods::Hidden) > Mods::None)
		{
			fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime -
				static_cast<int>(static_cast<double>(preempt) * 0.6f));
			fadeOut = Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.EndTime - preempt + 400,
				hitObject.EndTime, EasingType::SliderHidden);
		}
		else
		{
			fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime - preempt + 400);
			fadeOut = Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.EndTime, hitObject.EndTime + 240);
		}

		SliderOsu slider = SliderOsu(hitObject.StartTime, preempt, _timer, hitObject.Position, fadeIn, hitObject.SliderCurveSmoothLines);
		DrawSlider::CalculateCurves(&slider, EditorGlobals::PlayfieldScale(CIRCLESIZE(cs)), (_replay->Mods & Mods::HardRock) > Mods::None);
		slider.PushTransformation(fadeOut);
		_drawables.push_back(slider);
	}
	_isInit = true;
}

void ReplayEditor::OsuPlayfield::RenderPlayarea()
{
	_drawList->AddRectFilled(ImVec2(EditorGlobals::PlayfieldOffset.X, EditorGlobals::PlayfieldOffset.Y),
		ImVec2(EditorGlobals::PlayfieldOffset.X + EditorGlobals::PlayfieldSize.X, EditorGlobals::PlayfieldOffset.Y + EditorGlobals::PlayfieldSize.Y),
		ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));
}

void ReplayEditor::OsuPlayfield::RenderDrawables()
{
	float circleRadius = _beatmap->GetCircleSize();
	if ((_replay->Mods & Mods::HardRock) > Mods::None)
		circleRadius = std::min(circleRadius * 1.3f, 10.f);
	circleRadius = CIRCLESIZE(circleRadius);

	for (auto& drawable : _drawables)
	{
		switch (std::type_index(drawable.type()).hash_code())
		{
			case 1696882241:
			{
				SliderOsu sliderOsu = std::any_cast<SliderOsu>(drawable);
				if (sliderOsu.NeedsToDraw())
				{
					sliderOsu.DoTransformations();
					DrawSlider::Render(&sliderOsu, _drawList, EditorGlobals::PlayfieldScale(circleRadius));
				}
				break;
			}
			case 2753814496:
			{
				HitObjectOsu hitObjectOsu = std::any_cast<HitObjectOsu>(drawable);
				if (hitObjectOsu.NeedsToDraw())
				{
					const auto& currentHitObject = _hitObjects->at(hitObjectOsu.GetIndex());
					hitObjectOsu.DoTransformations();
					Vector2 position = EditorGlobals::ConvertToPlayArea(hitObjectOsu.GetPosition());
					bool isHit = currentHitObject.IsHit;
					ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), hitObjectOsu.GetOpacity()));
					if (!isHit)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(0.f), COL(0.f), hitObjectOsu.GetOpacity()));
					if (currentHitObject.Is100)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(255.f), COL(0.f), hitObjectOsu.GetOpacity()));
					if (currentHitObject.Is50)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(0.f), COL(255.f), hitObjectOsu.GetOpacity()));
					_drawList->AddCircle(ImVec2(position.X, position.Y), EditorGlobals::PlayfieldScale(circleRadius) * hitObjectOsu.GetScale(),
						color, 0, 4.20f);
				}
				break;
			}
			case 2215551597:
			{
				ApproachCircle approachCircle = std::any_cast<ApproachCircle>(drawable);
				if (approachCircle.NeedsToDraw())
				{
					const auto& currentHitObject = _hitObjects->at(approachCircle.GetIndex());
					approachCircle.DoTransformations();
					Vector2 position = EditorGlobals::ConvertToPlayArea(approachCircle.GetPosition());
					bool isHit = currentHitObject.IsHit;
					ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(255.f), COL(255.f), approachCircle.GetOpacity()));
					if (!isHit)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(255.f), COL(0.f), COL(0.f), approachCircle.GetOpacity()));
					if (currentHitObject.Is100)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(255.f), COL(0.f), approachCircle.GetOpacity()));
					if (currentHitObject.Is50)
						color = ImGui::ColorConvertFloat4ToU32(ImVec4(COL(0.f), COL(0.f), COL(255.f), approachCircle.GetOpacity()));
					_drawList->AddCircle(ImVec2(position.X, position.Y), EditorGlobals::PlayfieldScale(circleRadius) * approachCircle.GetScale(),
						color, 0, 4.20f);
				}
				break;
			}
		}
	}
}

ReplayEditor::OsuPlayfield::OsuPlayfield()
{
	_isInit = false;
	_drawList = nullptr;
	_replay = nullptr;
	_beatmap = nullptr;
	_hitObjectManager = nullptr;
	_timer = nullptr;
	_hitObjects = nullptr;
	_currentFrame = nullptr;
	_clientBounds = nullptr;
	_osuCursor = OsuCursor::OsuCursor();
	_drawables = std::vector<std::any>();
}

ReplayEditor::OsuPlayfield::OsuPlayfield(ImDrawList* drawList, Replay* replay, Beatmap* beatmap, void* hitObjectManager, int* timer, int* currentFrame, std::vector<HitObject>* hitObjects)
{
	_isInit = false;
	_drawList = drawList;
	_replay = replay;
	_beatmap = beatmap;
	_hitObjectManager = hitObjectManager;
	_timer = timer;
	_hitObjects = hitObjects;
	_currentFrame = currentFrame;
	_clientBounds = GameBase::GetClientBounds();
	_osuCursor = OsuCursor::OsuCursor(_currentFrame, _replay, _drawList);
	_drawables = std::vector<std::any>();

	CalculatePlayareaCoordinates();
	ConstructDrawables();
}

void ReplayEditor::OsuPlayfield::Render()
{
	if (_drawList == nullptr) return;
	if (_replay == nullptr) return;
	if (_beatmap == nullptr) return;
	if (_hitObjectManager == nullptr) return;
	if (_timer == nullptr) return;
	if (_currentFrame == nullptr) return;
	if (_clientBounds == nullptr) return;

	RenderPlayarea();
	RenderDrawables();
	_osuCursor.Render();
}