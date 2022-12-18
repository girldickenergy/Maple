#pragma once

#include <string>

class User
{
	std::string username;
	std::string sessionToken;
	std::string discordID;
	std::string avatarHash;
public:
	User(const std::string& username, const std::string& sessionToken, const std::string& discordID, const std::string& avatarHash);
	User() = default;

	const std::string& GetUsername();
	std::string GetUsernameHashed();
	const std::string& GetSessionToken();
	const std::string& GetDiscordID();
	const std::string& GetAvatarHash();
};
