#include "InputManager.h"

#define NOMINMAX

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../Player/Player.h"
#include "../Audio/AudioEngine.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Configuration/ConfigManager.h"
#include "../../Features/AimAssist/AimAssist.h"
#include "../../Features/Relax/Relax.h"
#include "../../Communication/Communication.h"

inline float roundoff(float val)
{
	return  static_cast<int>(val + 0.5 - (val < 0));
}

inline Vector2 roundVec(Vector2 val)
{
	return Vector2(roundoff(val.X), roundoff(val.Y));
}

void __fastcall InputManager::setMousePositionHook(Vector2 pos)
{
	Vector2 newPosition = pos;

	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused() && !Player::GetIsFailed())
	{
		if (ReplayBot::Ready)
			ReplayBot::Update();

		if (ReplayBot::Ready && !ReplayBot::DisableAiming)
		{
			newPosition = roundVec(ReplayBot::GetCursorPosition());

			accumulatedOffset = newPosition - pos;
		}
		else if (ConfigManager::CurrentConfig.AimAssist.Enabled)
			newPosition = roundVec(AimAssist::GetCursorPosition(pos));
	}
	else
	{
		if (!lastCursorPosition.IsNull())
			accumulatedOffset = Resync(lastCursorPosition - pos, accumulatedOffset, .5f);

		newPosition = roundVec(pos + accumulatedOffset);
	}

	cursorPosition = newPosition;
	lastCursorPosition = pos;
	
	[[clang::musttail]] return oSetMousePosition(newPosition);
}

void __fastcall InputManager::mouseViaKeyboardControlsHook()
{
	if (Player::GetIsLoaded() && !AudioEngine::GetIsPaused() && !Player::GetIsFailed() && (ConfigManager::CurrentConfig.Relax.Enabled || (ReplayBot::Ready && !ReplayBot::DisableTapping)))
	{
		const OsuKeys keys = ReplayBot::Ready && !ReplayBot::DisableTapping ? ReplayBot::GetCurrentKeys() : Relax::Update();

		SetKeyStates(keys);
	}

	[[clang::musttail]] return oMouseViaKeyboardControls();
}

void InputManager::Initialize()
{
	VIRTUALIZER_FISH_RED_START
	
	Memory::AddObject(xorstr_("MouseManager::SetMousePosition"), xorstr_("55 8B EC 83 EC 14 A1 ?? ?? ?? ?? 83 C0 04 D9 45 08 D9 18 D9 45 0C D9 58 04 A1 ?? ?? ?? ?? 83 C0 04 D9 00 D9 5D FC"));
	Memory::AddObject(xorstr_("InputManager::MouseViaKeyboardControls"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02 74 04 5E 5F 5D C3 33 C9 FF 15 ?? ?? ?? ?? 8B F0 85 F6 0F 84"));

	Memory::AddObject(xorstr_("InputManager::leftButton1"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), LEFTBUTTON1_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::leftButton1i"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), LEFTBUTTON1I_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::leftButton2"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), LEFTBUTTON2_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::leftButton2i"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), LEFTBUTTON2I_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::rightButton1"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), RIGHTBUTTON1_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::rightButton1i"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), RIGHTBUTTON1I_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::rightButton2"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), RIGHTBUTTON2_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::rightButton2i"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), RIGHTBUTTON2I_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::leftButton"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), LEFTBUTTON_OFFSET, 1);
	Memory::AddObject(xorstr_("InputManager::rightButton"), xorstr_("55 8B EC 57 56 83 3D ?? ?? ?? ?? 02"), RIGHTBUTTON_OFFSET, 1);

	Memory::AddHook(xorstr_("MouseManager::SetMousePosition"), xorstr_("MouseManager::SetMousePosition"), reinterpret_cast<uintptr_t>(setMousePositionHook), reinterpret_cast<uintptr_t*>(&oSetMousePosition));
	Memory::AddHook(xorstr_("InputManager::MouseViaKeyboardControls"), xorstr_("InputManager::MouseViaKeyboardControls"), reinterpret_cast<uintptr_t>(mouseViaKeyboardControlsHook), reinterpret_cast<uintptr_t*>(&oMouseViaKeyboardControls));

	Memory::AddObject(xorstr_("InputManager::ScorableFrame"), xorstr_("0F B6 05 ?? ?? ?? ?? A2 ?? ?? ?? ?? 80 3D ?? ?? ?? ?? 00 75 34"), 0x8, 0x1);

	VIRTUALIZER_FISH_RED_END
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
	accumulatedOffset = (Communication::IntegritySignature1 != 0xdeadbeef || Communication::IntegritySignature2 != 0xefbeadde || Communication::IntegritySignature3 != 0xbeefdead) ? (Vector2(-value.X, -value.Y)) : value;
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

void InputManager::SetKeyStates(OsuKeys keys)
{
	const bool m1Pressed = (keys & OsuKeys::M1) > OsuKeys::None;
	const bool k1Pressed = (keys & OsuKeys::K1) > OsuKeys::None;
	const bool m2Pressed = (keys & OsuKeys::M2) > OsuKeys::None;
	const bool k2Pressed = (keys & OsuKeys::K2) > OsuKeys::None;

	const uintptr_t leftButton1Address = Memory::Objects[xorstr_("InputManager::leftButton1")];
	const uintptr_t leftButton1iAddress = Memory::Objects[xorstr_("InputManager::leftButton1i")];
	const uintptr_t leftButton2Address = Memory::Objects[xorstr_("InputManager::leftButton2")];
	const uintptr_t leftButton2iAddress = Memory::Objects[xorstr_("InputManager::leftButton2i")];
	const uintptr_t rightButton1Address = Memory::Objects[xorstr_("InputManager::rightButton1")];
	const uintptr_t rightButton1iAddress = Memory::Objects[xorstr_("InputManager::rightButton1i")];
	const uintptr_t rightButton2Address = Memory::Objects[xorstr_("InputManager::rightButton2")];
	const uintptr_t rightButton2iAddress = Memory::Objects[xorstr_("InputManager::rightButton2i")];
	const uintptr_t leftButtonAddress = Memory::Objects[xorstr_("InputManager::leftButton")];
	const uintptr_t rightButtonAddress = Memory::Objects[xorstr_("InputManager::rightButton")];

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

bool InputManager::GetScorableFrame()
{
	const uintptr_t scorableFrameAddress = Memory::Objects[xorstr_("InputManager::ScorableFrame")];

	return scorableFrameAddress ? *reinterpret_cast<bool*>(scorableFrameAddress) : false;
}
