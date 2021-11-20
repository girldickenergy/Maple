#include "InputManager.h"

#include <Vanilla.h>

#include "../Osu/GameField.h"

void InputManager::Initialize()
{
	RawInputManager = Vanilla::Explorer["osu.Input.InputManager"];
	RawMouseManager = Vanilla::Explorer["osu.Input.Handlers.MouseManager"];
	
	RawMouseManager["get_MousePosition"].Method.Compile();
	getMousePosition = static_cast<fnGetMousePosition>(RawMouseManager["get_MousePosition"].Method.GetNativeStart());
}

Vector2 InputManager::CursorPosition()
{
	const Vector2 pos = getMousePosition();
	
	return GameField::DisplayToField(pos);
}