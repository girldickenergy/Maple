#pragma once

#include <map>
#include <string>
#include <vector>

#include "VanillaPatch.h"
#include "VanillaResult.h"

class VanillaPatcher
{
	static inline std::map<std::string, VanillaPatch> patches;
public:
	static VanillaResult InstallPatch(const std::string& name, uintptr_t address, const std::vector<uint8_t>& patch);
	static VanillaResult InstallPatch(const std::string& name, const std::string& pattern, uintptr_t scanBase, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch);
	static VanillaResult UninstallPatch(const std::string& name);
	static void UninstallAllPatches();
};
