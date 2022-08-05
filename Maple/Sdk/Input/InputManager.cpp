#include "InputManager.h"

#define NOMINMAX

#include "../Memory.h"
#include "../Player/Player.h"
#include "../Audio/AudioEngine.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Config/Config.h"

void __fastcall InputManager::setMousePositionHook(Vector2 pos)
{
	Vector2 newPosition = pos;

	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused()/*&& !Player::GetIsFailed()*/)
	{
		if (ReplayBot::Ready)
			ReplayBot::Update();

		if (ReplayBot::Ready && !ReplayBot::DisableAiming)
		{
			newPosition = ReplayBot::GetCursorPosition();

			AccumulatedOffset = newPosition - pos;
		}
	}
	else
	{
		if (!LastPosition.IsNull())
			AccumulatedOffset = Resync(LastPosition - pos, AccumulatedOffset, .5f);

		newPosition = pos + AccumulatedOffset;
	}

	cursorPosition = newPosition;
	LastPosition = pos;
	
	oSetMousePosition(newPosition);
}

void __fastcall InputManager::mouseViaKeyboardControlsHook()
{
	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused()/* && !Player::IsFailed()*/ && (Config::Relax::Enabled || (ReplayBot::Ready && !ReplayBot::DisableTapping)))
	{
		OsuKeys keys = ReplayBot::Ready && !ReplayBot::DisableTapping ? ReplayBot::GetCurrentKeys() : OsuKeys::None;//Relax::Update();

		bool m1Pressed = (keys & OsuKeys::M1) > OsuKeys::None;
		bool k1Pressed = (keys & OsuKeys::K1) > OsuKeys::None;
		bool m2Pressed = (keys & OsuKeys::M2) > OsuKeys::None;
		bool k2Pressed = (keys & OsuKeys::K2) > OsuKeys::None;

		*(bool*)Memory::Objects["InputManager::leftButton1i"] = (!*(bool*)Memory::Objects["InputManager::leftButton1"] && (m1Pressed || k1Pressed));
		*(bool*)Memory::Objects["InputManager::leftButton1"] = m1Pressed || k1Pressed;

		*(bool*)Memory::Objects["InputManager::leftButton2i"] = (!*(bool*)Memory::Objects["InputManager::leftButton2"] && k1Pressed);
		*(bool*)Memory::Objects["InputManager::leftButton2"] = k1Pressed;

		*(bool*)Memory::Objects["InputManager::rightButton1i"] = (!*(bool*)Memory::Objects["InputManager::rightButton1"] && (m2Pressed || k2Pressed));
		*(bool*)Memory::Objects["InputManager::rightButton1"] = m2Pressed || k2Pressed;

		*(bool*)Memory::Objects["InputManager::rightButton2i"] = (!*(bool*)Memory::Objects["InputManager::rightButton2"] && k2Pressed);
		*(bool*)Memory::Objects["InputManager::rightButton2"] = k2Pressed;

		if (*(bool*)Memory::Objects["InputManager::leftButton2i"] || *(bool*)Memory::Objects["InputManager::leftButton1i"])
			*(int*)Memory::Objects["InputManager::leftButton"] = 1;

		if (*(bool*)Memory::Objects["InputManager::rightButton2i"] || *(bool*)Memory::Objects["InputManager::rightButton1i"])
			*(int*)Memory::Objects["InputManager::rightButton"] = 1;
	}

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
