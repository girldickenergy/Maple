#pragma once
#include "../../../../../Dependencies/Chiyo/Replays/Replay.h"
#include "../../../EditorGlobals.h"
#include <imgui.h>

namespace ReplayEditor
{
	/// @class OsuCursor
	/// @brief Class wrapper that handles all the logic and drawing for the Replay Cursor.
	class OsuCursor
	{
		/// @brief Pointer to the CurrentFrame int taken from the Editor instance.
		int* _currentFrame;

		/// @brief Pointer to the DrawList taken from the Editor window created from the Editor instance.
		ImDrawList* _drawList;

		/// @brief Renders the cursor trail on screen
		void RenderTrail();

		/// @brief Renders the actual cursor on screen
		void RenderCursor();
	public:
		/// @brief Pointer to the currently pressed keys from the click overlay instance.
		OsuKeys* _currentPressedKeys;

		/// @brief Empty constructor for first initialization.
		OsuCursor();

		/// @brief Constructor for constructing a OsuCursor instance to handle Cursor drawing.
		/// @param currentFrame Pointer to the CurrentFrame int from the Editor instance.
		/// @param drawList Pointer to the DrawList from the Editor window.
		/// @param currentPressedKeys Pointer to the currently pressed keys from the click overlay instance.
		OsuCursor(int* currentFrame, ImDrawList* drawList, OsuKeys* currentPressedKeys = nullptr);

		/// @brief Renders the cursor and trail on screen
		void Render();
	};
}