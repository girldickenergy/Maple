#pragma once

#include <string>
#include <fstream>
#include <vector>

class SerializationReader
{
	std::istream* filestream;
public:
	SerializationReader(std::istream* filestream);

	unsigned char ReadByte();
	bool ReadBoolean();
	unsigned short ReadUInt16();
	int ReadInt32();
	long ReadInt64();
	std::vector<unsigned char> ReadByteArray();
	std::string ReadString();
};