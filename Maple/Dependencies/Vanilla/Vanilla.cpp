#include "Vanilla.h"

#include <metahost.h>
#include <string>

#include "COM/SafeVector.h"
#include "NameCache.h"

void Vanilla::cacheMemoryRegions()
{
	memoryRegions.clear();

	MEMORY_BASIC_INFORMATION32 mbi;
	LPCVOID address = nullptr;

	while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
	{
		if (mbi.State == MEM_COMMIT && mbi.Protect >= 0x10 && mbi.Protect <= 0x80)
			memoryRegions.emplace_back(mbi);
		
		address = reinterpret_cast<LPCVOID>(mbi.BaseAddress + mbi.RegionSize);
	}
}

void Vanilla::initializeCOM()
{
	ICLRMetaHost* metaHost = nullptr;

	ICLRRuntimeInfo* runtimeInfo = nullptr;
	ICLRRuntimeHost* runtimeHost = nullptr;
	ICorRuntimeHost* corHost = nullptr;

	IUnknownPtr appDomain = NULL;

	CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&metaHost);

	metaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&runtimeInfo);

	runtimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&runtimeHost);
	runtimeHost->Start();

	runtimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&corHost));

	corHost->GetDefaultDomain(&appDomain);

	appDomain->QueryInterface(&DefaultDomain);
}

void Vanilla::populateAssemblies()
{
	const std::wstring osuPattern(L"osu!, Version");
	const std::wstring osuStubPattern1(L"w, Version");
	const std::wstring osuStubPattern2(L"AlphaZero, Version"); //todo: regex maybe?
	const std::wstring osuStubPattern3(L"9ade2f99-9f30-423b-ae0b-a1cd9d846d61, Version");
	const std::wstring mscorlibPattern(L"mscorlib, Version");

	bool found_osu = false, found_auth = false, found_mscorlib = false;
	while (!found_osu || !found_auth || !found_mscorlib)
	{
		SAFEARRAY* assArray;

		DefaultDomain->GetAssemblies(&assArray);

		auto assemblies = SafeVector(assArray);

		const auto asmCount = assemblies.count();

		for (int i = 0; i < (int)asmCount; i++)
		{
			IUnknownPtr unk = NULL;

			assemblies.getElement(i, &unk);
			mscorlib::_AssemblyPtr assembly = mscorlib::_AssemblyPtr(unk);

			BSTR bsAsmName;

			assembly->get_FullName(&bsAsmName);

			std::wstring asmName(bsAsmName, SysStringLen(bsAsmName));

			if (!found_osu && asmName.find(osuPattern) != std::wstring::npos)
			{
				OsuAssembly = Assembly(assembly);
				found_osu = true;
			}

			if (!found_auth && (asmName.find(osuStubPattern1) != std::wstring::npos || 
				asmName.find(osuStubPattern2) != std::wstring::npos || asmName.find(osuStubPattern3) != std::wstring::npos))
			{
				OsuStubAssembly = Assembly(assembly);
				found_auth = true;
			}

			if (!found_mscorlib && asmName.find(mscorlibPattern) != std::wstring::npos)
			{
				MscorlibAssembly = Assembly(assembly);
				found_mscorlib = true;
			}
		}
	}
}

void Vanilla::Initialize()
{
	initializeCOM();
	populateAssemblies();

	NameCache::Initialize(OsuAssembly, OsuStubAssembly);
	Explorer = ::Explorer(OsuAssembly);
}

uintptr_t Vanilla::FindSignature(const char* pattern, const char* mask)
{
	if (memoryRegions.empty())
		cacheMemoryRegions();

	for (const auto& region : memoryRegions)
		FindSignature(pattern, mask, region.BaseAddress, region.RegionSize);

	return NULL;
}

uintptr_t Vanilla::FindSignature(const char* signature, const char* mask, uintptr_t entryPoint, int size)
{
	const size_t signatureLength = strlen(mask);

	for (uintptr_t i = 0; i < size - signatureLength; i++)
	{
		bool found = true;
		for (uintptr_t j = 0; j < signatureLength; j++)
		{
			if (mask[j] != '?' && signature[j] != *reinterpret_cast<char*>(entryPoint + i + j))
			{
				found = false;
				break;
			}
		}

		if (found)
			return entryPoint + i;
	}

	return NULL;
}

DWORD Vanilla::GetModuleSize(const char* moduleName)
{
	const HMODULE module = GetModuleHandleA(moduleName);
	if (module == nullptr)
		return 0;

	IMAGE_DOS_HEADER* pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
	IMAGE_NT_HEADERS* pNTHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(pDOSHeader) + pDOSHeader->e_lfanew);

	return pNTHeaders->OptionalHeader.SizeOfImage;
}

bool Vanilla::InstallPatch(const std::string& name, uintptr_t entryPoint, const char* signature, const char* mask, int size, int offset, const char* patch)
{
	if (patches.count(name) > 0)
		return false;

	const uintptr_t address = FindSignature(signature, mask, entryPoint, size);
	if (address == NULL)
		return false;

	std::vector<char> originalBytes;
	const int patchSize = strlen(patch);
	for (int i = 0; i < patchSize; i++)
	{
		originalBytes.push_back(*reinterpret_cast<char*>(address + offset + i));
		*reinterpret_cast<char*>(address + offset + i) = patch[i];
	}

	patches[name] = Patch(address, originalBytes);

	return true;
}

bool Vanilla::InstallNOPPatch(const std::string& name, uintptr_t entryPoint, const char* signature, const char* mask, int size, int offset, int nopSize)
{
	if (patches.count(name) > 0)
		return false;

	const uintptr_t address = FindSignature(signature, mask, entryPoint, size);
	if (address == NULL)
		return false;

	std::vector<char> originalBytes;
	for (int i = 0; i < nopSize; i++)
	{
		originalBytes.push_back(*reinterpret_cast<char*>(address + offset + i));
		*reinterpret_cast<char*>(address + offset + i) = 0x90;
	}

	patches[name] = ::Patch(address, originalBytes);

	return true;
}

void Vanilla::RemovePatch(const std::string& name)
{
	if (patches.count(name) == 0)
		return;

	auto patch = patches[name];
	for (unsigned int i = 0; i < patch.OriginalBytes.size(); i++)
		*reinterpret_cast<char*>(patch.Location + i) = patch.OriginalBytes.at(i);

	patches.erase(name);
}

void Vanilla::RemoveAllPatches()
{
	while (!patches.empty())
		RemovePatch(patches.begin()->first);
}
