#include "VanillaPatcher.h"

#include "PatternScanning/VanillaPatternScanner.h"

VanillaResult VanillaPatcher::InstallPatch(const std::string& name, uintptr_t address, const std::vector<uint8_t>& patch)
{
	if (patches.contains(name))
		return VanillaResult::PatchAlreadyInstalled;

	std::vector<uint8_t> originalBytes;
	for (unsigned int i = 0; i < patch.size(); i++)
	{
		originalBytes.push_back(*reinterpret_cast<uint8_t*>(address + i));
		*reinterpret_cast<uint8_t*>(address + i) = patch[i];
	}

	patches[name] = VanillaPatch(address, originalBytes);

	return VanillaResult::Success;
}

VanillaResult VanillaPatcher::InstallPatch(const std::string& name, const std::string& pattern, uintptr_t scanBase, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch)
{
	if (patches.contains(name))
		return VanillaResult::PatchAlreadyInstalled;

	if (const uintptr_t patchAddress = VanillaPatternScanner::FindPatternInRange(pattern, scanBase, scanSize, offset))
		return InstallPatch(name, patchAddress, patch);

	return VanillaResult::PatchPatternNotFound;
}

VanillaResult VanillaPatcher::UninstallPatch(const std::string& name)
{
	if (!patches.contains(name))
		return VanillaResult::PatchNotInstalled;

	const VanillaPatch patch = patches[name];
	for (unsigned int i = 0; i < patch.OriginalBytes.size(); i++)
		*reinterpret_cast<uint8_t*>(patch.Address + i) = patch.OriginalBytes.at(i);

	patches.erase(name);

	return VanillaResult::Success;
}

void VanillaPatcher::UninstallAllPatches()
{
	while (!patches.empty())
		UninstallPatch(patches.begin()->first);
}
