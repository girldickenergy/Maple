#include "GLControl.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Utilities/Security/xorstr.hpp"

void GLControl::Initialize()
{
	STR_ENCRYPT_START

	Memory::AddObject(xor ("GLControl::UsesAngle"), xor ("89 4D DC FF 15 ?? ?? ?? ?? 80 3D"), 0xB, 1);

	STR_ENCRYPT_END
}

bool GLControl::GetUsesAngle()
{
	const uintptr_t usesAngleAddress = Memory::Objects["GLControl::UsesAngle"];

	return usesAngleAddress ? *reinterpret_cast<bool*>(usesAngleAddress) : false;
}
