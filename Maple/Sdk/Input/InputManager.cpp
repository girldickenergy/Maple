#include "InputManager.h"

#include <Vanilla.h>

#include "../Osu/GameField.h"

void InputManager::Initialize()
{
	RawMouseManager = Vanilla::Explorer["osu.Input.Handlers.MouseManager"];
	RawInputManager = Vanilla::Explorer["osu.Input.InputManager"];

	currentCursorHandlerAddress = RawInputManager["CurrentCursorHandler"].Field.GetAddress();

	RawMouseManager["get_MousePosition"].Method.Compile();
	getMousePosition = static_cast<fnGetMousePosition>(RawMouseManager["get_MousePosition"].Method.GetNativeStart());

	RawMouseManager["ApplyHandler"].Method.Compile();
	applyHandler = static_cast<fnApplyHandler>(RawMouseManager["ApplyHandler"].Method.GetNativeStart());
}

void* InputManager::GetCurrentInputHandlerInstance()
{
	return *static_cast<void**>(currentCursorHandlerAddress);
}

Vector2 InputManager::CursorPosition()
{
	Vector2 pos = getMousePosition();
	
	return GameField::DisplayToField(pos);
}

Vector2 InputManager::CursorPositionAccurate()
{
	applyHandler(GetCurrentInputHandlerInstance());
	
	return CursorPosition();
}