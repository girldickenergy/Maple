#pragma once
#include "../Chiyo/Replays/Replay.h"

/// @class Click
/// @brief An additional abstraction over the ReplayFrames to only hold information about the clicks in the replay for easier handling.
class Click
{
public:
	/// @brief The original start time of the click.
	int OriginalStartTime;

	/// @brief Start time of the click.
	int StartTime;

	/// @brief The original duration of the click.
	int OriginalDuration;

	/// @brief The duration of the click.
	int Duration;

	/// @brief Position of the click in osu!px.
	Vector2 _position;

	/// @brief The OsuKeys utilized in the click.
	OsuKeys _keys;

	/// @brief Boolean if click has been previously used in analyzation.
	bool _usedInAnalyzation;

	/// @brief Boolean if the click has been edited on the ClickTimeline.
	bool _edited;

	/// @brief Empty constructor for first initialization.
	Click()
	{
		StartTime = 0;
		_position = Vector2(0, 0);
		Duration = 0;
		_keys = OsuKeys::None;
		_usedInAnalyzation = false;
		_edited = false;
	}

	/// @brief Constructs a new click instance.
	/// @param startTime Start time of the click.
	/// @param position Position of the click.
	/// @param duration Duration of the click.
	/// @param keys OsuKeys utilized in the click.
	Click(int startTime, Vector2 position, int duration, OsuKeys keys)
	{
		OriginalStartTime = startTime;
		StartTime = startTime;
		_position = position;
		OriginalDuration = duration;
		Duration = duration;
		_keys = keys;
		_usedInAnalyzation = false;
		_edited = false;
	}
};