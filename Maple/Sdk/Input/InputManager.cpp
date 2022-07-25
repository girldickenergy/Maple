#include "InputManager.h"

#include "../Memory.h"

void __fastcall InputManager::setMousePositionHook(Vector2 pos)
{
	cursorPosition = pos;
	
	oSetMousePosition(pos);
}

void __fastcall InputManager::mouseViaKeyboardControlsHook()
{
	oMouseViaKeyboardControls();
}

void InputManager::Initialize()
{
	Memory::AddObject("MouseManager::SetMousePosition", "55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04");
	Memory::AddObject("InputManager::MouseViaKeyboardControls", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02");

	Memory::AddObject("InputManager::leftButton1", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", LEFTBUTTON1_OFFSET, 1);
	Memory::AddObject("InputManager::leftButton1i", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", LEFTBUTTON1I_OFFSET, 1);
	Memory::AddObject("InputManager::leftButton2", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", LEFTBUTTON2_OFFSET, 1);
	Memory::AddObject("InputManager::leftButton2i", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", LEFTBUTTON2I_OFFSET, 1);
	Memory::AddObject("InputManager::rightButton1", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", RIGHTBUTTON1_OFFSET, 1);
	Memory::AddObject("InputManager::rightButton1i", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", RIGHTBUTTON1I_OFFSET, 1);
	Memory::AddObject("InputManager::rightButton2", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", RIGHTBUTTON2_OFFSET, 1);
	Memory::AddObject("InputManager::rightButton2i", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", RIGHTBUTTON2I_OFFSET, 1);
	Memory::AddObject("InputManager::leftButton", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", LEFTBUTTON_OFFSET, 1);
	Memory::AddObject("InputManager::rightButton", "55 8B EC 57 56 83 3D ?? ?? ?? ?? 02", RIGHTBUTTON_OFFSET, 1);

	Memory::AddHook("MouseManager::SetMousePosition", "MouseManager::SetMousePosition", reinterpret_cast<uintptr_t>(setMousePositionHook), reinterpret_cast<uintptr_t*>(&oSetMousePosition), VanillaHookType::UndetectedInline);
	Memory::AddHook("InputManager::MouseViaKeyboardControls", "InputManager::MouseViaKeyboardControls", reinterpret_cast<uintptr_t>(mouseViaKeyboardControlsHook), reinterpret_cast<uintptr_t*>(&oMouseViaKeyboardControls));
}

Vector2 InputManager::GetCursorPosition()
{
	return cursorPosition;
}

Vector2 InputManager::Resync(Vector2 displacement, Vector2 offset, float resyncFactor)
{
	if (offset.Length() <= std::numeric_limits<float>::epsilon())
		return offset;

	const auto dpi = displacement * resyncFactor;

	offset.X = offset.X > 0.f ? std::max(0.f, dpi.X >= 0.f ? offset.X - dpi.X : offset.X + dpi.X)
		: std::min(0.f, dpi.X <= 0.f ? offset.X - dpi.X : offset.X + dpi.X);

	offset.Y = offset.Y > 0.f ? std::max(0.f, dpi.Y >= 0.f ? offset.Y - dpi.Y : offset.Y + dpi.Y)
		: std::min(0.f, dpi.Y <= 0.f ? offset.Y - dpi.Y : offset.Y + dpi.Y);

	return offset;
}
