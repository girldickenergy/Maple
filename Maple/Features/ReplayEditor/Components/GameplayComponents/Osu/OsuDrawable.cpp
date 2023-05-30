#include "OsuDrawable.h"

ReplayEditor::OsuDrawable::OsuDrawable() : Drawable(Drawable_Null, nullptr, Vector2())
{ 
}

ReplayEditor::OsuDrawable::OsuDrawable(DrawableType drawableType, int* timer, Vector2 position, Transformation transformation, int index)
	: Drawable(drawableType, timer, position, transformation, index)
{
	_hitObjectScoring = HitObjectScoring::Miss;
}

HitObjectScoring ReplayEditor::OsuDrawable::GetHitObjectScoring()
{
	return _hitObjectScoring;
}

void ReplayEditor::OsuDrawable::SetHitObjectScoring(HitObjectScoring hitObjectScoring)
{
	_hitObjectScoring = hitObjectScoring;
}

ImU32 ReplayEditor::OsuDrawable::GetHitColor()
{
	ImVec4 color = ImVec4(1.f, 1.f, 1.f, 0.f);
	switch (_hitObjectScoring)
	{
		case HitObjectScoring::ThreeHundred:
			color = ImVec4(COL(255.f), COL(255.f), COL(255.f), GetOpacity());
			break;
		case HitObjectScoring::OneHundred:
			color = ImVec4(COL(0.f), COL(255.f), COL(0.f), GetOpacity());
			break;
		case HitObjectScoring::Fifty: // Old color = 255, 230, 170
			color = ImVec4(COL(255.f), COL(230.f), COL(170.f), GetOpacity());
			break;
		case HitObjectScoring::Miss:
			color = ImVec4(COL(255.f), COL(0.f), COL(0.f), GetOpacity());
			break;

	}

	return ImGui::ColorConvertFloat4ToU32(color);
}
