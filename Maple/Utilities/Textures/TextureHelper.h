#pragma once

#include <d3d9.h>

class TextureHelper
{
	static void* loadTextureInternalOGL3(unsigned char* data, int width, int height);
public:
	static unsigned char* LoadIMG(const char* path, int* x, int* y, int* channels_in_file, int desired_channels);
	static void FreeIMG(void* data);
	static void* LoadTextureFromFileOGL3(const char* path);
	static void* LoadTextureFromMemoryOGL3(const unsigned char* data, int size);
	static void FreeTextureOGL3(void* textureId);
	static void* LoadTextureFromFileD3D9(IDirect3DDevice9* d3d9Device, const char* filepath);
	static void* LoadTextureFromMemoryD3D9(IDirect3DDevice9* d3d9Device, LPCVOID data, int size);
};