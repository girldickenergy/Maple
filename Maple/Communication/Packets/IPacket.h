#pragma once

#include <cstdint>
#include "entt.hpp"
#include "Fnv1a.h"

class IPacket
{
public:
	virtual uint32_t GetIdentifier() = 0;
};
