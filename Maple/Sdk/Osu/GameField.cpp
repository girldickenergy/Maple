#include "GameField.h"

#include "GameBase.h"

void GameField::Initialize()
{
	RawGameField = Vanilla::Explorer["osu.GameField"];

	gameFieldInstanceAddress = GameBase::RawGameBase["GameField"].Field.GetAddress();
	
	heightField = RawGameField["<Height>k__BackingField"].Field;
	offsetField = RawGameField["<OffsetVector1>k__BackingField"].Field;
}

void* GameField::Instance()
{
	return *static_cast<void**>(gameFieldInstanceAddress);
}

float GameField::GetHeight()
{
	return *static_cast<float*>(heightField.GetAddress(Instance()));
}

float GameField::GetRatio()
{
	return GetHeight() / 384.f;
}

Vector2 GameField::GetOffsetVector()
{
	return *static_cast<Vector2*>(offsetField.GetAddress(Instance()));
}

Vector2 GameField::DisplayToField(Vector2 pos)
{
	return (pos - GetOffsetVector()) / GetRatio();
}

Vector2 GameField::FieldToDisplay(Vector2 pos)
{
	return pos * GetRatio() + GetOffsetVector();
}
