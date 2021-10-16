#pragma once

#include <Explorer/TypeExplorer.h>

class GameBase
{
	static inline void* instanceAddress = nullptr;
public:
	static inline TypeExplorer RawGameBase;
	
	static void Initialize();
	static void* Instance();
};