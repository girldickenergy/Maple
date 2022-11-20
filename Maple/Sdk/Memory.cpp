#include "Memory.h"

#include "ThemidaSDK.h"
#include "Vanilla.h"
#include "Hooking/VanillaHooking.h"
#include "Patching/VanillaPatcher.h"
#include "PatternScanning/VanillaPatternScanner.h"

#include "../Utilities/Security/xorstr.hpp"
#include "../Logging/Logger.h"
#include "../Communication/Communication.h"
#include "../Utilities/Security/Security.h"

void Memory::jitCallback(uintptr_t address, unsigned int size)
{
	std::unique_lock lock(mutex);

	for (auto it = pendingObjects.cbegin(); it != pendingObjects.cend();)
	{
		if (const uintptr_t scanResult = VanillaPatternScanner::FindPatternInRange(it->second.Pattern, address, size, it->second.Offset, it->second.ReadCount))
		{
			Logger::Log(LogSeverity::Info, xorstr_("%s has been resolved dynamically!"), it->first.c_str());
			Objects[it->first] = scanResult;

			it = pendingObjects.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = pendingPatches.cbegin(); it != pendingPatches.cend();)
	{
		if (const uintptr_t objectAddress = Objects[it->first])
		{
			if (VanillaPatcher::InstallPatch(it->second.Name, it->second.Pattern, objectAddress, it->second.ScanSize, it->second.Offset, it->second.Patch) == VanillaResult::Success)
				Logger::Log(LogSeverity::Info, xorstr_("Patched %s dynamically!"), it->second.Name.c_str());
			else
				Logger::Log(LogSeverity::Error, xorstr_("Failed to patch %s dynamically!"), it->second.Name.c_str());

			it = pendingPatches.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = pendingHooks.cbegin(); it != pendingHooks.cend();)
	{
		if (const uintptr_t objectAddress = Objects[it->first])
		{
			if (VanillaHooking::InstallHook(it->second.Name, objectAddress, it->second.DetourFunctionAddress, it->second.OriginalFunction) == VanillaResult::Success)
				Logger::Log(LogSeverity::Info, xorstr_("Hooked %s dynamically!"), it->second.Name.c_str());
			else
				Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s dynamically!"), it->second.Name.c_str());

			it = pendingHooks.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (initialized && pendingObjects.empty() && pendingPatches.empty() && pendingHooks.empty())
		Vanilla::RemoveJITCallback();
}

void Memory::StartInitialize()
{
	Vanilla::SetJITCallback(jitCallback);
}

void Memory::EndInitialize()
{
	initialized = true;
}

void Memory::AddObject(const std::string& name, const std::string& pattern, unsigned int offset, unsigned int readCount)
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched() || !Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle))
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}
	
	if (const uintptr_t scanResult = VanillaPatternScanner::FindPattern(pattern, offset, readCount))
	{
		Logger::Log(LogSeverity::Info, xorstr_("%s has been resolved!"), name.c_str());

		Objects[name] = scanResult;
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to resolve %s. Adding it to the queue for dynamic resolution."), name.c_str());

		pendingObjects[name] = MaplePattern(pattern, offset, readCount);
	}

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

void Memory::AddPatch(const std::string& name, const std::string& objectName, const std::string& pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch)
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched() || !Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle))
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}

	if (const uintptr_t objectAddress = Objects[objectName])
	{
		if (VanillaPatcher::InstallPatch(name, pattern, objectAddress, scanSize, offset, patch) == VanillaResult::Success)
			Logger::Log(LogSeverity::Info, xorstr_("Patched %s!"), name.c_str());
		else
			Logger::Log(LogSeverity::Error, xorstr_("Failed to patch %s!"), name.c_str());
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to patch %s. Adding it to the queue for dynamic patching."), name.c_str());

		pendingPatches[objectName] = MaplePatch(name, pattern, scanSize, offset, patch);
	}

	STR_ENCRYPT_END
	VM_FISH_RED_END
}

void Memory::AddHook(const std::string& name, const std::string& objectName, uintptr_t detourFunctionAddress, uintptr_t* originalFunction)
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched() || !Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle))
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}

	if (const uintptr_t objectAddress = Objects[objectName])
	{
		if (VanillaHooking::InstallHook(name, objectAddress, detourFunctionAddress, originalFunction) == VanillaResult::Success)
			Logger::Log(LogSeverity::Info, xorstr_("Hooked %s!"), name.c_str());
		else
			Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s!"), name.c_str());
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to hook %s. Adding it to the queue for dynamic hooking."), name.c_str());

		pendingHooks[objectName] = MapleHook(name, detourFunctionAddress, originalFunction);
	}

	STR_ENCRYPT_END
	VM_FISH_RED_END
}
