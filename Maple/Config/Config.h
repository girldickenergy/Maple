#pragma once

class Config
{
public:
	struct Timewarp
	{
		static inline bool Enabled = false;
		static inline int Rate = 100;
	};
};