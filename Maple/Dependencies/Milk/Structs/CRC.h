#pragma once

struct CRC
{
	void* functionPointer;
	int functionSize;
	int unknown;
	char className[256];
	char functionName[256];
	void* nextEntry;
};