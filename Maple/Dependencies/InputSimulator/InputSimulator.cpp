#include "InputSimulator.h"

void InputSimulator::KeyDown(WORD keyCode)
{
    INPUT ip;
	
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = keyCode;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));
}

void InputSimulator::KeyUp(WORD keyCode)
{
    INPUT ip;

    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = keyCode;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

void InputSimulator::LeftMouseButtonDown()
{
    INPUT ip;

    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(1, &ip, sizeof(INPUT));
}

void InputSimulator::LeftMouseButtonUp()
{
    INPUT ip;

    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(1, &ip, sizeof(INPUT));
}

void InputSimulator::RightMouseButtonDown()
{
    INPUT ip;

    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    SendInput(1, &ip, sizeof(INPUT));
}

void InputSimulator::RightMouseButtonUp()
{
    INPUT ip;

    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    SendInput(1, &ip, sizeof(INPUT));
}
