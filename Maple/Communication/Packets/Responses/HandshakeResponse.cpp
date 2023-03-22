#include "HandshakeResponse.h"

#include "json.hpp"
#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../../Crypto/CryptoProvider.h"
#include "../../../Utilities/Strings/StringUtilities.h"

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

HandshakeResponse HandshakeResponse::Deserialize(const std::vector<unsigned char>& payload)
{
	VIRTUALIZER_SHARK_BLACK_START
	
	nlohmann::json jsonPayload = nlohmann::json::parse(StringUtilities::ByteArrayToString(CryptoProvider::Get().RSADecrypt(payload)));

	HandshakeResponse response = HandshakeResponse(CryptoProvider::Get().Base64Decode(jsonPayload[xorstr_("Key")]), CryptoProvider::Get().Base64Decode(jsonPayload[xorstr_("IV")]));

		VIRTUALIZER_SHARK_BLACK_END

	return response;
}
