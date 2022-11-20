#pragma once

#include <vector>

class HandshakeRequest
{
public:
	HandshakeRequest() = default;
	std::vector<unsigned char> Serialize();
};
