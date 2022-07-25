#include "GLControl.h"

#include "../Memory.h"

void GLControl::Initialize()
{
	Memory::AddObject("GLControl::UsesAngle", "89 4D DC FF 15 ?? ?? ?? ?? 80 3D", 0xB, 1);
}

bool GLControl::GetUsesAngle()
{
	const uintptr_t usesAngleAddress = Memory::Objects["GLControl::UsesAngle"];

	return usesAngleAddress ? *reinterpret_cast<bool*>(usesAngleAddress) : false;
}
