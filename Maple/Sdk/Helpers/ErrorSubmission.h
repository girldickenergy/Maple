#pragma once

#include <cstdint>

class ErrorSubmission
{
	typedef void(__fastcall* fnSubmitError)(uintptr_t err);
	static inline fnSubmitError oSubmitError;
	static void __fastcall submitErrorHook(uintptr_t err);
public:
	static void Initialize();
};
