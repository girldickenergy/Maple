#pragma once

#include <map>
#include <stdexcept>
#include <string>

class DependencyContainer
{
	static inline std::map<std::string, void*> dependencyCache;

	static void* findDependency(const std::string& name)
	{
		for (auto [dependencyName, dependency] : dependencyCache)
			if (dependencyName == name)
				return dependency;

		return nullptr;
	}

public:
	static void Cache(const std::string& name, void* dependency)
	{
		if (findDependency(name) != nullptr)
			throw std::runtime_error(name + " dependency is already cached!");

		dependencyCache.insert(std::make_pair(name, dependency));
	}

	static void* Get(const std::string& name)
	{
		void* dependency = findDependency(name);
		if (dependency == nullptr)
			throw std::runtime_error(name + " dependency is not cached!");

		return dependency;
	}
};
