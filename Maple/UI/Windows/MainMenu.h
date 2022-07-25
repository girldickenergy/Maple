#pragma once

class MainMenu
{
	static inline int currentTab = -1;
	static inline void* backgroundTexture = nullptr;

	static void updateBackground();
public:
	static void Render();
};