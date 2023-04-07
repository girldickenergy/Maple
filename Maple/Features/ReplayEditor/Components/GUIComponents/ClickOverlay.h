#pragma once
#include <imgui.h>
#include <Math/sRectangle.h>
#include "../../UI/StyleProvider.h"
#include "../../EditorGlobals.h"
#include "Click.h"

namespace ReplayEditor
{
	/// @class ClickOverlay
	/// @brief Class that houses and creates the keypress overlay that shows the current keypress like in osu!.
	class ClickOverlay
	{
		/// @brief Pointer to the time integer from the editor instance.
		int* _timer;
		
		/// @brief Pointer to the vector containing all clicks parsed by the click timeline.
		std::vector<Click>* _clicks;

		/// @brief Pointer to the drawlist from the editor imgui instance.
		ImDrawList* _drawList;

		/// @brief Pointer to the Vector2 containing the osu! size.
		Vector2* _clientBounds;
	public:
		/// @brief A bool value that lets the rendering function know when to render the click overlay.
		bool _isInit;

		/// @brief The current pressed keys determined by time.
		OsuKeys _currentOsuKeys;

		/// @brief Empty constructor for first initialization.
		ClickOverlay();

		/// @brief Constructor for the click/keypress overlay in osu! style.
		/// @param timer Pointer to the time integer from the editor instance.
		/// @param clicks Pointer to the vector containing all clicks parsed by the click timeline.
		/// @param drawList Pointer to the drawlist from the editor imgui instance.
		/// @param clientBounds Pointer to the Vector2 containing the osu! size.
		ClickOverlay(int* timer, std::vector<Click>* clicks, ImDrawList* drawList, Vector2* clientBounds);

		/// @brief Renders the click overlay to the screen context.
		void Render();
	};
}