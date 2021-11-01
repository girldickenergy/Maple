#pragma once
#include <stack>
#include "Components/HitObjectOsu.h"

namespace ReplayEditor
{
	class UI
	{
		/*
			Stack drawing design (stack is faster than vector):  
			- Push element onto stack
			- Loop through elements
			- Pop element from stack
			- Draw this
		*/
		static inline std::stack<Drawable> drawables = std::stack<Drawable>();
	public:
		static void DrawUI();
	};
}