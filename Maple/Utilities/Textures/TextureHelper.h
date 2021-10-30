#pragma once

#include <d3d9.h>
#include <string>

class TextureHelper
{
	static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
	{
		static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
		return size * nmemb;
	}
	
	static void* loadTextureInternalOGL3(unsigned char* data, int width, int height);
public:
	static unsigned char* LoadIMG(const char* path, int* x, int* y, int* channels_in_file, int desired_channels);
	static void FreeIMG(void* data);
	static void* LoadTextureFromFileOGL3(const char* path);
	static void* LoadTextureFromMemoryOGL3(const unsigned char* data, int size);
	static void* LoadTextureFromURLOGL3(const std::string& url);
	static void FreeTextureOGL3(void* textureId);
	static void* LoadTextureFromFileD3D9(IDirect3DDevice9* d3d9Device, const char* filepath);
	static void* LoadTextureFromMemoryD3D9(IDirect3DDevice9* d3d9Device, LPCVOID data, int size);
	static void* LoadTextureFromURLD3D9(IDirect3DDevice9* d3d9Device, const std::string& url);
};
