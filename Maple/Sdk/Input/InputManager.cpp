#include "InputManager.h"

#define NOMINMAX

#include "../Memory.h"
#include "../Player/Player.h"
#include "../Audio/AudioEngine.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Config/Config.h"
#include "../../Features/AimAssist/AimAssist.h"

void __fastcall InputManager::setMousePositionHook(Vector2 pos)
{
	Vector2 newPosition = pos;

	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused() && !Player::GetIsFailed())
	{
		if (ReplayBot::Ready)
			ReplayBot::Update();

		if (ReplayBot::Ready && !ReplayBot::DisableAiming)
		{
			newPosition = ReplayBot::GetCursorPosition();

			accumulatedOffset = newPosition - pos;
		}
		else if (Config::AimAssist::Enabled)
			newPosition = AimAssist::GetCursorPosition(pos);
	}
	else
	{
		if (!lastCursorPosition.IsNull())
			accumulatedOffset = Resync(lastCursorPosition - pos, accumulatedOffset, .5f);

		newPosition = pos + accumulatedOffset;
	}

	cursorPosition = newPosition;
	lastCursorPosition = pos;
	
	oSetMousePosition(newPosition);
}

void __fastcall InputManager::mouseViaKeyboardControlsHook()
{
	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused() && !Player::GetIsFailed() && (Config::Relax::Enabled || (ReplayBot::Ready && !ReplayBot::DisableTapping)))
	{
		const OsuKeys keys = ReplayBot::Ready && !ReplayBot::DisableTapping ? ReplayBot::GetCurrentKeys() : OsuKeys::None;//Relax::Update();

		const bool m1Pressed = (keys & OsuKeys::M1) > OsuKeys::None;
		const bool k1Pressed = (keys & OsuKeys::K1) > OsuKeys::None;
		const bool m2Pressed = (keys & OsuKeys::M2) > OsuKeys::None;
		const bool k2Pressed = (keys & OsuKeys::K2) > OsuKeys::None;

		const uintptr_t leftButton1Address = Memory::Objects["InputManager::leftButton1"];
		const uintptr_t leftButton1iAddress = Memory::Objects["InputManager::leftButton1i"];
		const uintptr_t leftButton2Address = Memory::Objects["InputManager::leftButton2"];
		const uintptr_t leftButton2iAddress = Memory::Objects["InputManager::leftButton2i"];
		const uintptr_t rightButton1Address = Memory::Objects["InputManager::rightButton1"];
		const uintptr_t rightButton1iAddress = Memory::Objects["InputManager::rightButton1i"];
		const uintptr_t rightButton2Address = Memory::Objects["InputManager::rightButton2"];
		const uintptr_t rightButton2iAddress = Memory::Objects["InputManager::rightButton2i"];
		const uintptr_t leftButtonAddress = Memory::Objects["InputManager::leftButton"];
		const uintptr_t rightButtonAddress = Memory::Objects["InputManager::rightButton"];

		if (leftButton1Address && leftButton1iAddress && leftButton2Address && leftButton2iAddress && rightButton1Address && rightButton1iAddress && rightButton2Address && rightButton2iAddress && leftButtonAddress && rightButtonAddress)
		{
			*reinterpret_cast<bool*>(leftButton1iAddress) = (!*reinterpret_cast<bool*>(leftButton1Address) && (m1Pressed || k1Pressed));
			*reinterpret_cast<bool*>(leftButton1Address) = m1Pressed || k1Pressed;

			*reinterpret_cast<bool*>(leftButton2iAddress) = (!*reinterpret_cast<bool*>(leftButton2Address) && k1Pressed);
			*reinterpret_cast<bool*>(leftButton2Address) = k1Pressed;

			*reinterpret_cast<bool*>(rightButton1iAddress) = (!*reinterpret_cast<bool*>(rightButton1Address) && (m2Pressed || k2Pressed));
			*reinterpret_cast<bool*>(rightButton1Address) = m2Pressed || k2Pressed;

			*reinterpret_cast<bool*>(rightButton2iAddress) = (!*reinterpret_cast<bool*>(rightButton2Address) && k2Pressed);
			*reinterpret_cast<bool*>(rightButton2Address) = k2Pressed;

			if (*reinterpret_cast<bool*>(leftButton2iAddress) || *reinterpret_cast<bool*>(leftButton1iAddress))
				*reinterpret_cast<int*>(leftButtonAddress) = 1;

			if (*reinterpret_cast<bool*>(rightButton2iAddress) || *reinterpret_cast<bool*>(rightButton1iAddress))
				*reinterpret_cast<int*>(rightButtonAddress) = 1;
		}
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

Vector2 InputManager::GetLastCursorPosition()
{
	return lastCursorPosition;
}

Vector2 InputManager::GetAccumulatedOffset()
{
	return accumulatedOffset;
}

void InputManager::SetAccumulatedOffset(Vector2 value)
{
	accumulatedOffset = value;
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
