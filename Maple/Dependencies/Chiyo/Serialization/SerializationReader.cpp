#include "SerializationReader.h"

SerializationReader::SerializationReader(std::istream* filestream)
{
	this->filestream = filestream;
}

unsigned char SerializationReader::ReadByte()
{
	return filestream->get();
}

bool SerializationReader::ReadBoolean()
{
	return ReadByte() != 0;
}

unsigned short SerializationReader::ReadUInt16()
{
	unsigned short result = 0;
	filestream->read((char*)&result, sizeof(unsigned short));

	return result;
}

int SerializationReader::ReadInt32()
{
	int result = 0;
	filestream->read((char*)&result, sizeof(int));

	return result;
}

long SerializationReader::ReadInt64()
{
	long long result = 0;
	filestream->read((char*)&result, sizeof(long long));

	return result;
}

std::vector<unsigned char> SerializationReader::ReadByteArray()
{
	

	int length = ReadInt32();
	if (length > 0)
	{
		std::vector<unsigned char> result(length);
		filestream->read((char*)&result[0], length);

		return result;
	}

	return std::vector<unsigned char>();
}

std::string SerializationReader::ReadString()
{
	if (ReadByte() != 0x0b)
		return std::string();

	int stringLength = 0;
	int shift = 0;
	while (shift != 0x23)
	{
		unsigned char b = ReadByte();
		stringLength |= (int)(b & 0x7f) << shift;
		shift += 7;
		if ((b & 128) == 0)
		{
			std::string result = std::string(stringLength, NULL);
			filestream->read(&result[0], stringLength);

			return result;
		}
	}

	return std::string();
}
