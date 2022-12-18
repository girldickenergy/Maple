#pragma once

class BanchoClient
{
	typedef void(__fastcall* fnInitializePrivate)();
public:
	static void Initialize();

	static void InitializePrivate();
};