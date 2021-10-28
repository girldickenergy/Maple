#pragma once

#include <Explorer/TypeExplorer.h>

#include "../DataTypes/Enums/PlayKeys.h"

class BindingManager
{
	typedef int(__fastcall* fnGetPlayKey)(PlayKeys key);
	static inline fnGetPlayKey getPlayKey;
public:
	static inline TypeExplorer RawBindingManager;
	static void Initialize();
	static int GetPlayKey(PlayKeys key);
};
