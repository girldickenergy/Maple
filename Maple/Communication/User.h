#pragma once

#include <string>

#include "../Utilities/Crypto/CryptoHelper.h"

struct User
{
	std::string Username;
	std::string UsernameHashed;
	std::string SessionID;
	std::string DiscordID;
	std::string AvatarHash;

	User(std::string username, std::string sessionID, std::string discordID, std::string avatarHash)
	{
		Username = username;
		UsernameHashed = CryptoHelper::GetMD5Hash(username);
		SessionID = sessionID;
		DiscordID = discordID;
		AvatarHash = avatarHash;
	}
};
