#include "GameField.h"

#include "GameBase.h"

void GameField::Initialize()
{
	RawGameField = Vanilla::Explorer["osu.GameField"];

	gameFieldField = GameBase::RawGameBase["GameField"].Field;

	RawGameField["DisplayToFieldX"].Method.Compile();
	displayToFieldX = static_cast<fnDisplayToFieldX>(RawGameField["DisplayToFieldX"].Method.GetNativeStart());

	RawGameField["DisplayToFieldY"].Method.Compile();
	displayToFieldY = static_cast<fnDisplayToFieldY>(RawGameField["DisplayToFieldY"].Method.GetNativeStart());

	RawGameField["FieldToDisplayX"].Method.Compile();
	fieldToDisplayX = static_cast<fnFieldToDisplayX>(RawGameField["FieldToDisplayX"].Method.GetNativeStart());

	RawGameField["FieldToDisplayY"].Method.Compile();
	fieldToDisplayY = static_cast<fnFieldToDisplayX>(RawGameField["FieldToDisplayY"].Method.GetNativeStart());
}

void* GameField::Instance()
{
	return *static_cast<void**>(gameFieldField.GetAddress(GameBase::Instance()));
}

Vector2 GameField::DisplayToField(Vector2 pos)
{
	float x = displayToFieldX(Instance(), pos.X);
	float y = displayToFieldY(Instance(), pos.Y);

	return Vector2(x, y);
}

Vector2 GameField::FieldToDisplay(Vector2 pos)
{
	float x = fieldToDisplayX(Instance(), pos.X);
	float y = fieldToDisplayY(Instance(), pos.Y);

	return Vector2(x, y);
}
