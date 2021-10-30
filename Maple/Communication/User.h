#pragma once
#include <string>

struct User
{
	std::string Username;
	std::string SessionID;
	std::string DiscordID;
	std::string AvatarHash;

	User(std::string username, std::string sessionID, std::string discordID, std::string avatarHash)
	{
		Username = username;
		SessionID = sessionID;
		DiscordID = discordID;
		AvatarHash = avatarHash;
	}
};
