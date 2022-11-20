#include "HandshakeResponse.h"

#include "json.hpp"
#include "ThemidaSDK.h"

#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"
#include "../../../Utilities/Security/xorstr.hpp"

HandshakeResponse::HandshakeResponse(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	this->key = key;
	this->iv = iv;
}

const std::vector<unsigned char>& HandshakeResponse::GetKey()
{
	return key;
}

const std::vector<unsigned char>& HandshakeResponse::GetIV()
{
	return iv;
}

#pragma optimize("", off)
HandshakeResponse HandshakeResponse::Deserialize(const std::vector<unsigned char>& payload)
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::GetInstance()->RSADecrypt(payload)));

	HandshakeResponse response = HandshakeResponse(CryptoProvider::GetInstance()->Base64Decode(jsonPayload[xorstr_("Key")]), CryptoProvider::GetInstance()->Base64Decode(jsonPayload[xorstr_("IV")]));

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	return response;
}
#pragma optimize("", on)
