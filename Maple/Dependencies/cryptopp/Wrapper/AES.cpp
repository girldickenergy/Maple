#include "AES.h"

AESWrapper::AESWrapper()
{
	srand(static_cast<unsigned>(time(nullptr)));  // NOLINT(cert-msc51-cpp)
	GenerateRandomIV();
	GenerateRandomKey();
}

void AESWrapper::GenerateRandomIV()
{
	for (int i = 0; i < 16; i++)
		IV[i] = rand() % 256; // NOLINT(clang-diagnostic-implicit-int-conversion)
}

void AESWrapper::GenerateRandomKey()
{
	for (int i = 0; i < 32; i++)
		Key[i] = rand() % 256; // NOLINT(clang-diagnostic-implicit-int-conversion)
}

std::vector<unsigned char> AESWrapper::DumpIV()
{
	return IV;
}

std::vector<unsigned char> AESWrapper::DumpKey()
{
	return Key;
}

void AESWrapper::SetIV(std::vector<unsigned char> iv)
{
	IV = iv;
}

void AESWrapper::SetKey(std::vector<unsigned char> key)
{
	Key = key;
}

std::vector<unsigned char> AESWrapper::Encrypt(std::vector<unsigned char> input)
{
	VM_FISH_RED_START
	using namespace CryptoPP;

	if (IV.empty() || IV.size() <= 0)
		return std::vector<unsigned char>();
	if (Key.empty() || Key.size() <= 0)
		return std::vector<unsigned char>();

	AutoSeededRandomPool prng;

	SecByteBlock key((&Key[0]), Key.size());
	SecByteBlock iv((&IV[0]), IV.size());

	std::vector<unsigned char> cipher;


	CryptoPP::AES::Encryption aesEncryption(key, 32);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::VectorSink(cipher));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(&input[0]), input.size());
	stfEncryptor.MessageEnd();

	/*try
	{
		CBC_Mode<AES>::Encryption e;
		e.SetKeyWithIV(key, key.size(), iv);

		ArraySource s(input.data(), input.size(), true,
			new StreamTransformationFilter(e,
				new VectorSink(cipher)
			)
		);
	}
	catch (Exception & e)
	{
		return std::vector<unsigned char>();
	}
*/
	VM_FISH_RED_END
	return cipher;
}

std::string AESWrapper::Decrypt(std::vector<unsigned char> input)
{
	VM_FISH_RED_START
	STR_ENCRYPT_START
	using namespace CryptoPP;
	if (IV.empty() || IV.size() <= 0)
		return "IV null";
	if (Key.empty() || Key.size() <= 0)
		return "Key null";

	AutoSeededRandomPool prng;

	SecByteBlock key((&Key[0]), Key.size());
	SecByteBlock iv((&IV[0]), IV.size());

	std::string recovered;
	STR_ENCRYPT_END
	VM_FISH_RED_END

	try
	{
		VM_FISH_RED_START
		CBC_Mode<AES>::Decryption d;
		d.SetKeyWithIV(key, key.size(), iv);

		VectorSource s(input, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			)
		);
		VM_FISH_RED_END
	}
	catch (Exception & e)
	{
		return e.what();
	}
	return recovered;
}
