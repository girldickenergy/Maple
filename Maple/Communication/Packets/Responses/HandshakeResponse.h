#pragma once

#include <vector>

class HandshakeResponse
{
	std::vector<unsigned char> key;
	std::vector<unsigned char> iv;

	HandshakeResponse(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
public:
	const std::vector<unsigned char>& GetKey();
	const std::vector<unsigned char>& GetIV();

	static HandshakeResponse Deserialize(const std::vector<unsigned char>& payload);
};
