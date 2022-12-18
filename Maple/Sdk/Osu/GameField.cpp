#include "GameField.h"

#include "ThemidaSDK.h"

#include "../Memory.h"
#include "../../Utilities/Security/xorstr.hpp"
#include "../../Communication/Communication.h"

void GameField::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("GameField::Instance"), xorstr_("8B 15 ?? ?? ?? ?? 83 C2 04 8B 0D ?? ?? ?? ?? 39 09 FF 15 ?? ?? ?? ?? A1"), 0xB, 1);

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

uintptr_t GameField::GetInstance()
{
	const uintptr_t instanceAddress = Memory::Objects[xorstr_("GameField::Instance")];

	return instanceAddress ? *reinterpret_cast<uintptr_t*>(instanceAddress) : 0u;
}

float GameField::GetWidth()
{
	const uintptr_t instance = GetInstance();

	return instance ? *reinterpret_cast<float*>(instance + WIDTH_OFFSET) : 0.0f;
}

float GameField::GetHeight()
{
	const uintptr_t instance = GetInstance();

	return instance ? *reinterpret_cast<float*>(instance + HEIGHT_OFFSET) : 0.0f;
}

float GameField::GetRatio()
{
	return GetHeight() / 384.f;
}

Vector2 GameField::GetOffset()
{
	const uintptr_t instance = GetInstance();

	return instance ? *reinterpret_cast<Vector2*>(instance + OFFSETVECTOR_OFFSET) : Vector2(0, 0);
}

Vector2 GameField::DisplayToField(Vector2 display)
{
	return (display - GetOffset()) / GetRatio(); //todo: possible division by zero
}

Vector2 GameField::FieldToDisplay(Vector2 field)
{
	return field * GetRatio() + GetOffset();
}
