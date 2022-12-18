#pragma once
#include <iostream>
#include <cmath>
#include "../../Sdk/DataTypes/Structs/Vector2.h"

namespace ReplayEditor
{
	/// @brief An enum that specifies which easing algorithm should be used.
	enum EasingType : int
	{
		Normal,
		SliderHidden
	};

	/// @brief An enum that specifies the type of the transformation (animation).
	enum TransformationType : int
	{
		Empty,
		Fade,
		Scale,
		Move
	};

	/// @class Transformation
	/// @brief The transformation class that puts all the transformation (animation) data in a single place.
	class Transformation
	{
		/// @brief The current transformation (animation) type.
		TransformationType _transformationType;

		/// @brief The starting floating point value where the transformation (animation) will start.
		Vector2 _start;

		/// @brief The ending floating point value where the transformation (animation) will end.
		Vector2 _end;

		/// @brief The start time value where the transformation (animation) will start.
		int _startTime;

		/// @brief The end time value where the transformation (animation) will end.
		int _endTime;

		/// @brief The easing type (algorithm) that should be used in the transformation (animation).
		EasingType _easingType;
	public:
		/// @brief Empty constructor for first initialization.
		Transformation();

		/// @brief Constructor for constructing a transformation (animation) instance.
		/// @param transformationType The transformation (animation) type to use.
		/// @param startFloat The starting floating point where the transformation (animation) will start.
		/// @param endFloat  The ending floating point where the transformation (animation) will end.
		/// @param startTime The start time value where the transformation (animation) will start.
		/// @param endTime  The end time value where the transformation (animation) will end.
		/// @param easingType The easing type (algorithm) that should be used in the transformation (animation).
		Transformation(TransformationType transformationType, float startFloat, float endFloat, int startTime, int endTime, EasingType easingType = EasingType::Normal);

		/// @brief Constructor for constructing a transformation (animation) instance.
		/// @param transformationType The transformation (animation) type to use.
		/// @param startPosition The starting point where the transformation (animation) will start.
		/// @param endPosition  The ending point where the transformation (animation) will end.
		/// @param startTime The start time value where the transformation (animation) will start.
		/// @param endTime  The end time value where the transformation (animation) will end.
		/// @param easingType The easing type (algorithm) that should be used in the transformation (animation).
		Transformation(TransformationType transformationType, Vector2 startPosition, Vector2 endPosition, int startTime, int endTime, EasingType easingType = EasingType::Normal);

		/// @brief Returns the current transformation (animation) type being used.
		/// @returns Transformation (animation) type used.
		TransformationType GetTransformationType();

		/// @brief Returns the starting float of the transformation (animation).
		/// @returns Starting floating point value.
		float GetStartFloat();

		/// @brief Returns the ending float of the transformation (animation).
		/// @returns Ending floating point value.
		float GetEndFloat();

		/// @brief Returns the current transformation (animation) starting position.
		/// @return Starting position in Vector2
		Vector2 GetStartPosition();

		/// @brief Returns the current transformation (animation) ending position.
		/// @return Ending position in Vector2
		Vector2 GetEndPosition();

		/// @brief Returns the starting time of the transformation (animation).
		/// @returns Starting time.
		int GetStartTime();

		/// @brief Returns the ending time of the transformation (animation).
		/// @returns Ending time.
		int GetEndTime();

		/// @brief Returns the easing type (algorithm) being used by the transformation (animation).
		/// @returns Current easing type (algorithm).
		EasingType GetEasingType();

		/// @brief Sets the _endTime field to the endTime variable.
		/// @param endTime The ending time that'll overwrite the _endTime field.
		void SetEndTime(int endTime);

		/// @brief Sets the _easingType field to the easingType variable.
		/// @param easingType The easing type (algorithm) being used.
		void SetEasingType(EasingType easingType);
	};
}