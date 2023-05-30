#define NOMINMAX
#include "OsuPlayfield.h"

ReplayEditor::OsuPlayfield::OsuPlayfield()
{
	_isInit = false;
	_drawList = nullptr;
	_replay = nullptr;
	_beatmap = nullptr;
	_hitObjectManager = NULL;
	_timer = nullptr;
	_hitObjects = nullptr;
	_currentFrame = nullptr;
	_osuCursor = OsuCursor();
	_hits = std::vector<std::pair<int, HitObjectScoring>>();
	_drawables = std::vector<OsuDrawable*>();
}

ReplayEditor::OsuPlayfield::OsuPlayfield(ImDrawList* drawList, Replay* replay, Beatmap* beatmap, uintptr_t hitObjectManager, int* timer, int* currentFrame, std::vector<HitObject>* hitObjects)
{
	_isInit = false;
	_drawList = drawList;
	_replay = replay;
	_beatmap = beatmap;
	_hitObjectManager = hitObjectManager;
	_timer = timer;
	_hitObjects = hitObjects;
	_currentFrame = currentFrame;
	_clientBounds = GameBase::GetClientSize();
	_osuCursor = OsuCursor(_currentFrame, _replay, _drawList);
	_hits = std::vector<std::pair<int, HitObjectScoring>>();
	_drawables = std::vector<OsuDrawable*>();

	CalculatePlayareaCoordinates();
	ConstructDrawables();
	CalculateHits();
}

void ReplayEditor::OsuPlayfield::CalculateHits()
{
	_hits.clear();

	auto frame = _replay->ReplayFrames.begin() + 1; // First frame cannot be click
	for (auto current = _hitObjects->begin(); current != _hitObjects->end(); ++current) {
		auto& hitObject = *current;

		// Find the drawable associated with this hitObject
		auto& foundDrawable = _drawables[0];
		for (auto drawable = _drawables.begin(); drawable != _drawables.end(); ++drawable)
		{
			if ((*drawable)->GetIndex() == current->Count)
			{
				foundDrawable = *drawable;
				break;
			}
		}

		if (hitObject.IsType(HitObjectType::Spinner)) continue;

		foundDrawable->SetHitObjectScoring(HitObjectScoring::Miss);

		// We can advance replay frames until we're sure the current object isn't clicked
		// This won't miss future objects because of notelock
		// Otherwise, we check against the next object starting from the next frame
		// This may not work for double clicking if both keys go down on the same frame, I guess?
		for (; frame != _replay->ReplayFrames.end() && frame->Time <= hitObject.StartTime + HitObjectManager::GetHitWindow50(_hitObjectManager); ++frame) {
			const auto prevFrame = *(frame - 1);
			OsuKeys osuKeys = static_cast<OsuKeys>((int)frame->OsuKeys & ~(int)prevFrame.OsuKeys);
			if (((osuKeys & OsuKeys::K1) > OsuKeys::None || (osuKeys & OsuKeys::K2) > OsuKeys::None) && testHit(hitObject, *frame, hitObject.Count)) {
				int accuracy = std::abs(frame->Time - hitObject.StartTime);
				auto determinedRange = getHitRange(accuracy);

				_hits.emplace_back(frame->Time, determinedRange);
				foundDrawable->SetHitObjectScoring(determinedRange);

				// Frame used
				++frame;
				break;
			}
		}
	}
}

std::vector<std::pair<int, HitObjectScoring>> ReplayEditor::OsuPlayfield::GetHits()
{
	return _hits;
}

void ReplayEditor::OsuPlayfield::CalculatePlayareaCoordinates()
{
	if (_clientBounds.X == 0 || _clientBounds.Y == 0) return;

	float playAreaHeight = PERC(_clientBounds.Y, 80.f);
	float playAreaWidth = playAreaHeight * (512.f / 386.f);

	Vector2 playAreaPosition = Vector2((_clientBounds.X / 2.f) - (playAreaWidth / 2.f), PERC(_clientBounds.Y, 10.f));

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

		Transformation fadeAr = Transformation(TransformationType::Fade, 0.f, 0.9f, hitObject.StartTime - preempt, std::min(hitObject.StartTime, hitObject.StartTime - preempt + 400 * 2));
		Transformation scaleAr = Transformation(TransformationType::Scale, 4.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime);

		if (hitObject.IsType(HitObjectType::Normal))
		{
			HitObjectOsu* hitObjectOsu = new HitObjectOsu(hitObject.StartTime, preempt, _timer, hitObject.Position, fadeIn, hitObject.Count);
			hitObjectOsu->PushTransformation(fadeOut);

			_drawables.emplace_back(hitObjectOsu);
			ApproachCircle* approachCircle = new ApproachCircle(_timer, hitObject.Position, fadeAr, scaleAr, hitObjectOsu, hitObject.Count);
			if ((_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
				approachCircle->PushTransformation(fadeOut);

			if ((_replay->Mods & Mods::Hidden) <= Mods::None || (_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
				_drawables.emplace_back(approachCircle);
		}

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

		SliderOsu* slider = new SliderOsu(&hitObject, hitObject.StartTime, preempt, _timer, hitObject.Position, hitObject.Velocity, hitObject.SegmentCount, 
			fadeIn, hitObject.SliderScoreTimingPoints, hitObject.CumulativeLengths, hitObject.SliderCurveSmoothLines);

		ApproachCircle* approachCircle = new ApproachCircle(_timer, hitObject.Position, fadeAr, scaleAr, slider, hitObject.Count);
		if ((_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
			approachCircle->PushTransformation(fadeOut);

		if ((_replay->Mods & Mods::Hidden) <= Mods::None || (_replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
			_drawables.emplace_back(approachCircle);

		slider->PushTransformation(fadeOut);

		DrawSlider::CalculateCurves(slider, EditorGlobals::PlayfieldScale(CIRCLESIZE(cs)), (_replay->Mods & Mods::HardRock) > Mods::None);
		slider->InitializeSliderBall();
		slider->InitializeSliderTicks();
		//_drawables.push_back(slider.GetSliderBall());
		_drawables.emplace_back(slider);
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
		switch (drawable->GetDrawableType())
		{
			case Drawable_HitObjectSliderOsu:
			{
				SliderOsu* sliderOsu = dynamic_cast<SliderOsu*>(drawable);
				if (sliderOsu->NeedsToDraw())
				{
					sliderOsu->DoTransformations();
					DrawSlider::Render(sliderOsu, _drawList, EditorGlobals::PlayfieldScale(circleRadius));
				}
				break;
			}
			case Drawable_HitObjectOsu:
			{
				HitObjectOsu* hitObjectOsu = dynamic_cast<HitObjectOsu*>(drawable);
				if (hitObjectOsu->NeedsToDraw())
				{
					hitObjectOsu->DoTransformations();
					Vector2 position = EditorGlobals::ConvertToPlayArea(hitObjectOsu->GetPosition());

					_drawList->AddCircle(ImVec2(position.X, position.Y), EditorGlobals::PlayfieldScale(circleRadius) * hitObjectOsu->GetScale(),
						hitObjectOsu->GetHitColor(), 0, 4.20f);
				}
				break;
			}
			case Drawable_ApproachCircle:
			{
				ApproachCircle* approachCircle = dynamic_cast<ApproachCircle*>(drawable);
				if (approachCircle->NeedsToDraw())
				{
					approachCircle->DoTransformations();
					Vector2 position = EditorGlobals::ConvertToPlayArea(approachCircle->GetPosition());

					_drawList->AddCircle(ImVec2(position.X, position.Y), EditorGlobals::PlayfieldScale(circleRadius) * approachCircle->GetScale(),
						approachCircle->GetHitColor(), 0, 4.20f);
				}
				break;
			}
		}
	}
}

HitObjectScoring ReplayEditor::OsuPlayfield::getHitRange(int delta)
{
	if (delta <= HitObjectManager::GetHitWindow300(_hitObjectManager)) return HitObjectScoring::ThreeHundred;
	if (delta <= HitObjectManager::GetHitWindow100(_hitObjectManager)) return HitObjectScoring::OneHundred;
	if (delta <= HitObjectManager::GetHitWindow50(_hitObjectManager))  return HitObjectScoring::Fifty;

	return HitObjectScoring::Miss;
}

HitObjectScoring ReplayEditor::OsuPlayfield::testTimeMiss(HitObject hitObject, ReplayFrame replayFrame, int hitObjectIndex)
{
	auto preEmpt = HitObjectManager::GetPreEmpt(reinterpret_cast<void*>(_hitObjectManager));
	if ((hitObject.IsType(HitObjectType::Normal) || hitObject.IsType(HitObjectType::Slider)) && hitObjectIndex > 0)
	{
		auto& previousObject = (*_hitObjects)[hitObjectIndex - 1];
		if (previousObject.StackCount > 0 &&
			(replayFrame.Time >= previousObject.StartTime - preEmpt &&
				replayFrame.Time <= previousObject.EndTime + 240) &&
			!previousObject.IsHit)
			return HitObjectScoring::Ignore;
	}

	int hitWindow50 = HitObjectManager::GetHitWindow50(_hitObjectManager);

	bool isNextCircle = true;

	for (auto it = _hitObjects->begin(); it != _hitObjects->begin() + hitObjectIndex; ++it)
	{
		auto& h = *it;
		if (h.StartTime + hitWindow50 <= replayFrame.Time || h.IsHit)
			continue;
		if (h.StartTime < hitObject.StartTime && std::distance(it, _hitObjects->begin() + hitObjectIndex) != hitObjectIndex)
			isNextCircle = false;
		break;
	}

	if (isNextCircle && std::abs(hitObject.StartTime - replayFrame.Time) < 400)
		return HitObjectScoring::Hit;

	return HitObjectScoring::Notelock;
}

bool ReplayEditor::OsuPlayfield::testHit(HitObject hitObject, ReplayFrame replayFrame, int hitObjectIndex)
{
	auto radius = HitObjectManager::GetHitObjectRadius(_hitObjectManager);
	int hitWindow50 = HitObjectManager::GetHitWindow50(_hitObjectManager);
	auto preEmpt = HitObjectManager::GetPreEmpt(_hitObjectManager);
	if ((hitObject.StartTime - preEmpt <= replayFrame.Time && hitObject.StartTime + hitWindow50 >= replayFrame.Time && !hitObject.IsHit) &&
		(Vector2(replayFrame.X, replayFrame.Y).DistanceSquared(hitObject.Position) <= radius * radius))
		return true;
	return false;
}

void ReplayEditor::OsuPlayfield::Render()
{
	if (_drawList == nullptr) return;
	if (_replay == nullptr) return;
	if (_beatmap == nullptr) return;
	if (_hitObjectManager == NULL) return;
	if (_timer == nullptr) return;
	if (_currentFrame == nullptr) return;
	if (_clientBounds.X == 0 || _clientBounds.Y == 0) return;

	RenderPlayarea();
	RenderDrawables();
	_osuCursor.Render();
}