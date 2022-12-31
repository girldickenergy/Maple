#pragma once
#include <Math/Vector2.h>
#include "Transformation.h"
#include <vector>

namespace ReplayEditor
{
	/// @brief An enum that specifies which type of Drawable is being used.
	enum DrawableType : int
	{
		/* osu! drawables */
		Drawable_ApproachCircle,
		Drawable_HitObjectOsu,
		Drawable_HitObjectSliderOsu,
		Drawable_HitObjectSpinnerOsu,

		/* catch the beat drawables */
		Drawable_CatchFruit,
		Drawable_CatchJuiceDrop,
		Drawable_CatchDroplet
	};

	/// @class Drawable
	/// @brief Base implementation for all (animated) non-gui elements on screen.
	class Drawable
	{
		/// @brief The DrawableType of the current drawable.
		DrawableType _drawableType;

		/// @brief Position of the current drawable.
		Vector2 _position;

		/// @brief Vector of transformation, i.e. animations that will be applied to the current drawable.
		std::vector<Transformation> _transformations;

		/// @brief Pointer to the timer integer from the editor instance.
		int* _timer;

		/// @brief Calculated opacity of the current drawable.
		float _opacity;

		/// @brief Calculated scale of the current drawable.
		float _scale;

		/// @brief The index of the drawable. May corelate to the HitObject index.
		int _index;
	public:
		/// @brief Constructs a non-gui drawable element for drawing on the screen.
		/// @param drawableType An enum type specifying the drawable type.
		/// @param timer Pointer to the timer integer of the editor instance.
		/// @param position The playfield position of the drawable.
		/// @param transformation The base transformation to apply to the drawable.
		Drawable(DrawableType drawableType, int* timer, Vector2 position, Transformation transformation = Transformation(), int index = 0);

		/// @brief Returns the type of the current drawable.
		/// @return DrawableType
		DrawableType GetDrawableType();

		/// @brief Returns the playfield position of the current drawable.
		/// @return Vector2
		Vector2 GetPosition();

		/// @brief Returns a vector of all added transformation of the current drawable.
		/// @return std::vector Transformation
		std::vector<Transformation> GetTransformations();

		/// @brief Returns a tuple of two ints containing the start drawing time and the end drawing time of the current drawable.
		/// @return std::tuple DrawingTimes
		std::tuple<int, int> GetDrawingTimes();

		/// @brief Returns a value of 'true' when the drawable needs to be drawn on screen.
		/// @return bool
		bool NeedsToDraw();

		/// @brief Returns the timer pointer which is passed from the editor instance.
		/// @return int*
		int* GetTimer();

		/// @brief Adds a transformation (animation) onto the current drawable's transformation vector.
		/// @param transformation New transformation (animation) to add
		void PushTransformation(Transformation transformation);

		/// @brief Runs the calculations for the specified transformation (animation) and sets \c _opacity or \c _scale accordingly.
		/// @param transformation The transformation (animation) to "run" and apply the transformation.
		void ApplyTransformation(Transformation transformation);

		/// @brief Loops over all transformations and calls \c ApplyTransformation() to "run" and apply the transformations.
		void DoTransformations();

		/// @brief Returns the _opacity field
		/// @return float Opacity
		float GetOpacity();

		/// @brief Returns the _scale field
		/// @return float Scale
		float GetScale();

		/// @brief Returns the _index field
		/// @return int Index
		int GetIndex();
	};
}