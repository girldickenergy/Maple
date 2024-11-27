#pragma once

#include <vector>

#include "TaikoManiaStage.h"

class TaikoMania
{
	static inline std::vector<TaikoManiaStage> stages;

	static inline int duration;
public:
	static void Initialize();
	static void Render();
};
