#include "UI.h"

using namespace ReplayEditor;

void UI::DrawUI()
{
	Transformation tf = Transformation(TransformationType::Fade, 0.f, 1.f, 0, 100);
	HitObjectOsu hoo = HitObjectOsu(100, 50, Vector2(5.f, 1.f), tf);

	drawables.push(hoo);
}