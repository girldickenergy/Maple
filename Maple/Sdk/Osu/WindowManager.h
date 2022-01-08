#pragma once

#include <Explorer/TypeExplorer.h>

#include "../DataTypes/Structs/Vector2.h"

class WindowManager
{
	static inline void* instanceAddress = nullptr;
	static inline Field widthField;
	static inline Field heightField;
	static inline Field clientBoundsField;
public:
	static inline TypeExplorer RawWindowManager;

	static void Initialize();
	static void* Instance();
	static int Width();
	static int Height();
	static Vector2 ViewportPosition();
};