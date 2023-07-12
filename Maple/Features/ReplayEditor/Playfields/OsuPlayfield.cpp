#define NOMINMAX
#include "OsuPlayfield.h"

#include "../Editor.h"

ReplayEditor::OsuPlayfield::OsuPlayfield()
{
    _isInit = false;
    _drawList = nullptr;
    _beatmap = nullptr;
    _timer = nullptr;
    _hitObjects = nullptr;
    _currentFrame = nullptr;
    _osuCursor = OsuCursor();
    _drawables = std::vector<OsuDrawable*>();
}

ReplayEditor::OsuPlayfield::OsuPlayfield(ImDrawList* drawList, Beatmap* beatmap, int* timer, int* currentFrame, std::vector<HitObject>* hitObjects)
{
    _isInit = false;
    _drawList = drawList;
    _beatmap = beatmap;
    _timer = timer;
    _hitObjects = hitObjects;
    _currentFrame = currentFrame;
    _clientBounds = GameBase::GetClientSize();
    _osuCursor = OsuCursor(_currentFrame, _drawList);
    _drawables = std::vector<OsuDrawable*>();

    CalculatePlayareaCoordinates();
    ConstructDrawables();
    CalculateHits();
}

void ReplayEditor::OsuPlayfield::CalculateHits()
{
    const auto& replay = Editor::Get().GetReplayHandler().GetReplay();

    auto frame = replay->ReplayFrames.begin() + 1; // First frame cannot be click
    for (auto current = _hitObjects->begin(); current != _hitObjects->end(); ++current)
    {
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

        if (hitObject.IsType(HitObjectType::Spinner))
            continue;

        foundDrawable->SetHitObjectScoring(HitObjectScoring::Miss);

        // We can advance replay frames until we're sure the current object isn't clicked
        // This won't miss future objects because of notelock
        // Otherwise, we check against the next object starting from the next frame
        // This may not work for double clicking if both keys go down on the same frame, I guess?
        for (;
             frame != replay->ReplayFrames.end() && frame->Time <= hitObject.StartTime + Editor::Get().GetHitObjectManagerWrapper().GetHitWindow50();
             ++frame)
        {
            const auto& prevFrame = *(frame - 1);
            OsuKeys osuKeys = static_cast<OsuKeys>((int)frame->OsuKeys & ~(int)prevFrame.OsuKeys);
            if (((osuKeys & OsuKeys::K1) > OsuKeys::None || (osuKeys & OsuKeys::K2) > OsuKeys::None) && testHit(hitObject, *frame))
            {
                int accuracy = std::abs(frame->Time - hitObject.StartTime);
                auto determinedRange = getHitRange(accuracy);

                // Slidertick logic
                if (hitObject.IsType(HitObjectType::Slider))
                {
                    // Since sliderticks are already accounted for whenever this code is ran, we can just grab the event from the Slider
                    SliderOsu* slider = nullptr;
                    if (foundDrawable->GetDrawableType() == Drawable_HitObjectSliderOsu)
                        slider = dynamic_cast<SliderOsu*>(foundDrawable);
                    else if (foundDrawable->GetDrawableType() == Drawable_ApproachCircle)
                        slider = dynamic_cast<SliderOsu*>(dynamic_cast<ApproachCircle*>(foundDrawable)->GetLinkedObject());

                    // HOTFIX: Since we also have to take sliderbreaks into account, we recalculate the ticks here
                    slider->InitializeSliderBall();
                    slider->InitializeSliderTicks();

                    if (slider->GetHitObjectScoring() != HitObjectScoring::ThreeHundred && slider->GetHitObjectScoring() < determinedRange)
                        determinedRange = slider->GetHitObjectScoring();
                }

                foundDrawable->SetHitObjectScoring(determinedRange);

                ++frame;
                break;
            }
        }
    }
}

std::vector<ReplayEditor::OsuDrawable*> ReplayEditor::OsuPlayfield::GetDrawables()
{
    return _drawables;
}

void ReplayEditor::OsuPlayfield::CalculatePlayareaCoordinates()
{
    if (_clientBounds.X == 0 || _clientBounds.Y == 0)
        return;

    float playAreaHeight = PERC(_clientBounds.Y, 80.f);
    float playAreaWidth = playAreaHeight * (512.f / 386.f);

    Vector2 playAreaPosition = Vector2((_clientBounds.X / 2.f) - (playAreaWidth / 2.f), PERC(_clientBounds.Y, 10.f));

    EditorGlobals::PlayfieldSize = Vector2(playAreaWidth, playAreaHeight);
    EditorGlobals::PlayfieldOffset = playAreaPosition;
}

void ReplayEditor::OsuPlayfield::ConstructDrawables()
{
    const auto& replay = Editor::Get().GetReplayHandler().GetReplay();

    for (auto& hitObject : *_hitObjects)
    {
        if (hitObject.IsType(HitObjectType::Spinner))
            continue;

        int preempt = _beatmap->GetApproachRate();
        float cs = _beatmap->GetCircleSize();

        if ((replay->Mods & Mods::Easy) > Mods::None)
            preempt = std::max(0.f, preempt / 2.0f);
        if ((replay->Mods & Mods::HardRock) > Mods::None)
        {
            preempt = std::min(10.f, preempt * 1.4f);
            cs = std::min(10.f, cs * 1.3f);
        }

        if (preempt > 5.f)
            preempt = 1200.f + (450.f - 1200.f) * (preempt - 5.f) / 5.f;
        else if (preempt < 5.f)
            preempt = 1200.f - (1200.f - 1800.f) * (5.f - preempt) / 5.f;

        Transformation fadeIn =
            Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime - preempt + 400);
        Transformation fadeOut =
            Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.StartTime + Editor::Get().GetHitObjectManagerWrapper().GetHitWindow100(),
                           hitObject.StartTime + Editor::Get().GetHitObjectManagerWrapper().GetHitWindow50());

        if ((replay->Mods & Mods::Hidden) > Mods::None)
        {
            fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt,
                                    hitObject.StartTime - static_cast<int>(static_cast<double>(preempt) * 0.6f));
            fadeOut =
                Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.StartTime - static_cast<int>((static_cast<double>(preempt * 0.6f))),
                               hitObject.StartTime - static_cast<int>((static_cast<double>(preempt * 0.3f))));
        }

        Transformation fadeAr = Transformation(TransformationType::Fade, 0.f, 0.9f, hitObject.StartTime - preempt,
                                               std::min(hitObject.StartTime, hitObject.StartTime - preempt + 400 * 2));
        Transformation scaleAr = Transformation(TransformationType::Scale, 4.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime);

        HitObjectOsu* hitObjectOsu = new HitObjectOsu(hitObject.StartTime, preempt, _timer, hitObject.Position, fadeIn, hitObject.Count);
        hitObjectOsu->PushTransformation(fadeOut);

        ApproachCircle* approachCircle = new ApproachCircle(_timer, hitObject.Position, fadeAr, scaleAr, hitObjectOsu);
        if ((replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
            approachCircle->PushTransformation(fadeOut);

        if (!hitObject.IsType(HitObjectType::Slider))
        {
            _drawables.emplace_back(hitObjectOsu);
            if ((replay->Mods & Mods::Hidden) <= Mods::None || (replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
                _drawables.emplace_back(approachCircle);
            continue;
        }

        if ((replay->Mods & Mods::Hidden) > Mods::None)
        {
            fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt,
                                    hitObject.StartTime - static_cast<int>(static_cast<double>(preempt) * 0.6f));
            fadeOut =
                Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.EndTime - preempt + 400, hitObject.EndTime, EasingType::SliderHidden);
        }
        else
        {
            fadeIn = Transformation(TransformationType::Fade, 0.f, 1.f, hitObject.StartTime - preempt, hitObject.StartTime - preempt + 400);
            fadeOut = Transformation(TransformationType::Fade, 1.f, 0.f, hitObject.EndTime, hitObject.EndTime + 240);
        }

        SliderOsu* slider = new SliderOsu(&hitObject, hitObject.Count, hitObject.StartTime, preempt, _timer, hitObject.Position, hitObject.Velocity,
                                          hitObject.SegmentCount, fadeIn, hitObject.SliderScoreTimingPoints, hitObject.CumulativeLengths,
                                          hitObject.SliderCurveSmoothLines);

        approachCircle->SetLinkedObject(slider);
        if ((replay->Mods & Mods::Hidden) <= Mods::None || (replay->Mods & Mods::Hidden) > Mods::None && hitObject.Count == 0)
            _drawables.emplace_back(approachCircle);

        slider->PushTransformation(fadeOut);

        DrawSlider::CalculateCurves(slider, EditorGlobals::PlayfieldScale(CIRCLESIZE(cs)), (replay->Mods & Mods::HardRock) > Mods::None);
        slider->InitializeSliderBall();
        slider->InitializeSliderTicks();
        //_drawables.push_back(slider.GetSliderBall());
        _drawables.emplace_back(slider);
    }
    _isInit = true;
}

void ReplayEditor::OsuPlayfield::RenderPlayarea()
{
    _drawList->AddRectFilled(
        ImVec2(EditorGlobals::PlayfieldOffset.X, EditorGlobals::PlayfieldOffset.Y),
        ImVec2(EditorGlobals::PlayfieldOffset.X + EditorGlobals::PlayfieldSize.X, EditorGlobals::PlayfieldOffset.Y + EditorGlobals::PlayfieldSize.Y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(COL(51.f), COL(51.f), COL(51.f), 1.f)));
}

void ReplayEditor::OsuPlayfield::RenderDrawables()
{
    const auto& replay = Editor::Get().GetReplayHandler().GetReplay();

    float circleRadius = _beatmap->GetCircleSize();
    if ((replay->Mods & Mods::HardRock) > Mods::None)
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
    auto& hitObjectManagerWrapper = Editor::Get().GetHitObjectManagerWrapper();

    if (delta <= hitObjectManagerWrapper.GetHitWindow300())
        return HitObjectScoring::ThreeHundred;
    if (delta <= hitObjectManagerWrapper.GetHitWindow100())
        return HitObjectScoring::OneHundred;
    if (delta <= hitObjectManagerWrapper.GetHitWindow50())
        return HitObjectScoring::Fifty;

    return HitObjectScoring::Miss;
}

HitObjectScoring ReplayEditor::OsuPlayfield::testTimeMiss(HitObject hitObject, ReplayFrame replayFrame, int hitObjectIndex)
{
    auto preEmpt = Editor::Get().GetHitObjectManagerWrapper().GetPreempt();
    if ((hitObject.IsType(HitObjectType::Normal) || hitObject.IsType(HitObjectType::Slider)) && hitObjectIndex > 0)
    {
        auto& previousObject = (*_hitObjects)[hitObjectIndex - 1];
        if (previousObject.StackCount > 0 &&
            (replayFrame.Time >= previousObject.StartTime - preEmpt && replayFrame.Time <= previousObject.EndTime + 240) && !previousObject.IsHit)
            return HitObjectScoring::Ignore;
    }

    int hitWindow50 = Editor::Get().GetHitObjectManagerWrapper().GetHitWindow50();

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

bool ReplayEditor::OsuPlayfield::testHit(HitObject hitObject, ReplayFrame replayFrame)
{
    auto& hitObjectManagerWrapper = Editor::Get().GetHitObjectManagerWrapper();
    auto radius = hitObjectManagerWrapper.GetHitObjectRadius();
    int hitWindow50 = hitObjectManagerWrapper.GetHitWindow50();
    auto preEmpt = hitObjectManagerWrapper.GetPreempt();
    if ((hitObject.StartTime - preEmpt <= replayFrame.Time && hitObject.StartTime + hitWindow50 >= replayFrame.Time && !hitObject.IsHit) &&
        (Vector2(replayFrame.X, replayFrame.Y).DistanceSquared(hitObject.Position) <= radius * radius))
        return true;

    return false;
}

void ReplayEditor::OsuPlayfield::Render()
{
    if (_drawList == nullptr)
        return;
    if (_beatmap == nullptr)
        return;
    if (_timer == nullptr)
        return;
    if (_currentFrame == nullptr)
        return;
    if (_clientBounds.X == 0 || _clientBounds.Y == 0)
        return;

    RenderPlayarea();
    RenderDrawables();
    _osuCursor.Render();
}