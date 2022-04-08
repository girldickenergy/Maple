#include "InputManager.h"

#include <Vanilla.h>
#include <Enums/Gameplay/OsuKeys.h>

#include "../Osu/GameField.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../Player/Player.h"
#include "../../Config/Config.h"
#include "../../Features/AimAssist/AimAssist.h"

void InputManager::Initialize()
{
	RawInputManager = Vanilla::Explorer["osu.Input.InputManager"];
	RawMouseManager = Vanilla::Explorer["osu.Input.Handlers.MouseManager"];
	
	RawMouseManager["get_MousePosition"].Method.Compile();
	getMousePosition = static_cast<fnGetMousePosition>(RawMouseManager["get_MousePosition"].Method.GetNativeStart());

	leftButton1Address = RawInputManager["leftButton1"].Field.GetAddress();
	leftButton1iAddress = RawInputManager["leftButton1i"].Field.GetAddress();
	leftButton2Address = RawInputManager["leftButton2"].Field.GetAddress();
	leftButton2iAddress = RawInputManager["leftButton2i"].Field.GetAddress();
	rightButton1Address = RawInputManager["rightButton1"].Field.GetAddress();
	rightButton1iAddress = RawInputManager["rightButton1i"].Field.GetAddress();
	rightButton2Address = RawInputManager["rightButton2"].Field.GetAddress();
	rightButton2iAddress = RawInputManager["rightButton2i"].Field.GetAddress();
}

Vector2 InputManager::CursorPosition()
{
	const Vector2 pos = getMousePosition();
	
	return GameField::DisplayToField(pos);
}

void __stdcall InputManager::SetMousePositionHook(float x, float y)
{
	const Vector2 assistedPosition = Config::AimAssist::Algorithm == 0 ? AimAssist::DoAssist(Vector2(x, y)) : Config::AimAssist::Algorithm == 1 ? AimAssist::DoAssistv2(Vector2(x, y)) : AimAssist::DoAssistv3(Vector2(x, y));

	if (Player::IsLoaded() && !Player::IsPaused() && ReplayBot::Ready)
	{
		const Vector2 pos = ReplayBot::Update();

		if (ReplayBot::DisableAiming)
			oSetMousePosition(assistedPosition.X, assistedPosition.Y);
		else
			oSetMousePosition(pos.X, pos.Y);
	}
	else
		oSetMousePosition(assistedPosition.X, assistedPosition.Y);
}

void __fastcall InputManager::MouseViaKeyboardControlsHook()
{
	if (Player::IsLoaded() && !Player::IsPaused() && ReplayBot::Ready && !ReplayBot::DisableTapping)
	{
		OsuKeys keys = ReplayBot::GetCurrentKeys();

		bool m1Pressed = (keys & OsuKeys::M1) > OsuKeys::None;
		bool k1Pressed = (keys & OsuKeys::K1) > OsuKeys::None;
		bool m2Pressed = (keys & OsuKeys::M2) > OsuKeys::None;
		bool k2Pressed = (keys & OsuKeys::K2) > OsuKeys::None;

		*(bool*)leftButton1iAddress = (!*(bool*)leftButton1Address && (m1Pressed || k1Pressed));
		*(bool*)leftButton1Address = m1Pressed || k1Pressed;

		*(bool*)leftButton2iAddress = (!*(bool*)leftButton2Address && k1Pressed);
		*(bool*)leftButton2Address = k1Pressed;

		*(bool*)rightButton1iAddress = (!*(bool*)rightButton1Address && (m2Pressed || k2Pressed));
		*(bool*)rightButton1Address = m2Pressed || k2Pressed;

		*(bool*)rightButton2iAddress = (!*(bool*)rightButton2Address && k2Pressed);
		*(bool*)rightButton2Address = k2Pressed;
	}

	oMouseViaKeyboardControls();
}
