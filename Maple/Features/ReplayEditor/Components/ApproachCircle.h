#pragma once
#include "Drawable.h"

namespace ReplayEditor
{
	class ApproachCircle : public Drawable
	{
		using Drawable::Drawable;

	public:
		ApproachCircle(Drawable* pairedDrawable, Transformation _fade, Transformation _scale);
	};
}