#pragma once

enum class VanillaResult : unsigned int
{
	Success = 0x0,
	JITFailure = 0x1,
	RelocateFailure = 0x2,
	CLRStringFailure = 0x3,

	HookAlreadyInstalled = 0x1000,
	UnknownHookType = 0x1001,
	GetThreadContextFailed = 0x1002,
	NoFreeIndex = 0x1003,
	SetThreadContextFailed = 0x1004,
	BaseThreadInitThunkFailed = 0x1005,
	CreateToolhelp32SnapshotFailed = 0x1006,
	HookNotInstalled = 0x1007,
	
	PatchAlreadyInstalled = 0x2000,
	PatchPatternNotFound = 0x2001,
	PatchNotInstalled = 0x2002
};