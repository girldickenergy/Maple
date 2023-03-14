#include "ErrorSubmission.h"

#include "ThemidaSDK.h"
#include "xorstr.hpp"

#include "../Memory.h"
#include "../../Communication/Communication.h"

void __fastcall ErrorSubmission::submitErrorHook(uintptr_t err)
{
	return;
}

void ErrorSubmission::Initialize()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	Memory::AddObject(xorstr_("ErrorSubmission::SubmitError"), xorstr_("55 8B EC 57 56 83 EC 40 8B F1 8D 7D C0 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D C8 83 3D"));
	Memory::AddHook(xorstr_("ErrorSubmission::SubmitError"), xorstr_("ErrorSubmission::SubmitError"), reinterpret_cast<uintptr_t>(submitErrorHook), reinterpret_cast<uintptr_t*>(&oSubmitError));

	STR_ENCRYPT_END
	VM_FISH_RED_END
}
