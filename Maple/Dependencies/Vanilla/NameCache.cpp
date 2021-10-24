#include "NameCache.h"

#include "Vanilla.h"

std::string NameCache::getDeobfuscatedNameInternal(std::string obfuscatedName)
{
	auto wObfuscatedName = std::wstring(obfuscatedName.begin(), obfuscatedName.end());
	bstr_t bObfuscatedName = SysAllocStringLen(wObfuscatedName.data(), wObfuscatedName.size());
	
	std::vector<variant_t> args;
	args.push_back(bObfuscatedName);
	
	auto deobfuscatedName = getClearName.InvokeUnsafe(variant_t(), args);
	std::wstring wsName(deobfuscatedName.bstrVal, SysStringLen(deobfuscatedName.bstrVal));
	std::string name(wsName.begin(), wsName.end());
	
	return name;
}

void NameCache::Initialize(Assembly assemblyToDeobfuscate, Assembly stub)
{
	deobfuscatedNameLookup.clear();
	obfuscatedNameLookup.clear();
	
	getClearName = stub.GetTypeUnsafe("A.B").GetMethodUnsafe("X");

	auto types = assemblyToDeobfuscate.GetTypesUnsafe();
	for (auto& type : types)
	{
		auto name = type.GetFullNameUnsafe();
		if (!name._Starts_with("#=z") || name.find_first_of('+') != std::string::npos)
			continue;

		auto deobfuscatedTypeName = getDeobfuscatedNameInternal(name);

		obfuscatedNameLookup[deobfuscatedTypeName] = name;
		deobfuscatedNameLookup[name] = deobfuscatedTypeName;

		for (auto& field : type.GetFieldsUnsafe())
		{
			auto fieldName = field.GetFullNameUnsafe();

			if (!fieldName._Starts_with("#=z"))
				continue;

			auto deobfuscatedFieldName = getDeobfuscatedNameInternal(fieldName);

			obfuscatedNameLookup[deobfuscatedFieldName] = fieldName;
			deobfuscatedNameLookup[fieldName] = deobfuscatedFieldName;
		}

		for (auto& method : type.GetMethodsUnsafe())
		{
			auto methodName = method.GetFullNameUnsafe();

			if (!methodName._Starts_with("#=z"))
				continue;

			auto deobfuscatedMethodName = getDeobfuscatedNameInternal(methodName);
			
			obfuscatedNameLookup[deobfuscatedMethodName] = methodName;
			deobfuscatedNameLookup[methodName] = deobfuscatedMethodName;
		}
	}
}

std::string NameCache::GetDeobfuscatedName(std::string obfuscatedName)
{
	std::string name = deobfuscatedNameLookup[obfuscatedName];
	if (name.empty())
		return obfuscatedName;

	return name;
}

std::string NameCache::GetObfuscatedName(std::string deobfuscatedName)
{
	std::string name = obfuscatedNameLookup[deobfuscatedName];
	if (name.empty())
		return deobfuscatedName;
	
	return name;
}