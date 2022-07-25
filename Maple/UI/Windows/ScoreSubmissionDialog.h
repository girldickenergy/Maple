#pragma once

class ScoreSubmissionDialog
{
	static inline bool isVisible = false;
	static inline bool resetPosition = true;
public:
	static void Render();

	static void Show();
	static void Hide();
	static bool GetIsVisible();
};