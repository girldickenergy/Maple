#pragma once
#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include "../base64.h"
#include "../rsa.h"
#include "../osrng.h"
#include "../hex.h"
#include "../files.h"
#include "../pssr.h"
#include "../modes.h"
#include "ThemidaSDK.h"

//using genBytes = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;

class AESWrapper {
private:
	std::vector<unsigned char> IV = std::vector<unsigned char>(16);
	std::vector<unsigned char> Key = std::vector<unsigned char>(32);
public:
	AESWrapper();
	void GenerateRandomIV();
	void GenerateRandomKey();

	std::vector<unsigned char> DumpIV();
	std::vector<unsigned char> DumpKey();

	void SetIV(std::vector<unsigned char> iv);
	void SetKey(std::vector<unsigned char> key);

	std::vector<unsigned char> Encrypt(std::vector<unsigned char> input);
	std::string Decrypt(std::vector<unsigned char> input);
};