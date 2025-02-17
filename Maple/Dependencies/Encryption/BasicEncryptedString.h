#pragma once

#include <fstream>
#include <vector>
#include <random>

template<typename T>
class BasicEncryptedString
{
	T m_Key;
	std::vector<T> m_Data;

	static T GenerateKey()
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

	class EncryptedValueProxy
    {
        BasicEncryptedString<T>& m_EncryptedString;
        size_t m_Index;

	public:
        EncryptedValueProxy(BasicEncryptedString<T>& encryptedString, size_t index) : m_EncryptedString(encryptedString), m_Index(index) {}

        operator T() const
        {
            return m_EncryptedString.m_Data[m_Index] ^ m_EncryptedString.m_Key;
        }

        EncryptedValueProxy& operator=(T value)
        {
            m_EncryptedString.m_Data[m_Index] = value ^ m_EncryptedString.m_Key;

            return *this;
        }
    };

public:
	BasicEncryptedString() : m_Key(GenerateKey()), m_Data()
	{
        m_Data.push_back(0 ^ m_Key);
	}

	BasicEncryptedString(T* plaintext) : m_Key(GenerateKey()), m_Data()
	{
        do
        {
            m_Data.push_back(*plaintext ^ m_Key);
        } while (*plaintext++ != 0);
	}

	BasicEncryptedString(const T* plaintext) : m_Key(GenerateKey()), m_Data()
	{
        do
        {
            m_Data.push_back(*plaintext ^ m_Key);
        } while (*plaintext++ != 0);
	}

	BasicEncryptedString(const BasicEncryptedString& other)
	{
        if (!other.m_Data.data() || other.m_Data.size() <= 0)
            throw;

        m_Key = other.m_Key;
        m_Data = std::vector<T>(other.m_Data.size());
        std::copy(other.m_Data.begin(), other.m_Data.end(), m_Data.data());
    }

    BasicEncryptedString& operator=(const BasicEncryptedString& other)
    {
        if (this == &other)
            return *this;

        m_Key = other.m_Key;
		m_Data = std::vector<T>(other.m_Data.size());
        std::copy(other.m_Data.begin(), other.m_Data.end(), m_Data.data());

        return *this;
    }

	friend bool operator==(const BasicEncryptedString& lhs, const BasicEncryptedString& rhs)
    {
        if (lhs.m_Data.size() != rhs.m_Data.size())
            return false;

        for (size_t i = 0; i < lhs.m_Data.size(); i++)
            if ((lhs.m_Data[i] ^ lhs.m_Key) != (rhs.m_Data[i] ^ rhs.m_Key))
                return false;

        return true;
    }

	friend bool operator<(const BasicEncryptedString& lhs, const BasicEncryptedString& rhs)
	{
		if (lhs.m_Data.size() != rhs.m_Data.size())
			return lhs.m_Data.size() < rhs.m_Data.size();

		for (size_t i = 0; i < lhs.m_Data.size(); i++)
		{
			T lhs_value = lhs.m_Data[i] ^ lhs.m_Key;
			T rhs_value = rhs.m_Data[i] ^ rhs.m_Key;

			if (lhs_value != rhs_value)
				return lhs_value < rhs_value;
		}

		return false;
	}

	BasicEncryptedString& operator+=(const BasicEncryptedString& other)
	{
        while (!m_Data.empty() && (m_Data.back() ^ m_Key) == 0)
            m_Data.pop_back();

	    for (size_t i = 0; i < other.m_Data.size(); i++)
            m_Data.push_back((other.m_Data[i] ^ other.m_Key) ^ m_Key);

	    if ((m_Data.back() ^ m_Key) != 0)
	        m_Data.push_back(0 ^ m_Key);

	    return *this;
	}

	friend BasicEncryptedString operator+(BasicEncryptedString lhs, const BasicEncryptedString& rhs)
	{
	    lhs += rhs;

	    return lhs;
	}

	EncryptedValueProxy operator[](size_t index)
	{
		return EncryptedValueProxy(*this, index);
	}

	T operator[](size_t index) const
	{
		return m_Data[index] ^ m_Key;
	}

	size_t GetSize() const
	{
		return m_Data.size();
	}

	void SetSize(size_t newSize)
	{
		m_Data.resize(newSize, 0 ^ m_Key);

		if ((m_Data.back() ^ m_Key) != 0)
            m_Data.back() = 0 ^ m_Key;
	}

	void GetData(T* outData) const
	{
		for (size_t i = 0; i < m_Data.size(); i++)
            outData[i] = m_Data.at(i) ^ m_Key;
	}

	void SetData(T* inData)
	{
        m_Data.clear();

		do
		{
            m_Data.push_back(*inData ^ m_Key);
        } while (*inData++ != 0);
	}

	void SetData(T* data, size_t size)
	{
        m_Data.clear();

		for (size_t i = 0; i < size; i++)
            m_Data.push_back(data[i] ^ m_Key);

		if (data[size - 1] != 0)
            m_Data.push_back(0 ^ m_Key);
	}

	void Clear()
	{
		m_Data.clear();

		m_Data.push_back(0 ^ m_Key);
	}

    void Serialize(std::ostream& outStream) const
	{
        outStream.write(reinterpret_cast<const char*>(&m_Key), sizeof(T));

        const size_t dataSize = m_Data.size();
        outStream.write(reinterpret_cast<const char*>(&dataSize), sizeof(size_t));

        outStream.write(reinterpret_cast<const char*>(m_Data.data()), m_Data.size() * sizeof(T));
    }

	void Deserialize(std::istream &inStream)
	{
		inStream.read(reinterpret_cast<char*>(&m_Key), sizeof(T));

        size_t dataSize;
        inStream.read(reinterpret_cast<char*>(&dataSize), sizeof(size_t));

		m_Data = std::vector<T>(dataSize);

        inStream.read(reinterpret_cast<char*>(m_Data.data()), sizeof(T) * dataSize);
	}

	std::vector<uint8_t> Serialize() const
	{
        std::vector<uint8_t> result(sizeof(T) + sizeof(size_t) + m_Data.size() * sizeof(T));

		*(T*)result.data() = m_Key;
        *(size_t*)(result.data() + sizeof(T)) = m_Data.size();
        std::memcpy(result.data() + sizeof(T) + sizeof(size_t), m_Data.data(), m_Data.size());

		return result;
    }

	void Deserialize(const std::vector<uint8_t> data)
	{
        m_Key = *(T*)data.data();
        size_t dataSize = *(size_t*)(data.data() + sizeof(T));

        m_Data = std::vector<T>(dataSize);

		std::memcpy(m_Data.data(), data.data() + sizeof(T) + sizeof(size_t), dataSize);
	}
};
