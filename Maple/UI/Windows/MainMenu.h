#pragma once

class MainMenu
{
	static inline bool isVisible = false;
	static inline int currentTab = -1;
	static inline void* backgroundTexture = nullptr;

	static void updateBackground();
public:
	static void Render();

	static void Show();
	static void Hide();
	static void ToggleVisibility();
	static bool GetIsVisible();
};