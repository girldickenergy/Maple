#pragma once

#include <vector>

#include "rsa.h"
#include "secblock.h"
#include "../Singleton/Singleton.h"
#include "CryptoTransformation.h"

class CryptoProvider : public Singleton<CryptoProvider>
{
	std::string m_XorKey;
	CryptoPP::RSA::PrivateKey m_RsaPrivateKey;
	CryptoPP::SecByteBlock m_AesKeyBlock;
	CryptoPP::SecByteBlock m_AesIvBlock;
public:
	CryptoProvider(singletonLock);

	void InitializeAES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

	std::vector<unsigned char> XOR(const std::vector<unsigned char>& data);
	std::string Base64Encode(const std::vector<unsigned char>& data);
	std::vector<unsigned char> Base64Decode(const std::string& encoded);
	std::vector<unsigned char> RSADecrypt(const std::vector<unsigned char>& ciphertext);
	std::vector<unsigned char> AESEncrypt(const std::vector<unsigned char>& cleartext);
	std::vector<unsigned char> AESDecrypt(const std::vector<unsigned char>& ciphertext);

	std::vector<uint8_t> ApplyCryptoTransformations(const std::vector<uint8_t>& buffer, uint32_t key1, uint32_t key2, uint32_t key3, bool reverse = false);
};
