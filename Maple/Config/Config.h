#pragma once

class Config
{
public:
	struct Timewarp
	{
		static inline bool Enabled = false;
		static inline int Rate = 100;
	};

	struct Visuals
	{
		static inline bool ARChangerEnabled = false;
		static inline bool ARChangerAdjustToMods = false;
		static inline float AR = 9.2f;
		static inline bool HiddenDisabled = false;
		static inline bool FlashlightDisabled = false;
	};

	struct Misc
	{
		static inline bool DisableSpectators = false;
	};
};