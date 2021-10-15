#pragma once

enum class CinnamonResult
{
	Success = 0x0,
	HookAlreadyInstalled = 0x1,
	HookNotFound = 0x2,
	UnknownHookType = 0x3,
	CreateToolhelp32SnapshotFailed = 0x4,
	GetThreadContextFailed = 0x5,
	NoFreeIndex = 0x6,
	SetThreadContextFailed = 0x7,
	BaseThreadInitThunkFailed = 0x8
};