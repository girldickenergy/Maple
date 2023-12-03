#pragma once

#include <windows.h>

#include <fstream>

#include "EncryptedString.h"
#include "xorstr.hpp"

#pragma pack(push, 1)
struct StorageConfig
{
	EncryptedString DefaultConfig = xorstr_("default");
	EncryptedString DefaultProfile = xorstr_("none");
	bool ShowMenuAfterInjection = true;
	int MenuKey = VK_DELETE;

	void Serialize(std::ostream& outStream)
	{
		DefaultConfig.Serialize(outStream);
		DefaultProfile.Serialize(outStream);
        outStream.write(reinterpret_cast<const char*>(&ShowMenuAfterInjection), sizeof(bool));
		outStream.write(reinterpret_cast<const char*>(&MenuKey), sizeof(int));
	}

	void Deserialize(std::istream& inStream)
	{
        DefaultConfig.Deserialize(inStream);
        DefaultProfile.Deserialize(inStream);
        inStream.read(reinterpret_cast<char*>(&ShowMenuAfterInjection), sizeof(bool));
		inStream.read(reinterpret_cast<char*>(&MenuKey), sizeof(int));
	}
};
#pragma pack(pop)
