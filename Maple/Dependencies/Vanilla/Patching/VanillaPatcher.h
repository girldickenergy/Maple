#pragma once

#include <map>
#include <string>
#include <vector>

#include "EncryptedString.h"
#include "VanillaPatch.h"
#include "VanillaResult.h"

class VanillaPatcher
{
	static inline std::map<EncryptedString, VanillaPatch> patches;
public:
	static VanillaResult InstallPatch(const char* name, uintptr_t address, const std::vector<uint8_t>& patch);
	static VanillaResult InstallPatch(const char* name, const std::string& pattern, uintptr_t scanBase, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch);
	static VanillaResult UninstallPatch(const char* name);
	static void UninstallAllPatches();
};
