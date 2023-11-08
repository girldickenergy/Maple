#include "Memory.h"

#include "VirtualizerSDK.h"
#include "Vanilla.h"
#include "xorstr.hpp"
#include "Hooking/VanillaHooking.h"
#include "Patching/VanillaPatcher.h"
#include "PatternScanning/VanillaPatternScanner.h"

#include "../Logging/Logger.h"
#include "../Communication/Communication.h"
#include "../Utilities/Security/Security.h"
#include "../Dependencies/Milk/Milk.h"
#include "../Configuration/ConfigManager.h"
#include "Scoring/Score.h"

void Memory::jitCallback(uintptr_t address, unsigned int size)
{
	std::unique_lock lock(mutex);

	for (auto it = pendingObjects.cbegin(); it != pendingObjects.cend();)
	{
		if (const uintptr_t scanResult = VanillaPatternScanner::FindPatternInRange(it->second.Pattern, address, size, it->second.Offset, it->second.ReadCount, it->second.ResolveRelativeAddress))
		{
			Logger::Log(LogSeverity::Info, xorstr_("%s has been resolved dynamically!"), it->first.c_str());
			Objects[it->first] = scanResult;

			// special handling for submit hook. holy fuck.
			if (it->first == xorstr_("Player::HandleScoreSubmission"))
			{
				if (const uintptr_t submitPatchLocation = VanillaPatternScanner::FindPatternInRange(xorstr_("8B 3D ?? ?? ?? ?? 8B CF 39 09 E8"), scanResult, 0x43E, 0xB))
				{
					void* submit = reinterpret_cast<void*>(static_cast<intptr_t>(submitPatchLocation) + 0x4 + *reinterpret_cast<int*>(submitPatchLocation));
					Score::SetOriginal(submit);

					int relativeSubmitHook = reinterpret_cast<intptr_t>(Score::GetHook()) - static_cast<intptr_t>(submitPatchLocation) - 0x4;
					*reinterpret_cast<int*>(submitPatchLocation) = relativeSubmitHook;

					Logger::Log(LogSeverity::Info, xorstr_("Hooked Score::Submit!"));

					if (!Milk::Get().DoCRCBypass(scanResult))
					{
						ConfigManager::ForceDisableScoreSubmission = true;
						ConfigManager::BypassFailed = true;

						Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), it->first.c_str());
					}
				}
				else
				{
					Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s. Score::Submit!"));
					Security::CorruptMemory();
				}
			}

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

			if (!Milk::Get().DoCRCBypass(objectAddress))
			{
				ConfigManager::ForceDisableScoreSubmission = true;
				ConfigManager::BypassFailed = true;

				Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), it->second.Name.c_str());
			}

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
			if (VanillaHooking::InstallHook(it->second.Name, objectAddress, it->second.DetourFunctionAddress, it->second.OriginalFunction, it->second.Safe) == VanillaResult::Success)
				Logger::Log(LogSeverity::Info, xorstr_("Hooked %s dynamically!"), it->second.Name.c_str());
			else
				Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s dynamically!"), it->second.Name.c_str());

			if (!Milk::Get().DoCRCBypass(objectAddress))
			{
				ConfigManager::ForceDisableScoreSubmission = true;
				ConfigManager::BypassFailed = true;

				Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), it->second.Name.c_str());
			}

			it = pendingHooks.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (initialized && pendingObjects.empty() && pendingPatches.empty() && pendingHooks.empty())
		Vanilla::RemoveJITCallback();

	lock.unlock();
}

void Memory::StartInitialize()
{
	Vanilla::SetJITCallback(jitCallback);
}

void Memory::EndInitialize()
{
	initialized = true;
}

void Memory::AddObject(const std::string& name, const std::string& pattern, unsigned int offset, unsigned int readCount, bool resolveRelativeAddress)
{
        VIRTUALIZER_TIGER_WHITE_START
	
	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched() || !Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle))
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}
	
	if (const uintptr_t scanResult = VanillaPatternScanner::FindPattern(pattern, offset, readCount, resolveRelativeAddress))
	{
		Logger::Log(LogSeverity::Info, xorstr_("%s has been resolved!"), name.c_str());

		Objects[name] = scanResult;

		// special handling for submit hook. holy fuck.
		if (name == xorstr_("Player::HandleScoreSubmission"))
		{
			if (const uintptr_t submitPatchLocation = VanillaPatternScanner::FindPatternInRange(xorstr_("8B 3D ?? ?? ?? ?? 8B CF 39 09 E8"), scanResult, 0x43E, 0xB))
			{
				void* submit = reinterpret_cast<void*>(static_cast<intptr_t>(submitPatchLocation) + 0x4 + *reinterpret_cast<int*>(submitPatchLocation));
				Score::SetOriginal(submit);

				int relativeSubmitHook = reinterpret_cast<intptr_t>(Score::GetHook()) - static_cast<intptr_t>(submitPatchLocation) - 0x4;
				*reinterpret_cast<int*>(submitPatchLocation) = relativeSubmitHook;

				Logger::Log(LogSeverity::Info, xorstr_("Hooked Score::Submit!"));

				if (!Milk::Get().DoCRCBypass(scanResult))
				{
					ConfigManager::ForceDisableScoreSubmission = true;
					ConfigManager::BypassFailed = true;

					Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), name.c_str());
				}
			}
			else
			{
				Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s. Score::Submit!"));
				Security::CorruptMemory();
			}
		}
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to resolve %s. Adding it to the queue for dynamic resolution."), name.c_str());

		pendingObjects[name] = MaplePattern(pattern, offset, readCount, resolveRelativeAddress);
	}

	VIRTUALIZER_TIGER_WHITE_END
}

void Memory::AddPatch(const std::string& name, const std::string& objectName, const std::string& pattern, unsigned int scanSize, unsigned int offset, const std::vector<uint8_t>& patch)
{
        VIRTUALIZER_TIGER_WHITE_START
	
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

		if (!Milk::Get().DoCRCBypass(objectAddress))
		{
			ConfigManager::ForceDisableScoreSubmission = true;
			ConfigManager::BypassFailed = true;

			Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), objectName.c_str());
		}
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to patch %s. Adding it to the queue for dynamic patching."), name.c_str());

		pendingPatches[objectName] = MaplePatch(name, pattern, scanSize, offset, patch);
	}

	VIRTUALIZER_TIGER_WHITE_END
}

void Memory::AddHook(const std::string& name, const std::string& objectName, uintptr_t detourFunctionAddress, uintptr_t* originalFunction, bool safe)
{
        VIRTUALIZER_TIGER_WHITE_START
	
	if (!Communication::GetIsConnected() || !Communication::GetIsHandshakeSucceeded() || !Communication::GetIsHeartbeatThreadLaunched() || !Security::CheckIfThreadIsAlive(Communication::ThreadCheckerHandle))
	{
		Communication::IntegritySignature1 -= 0x1;
		Communication::IntegritySignature2 -= 0x1;
		Communication::IntegritySignature3 -= 0x1;
	}

	if (const uintptr_t objectAddress = Objects[objectName])
	{
		if (VanillaHooking::InstallHook(name, objectAddress, detourFunctionAddress, originalFunction, safe) == VanillaResult::Success)
			Logger::Log(LogSeverity::Info, xorstr_("Hooked %s!"), name.c_str());
		else
			Logger::Log(LogSeverity::Error, xorstr_("Failed to hook %s!"), name.c_str());

		if (!Milk::Get().DoCRCBypass(objectAddress))
		{
			ConfigManager::ForceDisableScoreSubmission = true;
			ConfigManager::BypassFailed = true;

			Logger::Log(LogSeverity::Error, xorstr_("Failed to bypass CRC check for %s!"), objectName.c_str());
		}
	}
	else
	{
		Logger::Log(LogSeverity::Debug, xorstr_("Failed to hook %s. Adding it to the queue for dynamic hooking."), name.c_str());

		pendingHooks[objectName] = MapleHook(name, detourFunctionAddress, originalFunction, safe);
	}

	VIRTUALIZER_TIGER_WHITE_END
}
