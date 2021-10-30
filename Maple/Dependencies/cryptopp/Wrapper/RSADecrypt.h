#pragma once
#include <string>
#include "../base64.h"
#include "../rsa.h"
#include "../osrng.h"
#include "../hex.h"
#include "../files.h"
#include "../pssr.h"
#include "ThemidaSDK.h"

class RSADecrypt {

private:
	std::string pubKey;
	std::string pubKeyDecoded;

	void decodePubKeyBase64();
public:
	RSADecrypt();

	std::vector<unsigned char> Decode(std::vector<unsigned char> input, int signatureLen);
};