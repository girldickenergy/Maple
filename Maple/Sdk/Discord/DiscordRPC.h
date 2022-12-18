#pragma once

#include "CLR/CLRString.h"

class DiscordRPC
{
	typedef void(__fastcall* fnUpdateStatus)(void* instance, int mode, int status, CLRString* beatmapDetails);
	static inline fnUpdateStatus oUpdateStatus;
	static void __fastcall updateStatusHook(void* instance, int mode, int status, CLRString* beatmapDetails);

	typedef void(__fastcall* fnUpdateMatch)(void* instance, void* match);
	static inline fnUpdateMatch oUpdateMatch;
	static void __fastcall updateMatchHook(void* instance, void* match);

	typedef void(__fastcall* fnSetLargeImageText)(void* instance, CLRString* string);
	static inline fnSetLargeImageText oSetLargeImageText;
	static void __fastcall setLargeImageTextHook(void* instance, CLRString* string);

	typedef void(__fastcall* fnSetState)(void* instance, CLRString* string);
	static inline fnSetState oSetState;
	static void __fastcall setStateHook(void* instance, CLRString* string);

	typedef void(__fastcall* fnSetDetails)(void* instance, CLRString* string);
	static inline fnSetDetails oSetDetails;
	static void __fastcall setDetailsHook(void* instance, CLRString* string);

	typedef void(__fastcall* fnSetSpectateSecret)(void* instance, CLRString* string);
	static inline fnSetSpectateSecret oSetSpectateSecret;
	static void __fastcall setSpectateSecretHook(void* instance, CLRString* string);

public:
	static void Initialize();
};
