#include "TextureHelper.h"

#include <d3dx9tex.h>
#pragma comment(lib, "D3dx9")

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/gl3w.h>

#include <WinSock2.h>
#include <curl.h>
#include <vector>

#include "../Strings/StringUtilities.h"

void* TextureHelper::loadTextureInternalOGL3(unsigned char* data, int width, int height)
{
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	return (void*)tex;
}

unsigned char* TextureHelper::LoadIMG(const char* path, int* x, int* y, int* channels_in_file, int desired_channels)
{
	return stbi_load(path, x, y, channels_in_file, desired_channels);
}

void TextureHelper::FreeIMG(void* data)
{
	stbi_image_free(data);
}

void* TextureHelper::LoadTextureFromFileOGL3(const char* path)
{
	int width = 0;
	int height = 0;
	unsigned char* data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL)
		return nullptr;

	return loadTextureInternalOGL3(data, width, height);
}

void* TextureHelper::LoadTextureFromMemoryOGL3(const unsigned char* data, int size)
{
	int width, height, n;
	unsigned char* textureData = stbi_load_from_memory(data, size, &width, &height, &n, 4);
	if (data == NULL)
		return nullptr;

	return loadTextureInternalOGL3(textureData, width, height);
}

void* TextureHelper::LoadTextureFromURLOGL3(const std::string& url)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::string readBuffer;

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			std::vector<unsigned char> bytes = StringUtilities::StringToByteArray(readBuffer);
			return LoadTextureFromMemoryOGL3(bytes.data(), bytes.size());
		}
	}

	return nullptr;
}

void TextureHelper::FreeTextureOGL3(void* textureId)
{
	GLuint texID = (GLuint)textureId;
	glDeleteTextures(1, &texID);
}

void* TextureHelper::LoadTextureFromFileD3D9(IDirect3DDevice9* d3d9Device, const char* filepath)
{
	PDIRECT3DTEXTURE9 texture;
	HRESULT hr = D3DXCreateTextureFromFileA(d3d9Device, filepath, &texture);

	if (hr != S_OK)
		return nullptr;

	return texture;
}

void* TextureHelper::LoadTextureFromMemoryD3D9(IDirect3DDevice9* d3d9Device, LPCVOID data, int size)
{
	IDirect3DTexture9* ret;
	HRESULT hr = D3DXCreateTextureFromFileInMemory(d3d9Device, data, size, &ret);

	return hr == S_OK ? ret : nullptr;
}

void* TextureHelper::LoadTextureFromURLD3D9(IDirect3DDevice9* d3d9Device, const std::string& url)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::string readBuffer;

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			std::vector<unsigned char> bytes = StringUtilities::StringToByteArray(readBuffer);
			return LoadTextureFromMemoryD3D9(d3d9Device, bytes.data(), bytes.size());
		}
	}

	return nullptr;
}
