#include "GLControl.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Communication/Communication.h"

void GLControl::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("GLControl::UsesAngle"), xorstr_("89 4D DC FF 15 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 75 16 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C8 FF 15"), 0xB, 1);

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

bool GLControl::GetUsesAngle()
{
	const uintptr_t usesAngleAddress = Memory::Objects[xorstr_("GLControl::UsesAngle")];

	return usesAngleAddress ? *reinterpret_cast<bool*>(usesAngleAddress) : false;
}
