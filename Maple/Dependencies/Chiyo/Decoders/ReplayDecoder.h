#pragma once

#include <string>

#include "../Replays/Replay.h"

class ReplayDecoder
{
public:
	static Replay Decode(std::string filepath);
	static Replay Decode(std::istream* filestream);
};