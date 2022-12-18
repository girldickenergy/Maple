#pragma once

#include <vector>

#include "rsa.h"
#include "secblock.h"

class CryptoProvider
{
	std::string xorKey;
	CryptoPP::RSA::PrivateKey rsaPrivateKey;
	CryptoPP::SecByteBlock aesKeyBlock;
	CryptoPP::SecByteBlock aesIVBlock;

	static inline CryptoProvider* instance = nullptr;
	CryptoProvider();
public:
	static CryptoProvider* GetInstance()
	{
		if (!instance)
			instance = new CryptoProvider();

		return instance;
	}

	void InitializeAES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

	std::vector<unsigned char> XOR(const std::vector<unsigned char>& data);
	std::vector<unsigned char> Base64Decode(const std::string& encoded);
	std::vector<unsigned char> RSADecrypt(const std::vector<unsigned char>& ciphertext);
	std::vector<unsigned char> AESEncrypt(const std::vector<unsigned char>& cleartext);
	std::vector<unsigned char> AESDecrypt(const std::vector<unsigned char>& ciphertext);
};
