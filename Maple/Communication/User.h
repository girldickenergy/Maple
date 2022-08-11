#pragma once

#include "../Utilities/Crypto/CryptoUtilities.h"

struct User
{
	std::string Username;
	std::string UsernameHashed;
	std::string SessionID;
	std::string DiscordID;
	std::string AvatarHash;

	User(const std::string& username, const std::string& sessionID, const std::string& discordID, const std::string& avatarHash)
	{
		Username = username;
		UsernameHashed = CryptoUtilities::GetMD5Hash(username);
		SessionID = sessionID;
		DiscordID = discordID;
		AvatarHash = avatarHash;
	}
};