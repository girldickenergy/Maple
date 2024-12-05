#pragma once
#include <array>
#include <vector>
#include <span>

class Mha256
{
	std::array<uint8_t, 32> m_State;
	std::array<uint8_t, 32> m_InitialState;

	size_t m_BlockSize;
	int m_Rounds;

	void InitializeState();

	void Mix(const std::span<uint8_t>& block, int round);

	void FinalizeState();
public:
	Mha256(const size_t blockSize = 16, const int rounds = 8);

	std::array<uint8_t, 32> ComputeHash(std::vector<uint8_t> buffer);
};
