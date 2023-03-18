#pragma once

#include <vector>

#include "rsa.h"
#include "secblock.h"
#include "../Singleton/Singleton.h"

class CryptoProvider : public Singleton<CryptoProvider>
{
	std::string xorKey;
	CryptoPP::RSA::PrivateKey rsaPrivateKey;
	CryptoPP::SecByteBlock aesKeyBlock;
	CryptoPP::SecByteBlock aesIVBlock;
public:
	CryptoProvider(singletonLock);

	void InitializeAES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

	std::vector<unsigned char> XOR(const std::vector<unsigned char>& data);
	std::string Base64Encode(const std::vector<unsigned char>& data);
	std::vector<unsigned char> Base64Decode(const std::string& encoded);
	std::vector<unsigned char> RSADecrypt(const std::vector<unsigned char>& ciphertext);
	std::vector<unsigned char> AESEncrypt(const std::vector<unsigned char>& cleartext);
	std::vector<unsigned char> AESDecrypt(const std::vector<unsigned char>& ciphertext);
};
