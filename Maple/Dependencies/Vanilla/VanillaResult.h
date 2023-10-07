#pragma once

enum class VanillaResult : unsigned int
{
    Success = 0x0,
    JITFailure = 0x1,
    RelocateFailure = 0x2,
    CLRStringFailure = 0x3,

	HookAlreadyInstalled = 0x10,
    HookNotInstalled = 0x11
};