#pragma once

#include <vector>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <cstdint>
#include "xorstr.hpp"

class BinaryReader
{
	struct MemoryStream
	{
	private:
		const uint8_t* m_Data;
		size_t m_Size;
		size_t m_Pos;

	public:
		MemoryStream(const uint8_t* data, size_t size) : m_Data(data), m_Size(size), m_Pos(0) { }

		void Read(char* out, size_t count)
		{
			if (m_Pos + count > m_Size)
				throw std::runtime_error(xorstr_("Read exceeds buffer size"));

			std::memcpy(out, m_Data + m_Pos, count);
			m_Pos += count;
		}

		explicit operator bool() const { return m_Pos <= m_Size; }
	};

	MemoryStream m_Stream;

public:
    explicit BinaryReader(const std::vector<uint8_t>& buffer) : m_Stream(buffer.data(), buffer.size()) { }

    template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T>>* = nullptr>
    T Read()
	{
        T value;
		m_Stream.Read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!m_Stream)
            throw std::runtime_error(xorstr_("Failed to read from stream"));
        
        return value;
    }

	std::vector<uint8_t> Read(uint32_t count)
	{
		std::vector<uint8_t> value(count);
		m_Stream.Read(reinterpret_cast<char*>(value.data()), count);
		if (!m_Stream)
			throw std::runtime_error(xorstr_("Failed to read from stream"));

		return value;
	}
};