#include "ErrorSubmission.h"

#include "../Memory.h"

void __fastcall ErrorSubmission::submitErrorHook(uintptr_t err)
{
	return;
}

void ErrorSubmission::Initialize()
{
	Memory::AddObject("ErrorSubmission::SubmitError", "55 8B EC 57 56 83 EC 40 8B F1 8D 7D C0 B9 ?? ?? ?? ?? 33 C0 F3 AB 8B CE 89 4D C8 83 3D");
	Memory::AddHook("ErrorSubmission::SubmitError", "ErrorSubmission::SubmitError", reinterpret_cast<uintptr_t>(submitErrorHook), reinterpret_cast<uintptr_t*>(&oSubmitError));
}
