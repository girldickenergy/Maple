#include "GLControl.h"

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Communication/Communication.h"

void GLControl::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("GLControl::UsesAngle"), xorstr_("89 4D DC FF 15 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 75 16 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C8 FF 15"), 0xB, 1);

		VIRTUALIZER_FISH_RED_END
}

bool GLControl::GetUsesAngle()
{
	const uintptr_t usesAngleAddress = Memory::Objects[xorstr_("GLControl::UsesAngle")];

	return usesAngleAddress ? *reinterpret_cast<bool*>(usesAngleAddress) : false;
}
