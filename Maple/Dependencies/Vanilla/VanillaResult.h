#pragma once

enum class VanillaResult : unsigned int
{
    Success = 0x0,

    JITFailure = 0x1,
    RelocateFailure = 0x2,
    Entry2MethodDescFailure = 0x3,
    GetAddrOfSlotFailure = 0x4,
    CLRStringFailure = 0x5,

    HookAlreadyInstalled = 0x10,
    HookNotInstalled = 0x11,
    HookTypeUnknown = 0x12
};