#pragma once
#include <string>

class ClipboardUtilities
{
public:
	static std::string Read();
	static void Write(const std::string& data);
};
