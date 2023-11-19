#pragma once

#include <array>
#include <random>

template<typename T, int SIZE>
class EncryptedArray
{
	size_t m_Size;
	T m_Key;
    std::array<T, SIZE <= 0 ? 1 : SIZE> m_Buffer;

	static T generateKey()
	{
	    static std::random_device rd;
	    static std::mt19937 rng(rd());
	    static std::uniform_int_distribution<int> dist(1, (std::numeric_limits<uint8_t>::max)());

	    T key = 0;
	    const auto keyBytes = reinterpret_cast<uint8_t*>(&key);

	    for (size_t i = 0; i < sizeof(T); i++)
	        keyBytes[i] = static_cast<uint8_t>(dist(rng));

	    return key;
	}

public:
    EncryptedArray(T* initialBuffer = nullptr) : m_Size(SIZE <= 0 ? 1 : SIZE), m_Key(generateKey())
    {
		for (size_t i = 0; i < m_Size; i++)
			m_Buffer[i] = (initialBuffer ? initialBuffer[i] : 0) ^ m_Key;
    }

	EncryptedArray(const EncryptedArray& other) : m_Size(other.m_Size), m_Key(other.m_Key), m_Buffer(other.m_Buffer) {}

    EncryptedArray& operator=(const EncryptedArray& other)
    {
        if (this == &other)
            return *this;

        m_Size = other.m_Size;
        m_Key = other.m_Key;
        m_Buffer = other.m_Buffer;

        return *this;
    }

	friend bool operator==(const EncryptedArray& lhs, const EncryptedArray& rhs)
    {
        if (lhs.m_Size != rhs.m_Size)
            return false;

        for (size_t i = 0; i < lhs.m_Size; i++)
            if ((lhs.m_Buffer[i] ^ lhs.m_Key) != (rhs.m_Buffer[i] ^ rhs.m_Key))
                return false;

        return true;
    }

	void GetData(T* buffer) const
	{
        if (buffer)
        {
            for (size_t i = 0; i < m_Size; i++)
				buffer[i] = m_Buffer[i] ^ m_Key;
		}
	}

	void SetData(const T* buffer)
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