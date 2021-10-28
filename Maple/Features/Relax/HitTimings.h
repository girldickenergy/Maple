#pragma once

struct HitTimings
{
	int Offset;
	int HoldTime;

	HitTimings() = default;
	
	HitTimings(int offset, int holdTime)
	{
		Offset = offset;
		HoldTime = holdTime;
	}
};