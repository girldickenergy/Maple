#include "User.h"
#include "../Utilities/Crypto/CryptoUtilities.h"

User::User(const std::string& username, const std::string& sessionToken, const std::string& discordID, const std::string& avatarHash)
{
	this->username = username;
	this->sessionToken = sessionToken;
	this->discordID = discordID;
	this->avatarHash = avatarHash;
}

const std::string& User::GetUsername()
{
	return username;
}

std::string User::GetUsernameHashed(const char* postfix)
{
	return CryptoUtilities::GetMD5Hash(username + postfix);
}

const std::string& User::GetSessionToken()
{
	return sessionToken;
}

const std::string& User::GetDiscordID()
{
	return discordID;
}

const std::string& User::GetAvatarHash()
{
	return avatarHash;
}
