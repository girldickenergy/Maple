#pragma once

class Timewarp
{
	static inline double tickrate = 1000.0 / 60.0;
public:
	static void Initialize();
	static void TimewarpThread();
};