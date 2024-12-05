#include "Mha256.h"
#include <bit>

Mha256::Mha256(const size_t blockSize, const int rounds)
{
	_ASSERT(blockSize % 4 != 0);

	m_InitialState = {
		0x1f, 0x4b, 0xa7, 0x3c, 0xd2, 0x5e, 0x9f, 0x8a,
		0x7d, 0x6c, 0x2a, 0x1b, 0x4e, 0x3f, 0x5d, 0x6e,
		0x9a, 0x8b, 0x7c, 0x6d, 0x5e, 0x4f, 0x3a, 0x2b,
		0x1c, 0x0d, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54
	};

	m_BlockSize = blockSize;
	m_Rounds = rounds;
}

void Mha256::InitializeState()
{
	m_State = std::array<uint8_t, 32>();
	std::copy(std::begin(m_InitialState), std::end(m_InitialState), std::begin(m_State));
}

void Mha256::Mix(const std::span<uint8_t>& block, int round)
{
	auto segmentCount = m_BlockSize / 4;

	for (size_t segment = 0; segment < segmentCount; segment++)
	{
		auto offset = segment * 4;
		uint32_t blockInt = block[offset] << 24 | block[offset + 1] << 16 | block[offset + 2] << 8 | block[offset + 3];

		for (size_t i = 0; i < m_State.size(); i += 8)
		{
			uint32_t stateInt1 = m_State[i] << 24 | m_State[i + 1] << 16 | m_State[i + 2] << 8 | m_State[i + 3];
			uint32_t stateInt2 = m_State[i + 4] << 24 | m_State[i + 5] << 16 | m_State[i + 6] << 8 | m_State[i + 7];

			stateInt1 ^= blockInt;
			stateInt2 ^= blockInt;

			stateInt1 = std::rotl(stateInt1, 7);
			stateInt2 = std::rotl(stateInt2, 11);

			stateInt1 = ~stateInt1;
			stateInt2 = ~stateInt2;

			stateInt1 += (round + 1) * 0xDEADDAED;
			stateInt2 += (round + 1) * 0xBEEFFEEB;

			m_State[i] = (stateInt1 >> 24) & 0xFF;
			m_State[i + 1] = (stateInt1 >> 16) & 0xFF;
			m_State[i + 2] = (stateInt1 >> 8) & 0xFF;
			m_State[i + 3] = stateInt1 & 0xFF;

			m_State[i + 4] = (stateInt2 >> 24) & 0xFF;
			m_State[i + 5] = (stateInt2 >> 16) & 0xFF;
			m_State[i + 6] = (stateInt2 >> 8) & 0xFF;
			m_State[i + 7] = stateInt2 & 0xFF;
		}
	}
}

void Mha256::FinalizeState()
{
	uint32_t final = 0;

	for (const auto& b : m_State)
	{
		final = (final ^ b) * 0x39100010;
		final &= 0xFFFFFFFF;
	}

	for (size_t i = 0; i < m_State.size(); i++)
	{
		m_State[i] ^= final >> ((i % 4) * 8) & 0xFF;

		m_State[i] = ~m_State[i];
		m_State[i] = std::rotl(m_State[i], (i * 3) % 8);
	}
}

std::array<uint8_t, 32> Mha256::ComputeHash(std::vector<uint8_t> buffer)
{
	InitializeState();

	auto bufferSpan = std::span{ buffer };

	size_t fullBlocks = buffer.size() / m_BlockSize;
	for (size_t i = 0; i < fullBlocks; i++)
	{
		auto block = bufferSpan.subspan(i * m_BlockSize, m_BlockSize);
		for (size_t r = 0; r < m_Rounds; r++)
			Mix(block, r);
	}

	size_t remaining = buffer.size() % m_BlockSize;
	if (remaining > 0)
	{
		auto lastBlock = std::vector<uint8_t>(m_BlockSize);
		auto finalSpan = bufferSpan.subspan(fullBlocks * m_BlockSize, remaining);
		std::copy(finalSpan.begin(), finalSpan.end(), lastBlock.begin());

		for (size_t r = 0; r < m_Rounds; r++)
			Mix(std::span{ lastBlock }, r);
	}

	FinalizeState();

	return m_State;
}
