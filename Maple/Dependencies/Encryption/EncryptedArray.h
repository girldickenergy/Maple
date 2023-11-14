#pragma once

#include <array>
#include <random>

template<typename T, int SIZE>
class EncryptedArray
{
	size_t m_Size;
	uint8_t m_Key;
    std::array<T, SIZE <= 0 ? 1 : SIZE> m_Buffer;

	static uint8_t generateKey()
	{
	    static std::random_device rd;
	    static std::mt19937 rng(rd());

	    std::uniform_int_distribution<int> dist(1, (std::numeric_limits<uint8_t>::max)());

	    return (uint8_t)dist(rng);
	}

public:
    EncryptedArray(T* initialBuffer = nullptr) : m_Size(SIZE <= 0 ? 1 : SIZE), m_Key(generateKey())
    {
		for (size_t i = 0; i < m_Size; i++)
			m_Buffer[i] = (initialBuffer ? initialBuffer[i] : 0) ^ m_Key;
    }

	EncryptedArray(const EncryptedArray &) = delete;
    EncryptedArray &operator=(const EncryptedArray &) = delete;

	void DecryptTo(T* buffer) const
	{
        if (buffer)
        {
            for (size_t i = 0; i < m_Size; i++)
				buffer[i] = m_Buffer[i] ^ m_Key;
		}
	}

	void EncryptFrom(const T* buffer)
	{
		if (buffer)
        {
	        m_Key = generateKey();

			for (size_t i = 0; i < m_Size; i++)
                m_Buffer[i] = buffer[i] ^ m_Key;
		}
	}

	void Clear()
	{
		for (size_t i = 0; i < m_Size; i++)
			m_Buffer[i] = 0 ^ m_Key;
	}
};