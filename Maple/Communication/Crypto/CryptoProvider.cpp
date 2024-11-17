#include "CryptoProvider.h"

#include "base64.h"
#include "modes.h"
#include "osrng.h"
#include "pem.h"
#include "VirtualizerSDK.h"
#include "xorstr.hpp"
#include <random>
#include "../../Utilities/Security/Security.h"
#include <bit>

[[clang::optnone]] CryptoProvider::CryptoProvider(singletonLock)
{
    VIRTUALIZER_FISH_RED_START
    
    m_XorKey = xorstr_("xjCFQ58Pqd8KPNHp");

    const std::string rsaPrivateKeyStr =
        xorstr_("-----BEGIN PRIVATE KEY-----\n"
        "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC3vUslDlitRYQl\n"
        "WmFZLPLssI21Y1gCwlQpv0ORajzzpGe1MjJUWd1ighMCxPb5S2HzsD46bMbaKHBp\n"
        "RAf3UfOCmMpsCpwOrV9WSv11Lq0iLzr83HdAfSyXE3v3p+3OzaXJa0+5DokpDdfT\n"
        "BK6wQ+1rbPLR+vHGX1uz+30LG6gLH/gokKuEDDGCg50pQDxNd+oyZBhgj1hMy+ll\n"
        "knF5EjYbFwJfuF/EdmG/8zXEFCIz9l9zTDt5Ck5IUwWQZUojifFlID7S3D2q7qAL\n"
        "vz5ZAbPQv75+M504Sx2P+ikiIJ3EAsN2dF3x1szM892qNC9w5mKRbSPVd/6gBXEl\n"
        "Z1qEvD4fAgMBAAECggEACS5U8sQRLfz/rmRrT2r7a1SQ1Lh1Wzg5LTxGYnlQXYOy\n"
        "0TO7DFzQmmR/f4rtbgoUYwyybQgUbMA4QSgA8eW81seX+3k19Q04ar8AOuhSBqn4\n"
        "pLvrbq0SiJqUx1Wg7jsuYfe2mfJ5kFiYBKCVIEsQDYuw0m4SVBsUj0HkLKU0Ias7\n"
        "+EwErf/82g2yi+Cb/BJ3hbSXarjEySyZYvPmDUHplIAEeYZPpVUveidLTi03dzqM\n"
        "mKe03hJEWS9taDDHpOuGEYwni8GCGWhUcuDMaRiMb0iAufP9wLDgK9fneSNyzqlo\n"
        "rQwpjkV24UwkNToamrWC1NY/OnV0fw1mBvAVyCAccQKBgQDqDasDgl4j12wnnbAm\n"
        "AOjMz+5VhlPKKwWfb+RU+H0wsJdLhpC9KcvnnIyigvYHmn3HCrD5FNuqOqTi5I97\n"
        "YmBPcYjKZ7RIsUghL+0TrnttpV85YIP9aWR7Y/BI7dLblhb7XEaMTSqchsTQm8ZV\n"
        "2CokTKAUoh8KuGF8yadMuPhQCQKBgQDI991TsCW0s8VRTilA35Tl3feca/3hW+gP\n"
        "rSUo9A9pLe8JfhCBI8Oem7UJuCXqdCWkrWBoottOprghBqMm8q3FbRddG2+ZQq09\n"
        "sHhxs6QBi48n86wj+13iEu1q8zOS/3UHtkXqpcmun4kuNQjCyMJVBh7+GnIVeBq9\n"
        "jc6Jw8FW5wKBgFc50Mn2A++HkR8SfVssa5Yox8HH1eI6NlY89PNVyBIlPb0/4SEe\n"
        "OoeOqDrIY7hhsw5CxcGOLG88e8azN2XcO9hfj4MfMukEaMU1fz0egZFTWDGh7o66\n"
        "gGAb1qjXOQXs4ZO7x+7xszeyYhb8duX3MX1lsSL/aGIuZOaEoHzPUntBAoGBAKjK\n"
        "pfYYSFMRenTjdPqrzt+SItoaSc3x52LjRgazlg39KnuKikosw6x/kvpq1E2q58L8\n"
        "a7D2t8PDe3shm/6mbRUYnzrwRDcxLVhW05KMEv/JO5+OpdJC2YN76jSdt3+LV7v2\n"
        "d5qxTEeCbzGEEaun+rB2x6kchR1LFwqGBCbpZ4WZAoGANHf3B3LBTos17tAAxxxj\n"
        "ta1elq2vD3CGNmXO0Hh5aagii6VE5X2FBB2bKwJJgqmQdn9iywUWhSVmDSbVR8Er\n"
        "EwvtX6RqdXYpDOhxV+m0QH52a2mZ2thVN48t5aC2ir7Kv3T0FDbZo2istd9ZiHJF\n"
        "iOyKYKWa5FQasKJD0qnaZqA=\n"
        "-----END PRIVATE KEY-----\n");

    CryptoPP::StringSource source(rsaPrivateKeyStr, true);
    CryptoPP::PEM_Load(source, m_RsaPrivateKey);

    VIRTUALIZER_FISH_RED_END
}

void CryptoProvider::InitializeAES(const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    VIRTUALIZER_FISH_RED_START

    m_AesKeyBlock = CryptoPP::SecByteBlock((&key[0]), key.size());
    m_AesIvBlock = CryptoPP::SecByteBlock((&iv[0]), iv.size());

    VIRTUALIZER_FISH_RED_END
}

std::vector<unsigned char> CryptoProvider::XOR(const std::vector<unsigned char>& data)
{
    VIRTUALIZER_FISH_RED_START

    std::vector<unsigned char> result = data;

    unsigned int j = 0;
    for (unsigned int i = 0; i < data.size(); i++)
    {
        result[i] = data[i] ^ m_XorKey[j];

        j = (++j < m_XorKey.length() ? j : 0);
    }

    VIRTUALIZER_FISH_RED_END

    return result;
}

std::string CryptoProvider::Base64Encode(const std::vector<unsigned char>& data)
{
    VIRTUALIZER_FISH_RED_START

    std::string encoded;

    CryptoPP::VectorSource ss(data, true,
        new CryptoPP::Base64Encoder(
            new CryptoPP::StringSink(encoded), false
        )
    );

    VIRTUALIZER_FISH_RED_END

    return encoded;
}

std::vector<unsigned char> CryptoProvider::Base64Decode(const std::string& encoded)
{
    VIRTUALIZER_FISH_RED_START

    std::vector<unsigned char> decoded;

    CryptoPP::StringSource ss(encoded, true,
        new CryptoPP::Base64Decoder(
            new CryptoPP::VectorSink(decoded)
        )
    );

    VIRTUALIZER_FISH_RED_END

    return decoded;
}

std::vector<unsigned char> CryptoProvider::RSADecrypt(const std::vector<unsigned char>& ciphertext)
{
    VIRTUALIZER_FISH_RED_START

    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(m_RsaPrivateKey);

    size_t dpl = decryptor.MaxPlaintextLength(ciphertext.size());
    CryptoPP::SecByteBlock recovered(dpl);

    auto result = decryptor.Decrypt(rng, ciphertext.data(), ciphertext.size(), recovered);

    VIRTUALIZER_FISH_RED_END

    return std::vector(recovered.begin(), recovered.begin() + result.messageLength);
}

std::vector<unsigned char> CryptoProvider::AESEncrypt(const std::vector<unsigned char>& cleartext)
{
    VIRTUALIZER_FISH_RED_START

    if (m_AesKeyBlock.empty() || m_AesKeyBlock.size() <= 0)
        return {};

    if (m_AesIvBlock.empty() || m_AesIvBlock.size() <= 0)
        return {};

    CryptoPP::AutoSeededRandomPool prng;

    std::vector<unsigned char> cipher;

    CryptoPP::AES::Encryption aesEncryption(m_AesKeyBlock, 32);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, m_AesIvBlock);

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::VectorSink(cipher));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(&cleartext[0]), cleartext.size());
    stfEncryptor.MessageEnd();

    VIRTUALIZER_FISH_RED_END

    return cipher;
}

std::vector<unsigned char> CryptoProvider::AESDecrypt(const std::vector<unsigned char>& ciphertext)
{
    VIRTUALIZER_FISH_RED_START

    if (m_AesKeyBlock.empty() || m_AesKeyBlock.size() <= 0)
        return {};

    if (m_AesIvBlock.empty() || m_AesIvBlock.size() <= 0)
        return {};

    CryptoPP::AutoSeededRandomPool prng;

    std::vector<unsigned char> recovered;

    VIRTUALIZER_FISH_RED_END

    try
    {
        VIRTUALIZER_FISH_RED_START

        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV(m_AesKeyBlock, m_AesKeyBlock.size(), m_AesIvBlock);

        CryptoPP::VectorSource s(ciphertext, true,
            new CryptoPP::StreamTransformationFilter(d,
                new CryptoPP::VectorSink(recovered)
            )
        );

        VIRTUALIZER_FISH_RED_END
    }
    catch (CryptoPP::Exception& e)
    {
        return {};
    }

    return recovered;
}

std::vector<uint8_t> CryptoProvider::ApplyCryptoTransformations(const std::vector<uint8_t>& buffer, uint32_t key1, uint32_t key2, uint32_t key3, bool reverse)
{
    VIRTUALIZER_MUTATE_ONLY_START

    auto transformedBuffer = std::vector<uint8_t>(buffer);
    std::vector<CryptoTransformation> transformations = GenerateCryptoTransformations(key1 ^ key2 ^ key3, transformedBuffer.size());
    key1 ^= key3;
    key2 ^= key3;

    size_t startingPosition = reverse ? (transformedBuffer.size() - 1) : 0;
    for (auto i = startingPosition; reverse ? i >= 0 : i < transformedBuffer.size(); i += reverse ? -1 : 1)
    {
        if (reverse)
            transformedBuffer[i] ^= (i > 0 ? transformedBuffer[i - 1] : 0);

        switch (transformations[i])
        {
            case Xor:
                transformedBuffer[i] ^= (key1 ^ key2);
                break;
            case Rol:
                transformedBuffer[i] ^= std::rotl(key1 ^ key2, (i + 1) % 16);
                break;
            case Ror:
                transformedBuffer[i] ^= std::rotr(key1 ^ key2, 16 - ((i + 1) % 16));
                break;
            case Rolr:
                transformedBuffer[i] ^= std::rotl(key1, (i + 1) % 16) ^ std::rotr(key2, 16 - ((1 + 1) % 16));
                break;
            case Add:
                if (reverse)
                    transformedBuffer[i] -= key1 ^ key2;
                else
                    transformedBuffer[i] += key1 ^ key2;
                break;
            case Sub:
                if (reverse)
                    transformedBuffer[i] += key1 ^ key2;
                else
                    transformedBuffer[i] -= key1 ^ key2;
                break;
            case Cancer:
                transformedBuffer[i] ^= key1 ^ key2 ^ 0xdeadbeef ^ std::rotl(key3, (i + 1) % 16) ^ std::rotr(key3, 16 - ((i + 1) % 16));
                break;
            default:
                Security::CorruptMemory();
                break;
        }

        if (!reverse)
            transformedBuffer[i] ^= i > 0 ? transformedBuffer[i - 1] : 0;
    }

    return transformedBuffer;

    VIRTUALIZER_MUTATE_ONLY_END
}

std::vector<CryptoTransformation> CryptoProvider::GenerateCryptoTransformations(uint32_t seed, size_t size)
{
    VIRTUALIZER_MUTATE_ONLY_START

    auto transformations = std::vector<CryptoTransformation>(size);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(CryptoTransformation::Cancer));

    for (size_t i = 0; i < size; i++)
    {
        transformations[i] = static_cast<CryptoTransformation>(dist(rng));
    }

    return transformations;

    VIRTUALIZER_MUTATE_ONLY_END
}