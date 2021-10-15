#pragma once

#include <map>
#include <string>

#include "COM/Method.h"

class NameCache
{
	static inline std::map<std::string, std::string> deobfuscatedNameLookup;
	static inline std::map<std::string, std::string> obfuscatedNameLookup;
	static inline Method getClearName;
	
	static std::string getDeobfuscatedNameInternal(std::string obfuscatedName);
public:
	static void Initialize(Assembly assemblyToDeobfuscate, Assembly stub);
	static std::string GetDeobfuscatedName(std::string obfuscatedName);
	static std::string GetObfuscatedName(std::string deobfuscatedName);
};
