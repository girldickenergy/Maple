#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"

#include "Assets/Textures.h"
#include "Assets/Fonts.h"
#include "../Configuration/ConfigManager.h"
#include "UI.h"
#include "../Utilities/Textures/TextureLoader.h"
#include "../Communication/Communication.h"

class StyleProvider
{
public:
	static inline ImFont* FontDefault;
	static inline ImFont* FontDefaultSemiBold;
	static inline ImFont* FontDefaultBold;
	static inline ImFont* FontSmall;
	static inline ImFont* FontSmallSemiBold;
	static inline ImFont* FontSmallBold;
	static inline ImFont* FontBig;
	static inline ImFont* FontBigSemiBold;
	static inline ImFont* FontBigBold;
	static inline ImFont* FontHuge;
	static inline ImFont* FontHugeSemiBold;
	static inline ImFont* FontHugeBold;

	static inline float Scale = 1;
	static inline ImVec2 MainMenuSize;
	static inline ImVec2 MainMenuSideBarSize;
	static inline float MainMenuUserInfoHeight;
	static inline float MainMenuBuildInfoHeight;
	static inline ImVec2 MapleLogoSize;
	static inline ImVec2 Padding;

	static inline ImVec4 AccentColour;
	static inline ImVec4 MenuColour;
	static inline ImVec4 ControlColour;
	static inline ImVec4 TextColour;
	static inline ImVec4 MottoColour;
	static inline ImVec4 MenuColourDark;
	static inline ImVec4 MenuColourVeryDark;
	static inline ImVec4 PanelGradientStartColour;
	static inline ImVec4 CheckMarkColour;
	static inline ImVec4 CheckMarkHoveredColour;
	static inline ImVec4 CheckMarkActiveColour;
	static inline ImVec4 SliderGrabColour;
	static inline ImVec4 SliderGrabHoveredColour;
	static inline ImVec4 SliderColour;
	static inline ImVec4 SliderHoveredColour;

	static inline void* MapleLogoTexture;
	static inline void* AvatarTexture;
	static inline void* RelaxIconTexture;
	static inline void* AimAssistIconTexture;
	static inline void* TimewarpIconTexture;
	static inline void* ReplaysIconTexture;
	static inline void* VisualsIconTexture;
	static inline void* SpooferIconTexture;
	static inline void* MiscIconTexture;
	static inline void* ConfigIconTexture;
	static inline void* ChevronIconTexture;

	static void LoadFonts()
	{
		ImGuiIO& io = ImGui::GetIO();

		FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaData, Fonts::ComfortaaSize, 16);
		FontDefaultSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaSemiBoldData, Fonts::ComfortaaSemiBoldSize, 16);
		FontDefaultBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaBoldData, Fonts::ComfortaaBoldSize, 16);

		FontSmall = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaData, Fonts::ComfortaaSize, 14);
		FontSmallSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaSemiBoldData, Fonts::ComfortaaSemiBoldSize, 14);
		FontSmallBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaBoldData, Fonts::ComfortaaBoldSize, 14);

		FontBig = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaData, Fonts::ComfortaaSize, 22);
		FontBigSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaSemiBoldData, Fonts::ComfortaaSemiBoldSize, 22);
		FontBigBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaBoldData, Fonts::ComfortaaBoldSize, 22);

		FontHuge = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaData, Fonts::ComfortaaSize, 26);
		FontHugeSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaSemiBoldData, Fonts::ComfortaaSemiBoldSize, 26);
		FontHugeBold = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ComfortaaBoldData, Fonts::ComfortaaBoldSize, 26);
	}

	static void LoadTextures()
	{
		if (Communication::GetUser()->GetDiscordID() != "-1" && Communication::GetUser()->GetAvatarHash() != "-1")
		{
			const std::string avatarURL = "https://cdn.discordapp.com/avatars/" + Communication::GetUser()->GetDiscordID() + "/" + Communication::GetUser()->GetAvatarHash() + ".png?size=64";

			AvatarTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromURLOGL3(avatarURL) : TextureLoader::LoadTextureFromURLD3D9(UI::D3D9Device, avatarURL);
		}
		else
			AvatarTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::DefaultAvatar, Textures::DefaultAvatarSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::DefaultAvatar, Textures::DefaultAvatarSize);

		if (!AvatarTexture)
			AvatarTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::DefaultAvatar, Textures::DefaultAvatarSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::DefaultAvatar, Textures::DefaultAvatarSize);

		MapleLogoTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::MapleLogo, Textures::MapleLogoSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::MapleLogo, Textures::MapleLogoSize);
		RelaxIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::RelaxIcon, Textures::RelaxIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::RelaxIcon, Textures::RelaxIconSize);
		AimAssistIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::AimAssistIcon, Textures::AimAssistIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::AimAssistIcon, Textures::AimAssistIconSize);
		TimewarpIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::TimewarpIcon, Textures::TimewarpIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::TimewarpIcon, Textures::TimewarpIconSize);
		ReplaysIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::ReplaysIcon, Textures::ReplaysIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::ReplaysIcon, Textures::ReplaysIconSize);
		VisualsIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::VisualsIcon, Textures::VisualsIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::VisualsIcon, Textures::VisualsIconSize);
		SpooferIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::SpooferIcon, Textures::SpooferIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::SpooferIcon, Textures::SpooferIconSize);
		MiscIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::MiscIcon, Textures::MiscIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::MiscIcon, Textures::MiscIconSize);
		ConfigIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::ConfigIcon, Textures::ConfigIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::ConfigIcon, Textures::ConfigIconSize);
		ChevronIconTexture = UI::Renderer == Renderer::OGL3 ? TextureLoader::LoadTextureFromMemoryOGL3(Textures::ChevronIcon, Textures::ChevronIconSize) : TextureLoader::LoadTextureFromMemoryD3D9(UI::D3D9Device, Textures::ChevronIcon, Textures::ChevronIconSize);
	}

	static void UpdateScale()
	{
		if (ConfigManager::CurrentConfig.Visuals.UI.MenuScale == 0)
			Scale = 0.5f;
		else if (ConfigManager::CurrentConfig.Visuals.UI.MenuScale == 1)
			Scale = 0.75f;
		else if (ConfigManager::CurrentConfig.Visuals.UI.MenuScale == 3)
			Scale = 1.25f;
		else if (ConfigManager::CurrentConfig.Visuals.UI.MenuScale == 4)
			Scale = 1.5f;
		else Scale = 1;

		ImGui::GetIO().FontGlobalScale = Scale;

		MainMenuSize = ImVec2(800, 600) * Scale;
		MainMenuSideBarSize = ImVec2(250, 600) * Scale;
		MainMenuUserInfoHeight = 100 * Scale;
		MainMenuBuildInfoHeight = 50 * Scale;
		MapleLogoSize = ImVec2(24, 24) * Scale;

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 0;
		style.ItemSpacing = ImVec2(10, 10) * Scale;
		style.WindowRounding = 20 * Scale;
		style.ChildRounding = 20 * Scale;
		style.FrameRounding = 10 * Scale;
		style.PopupRounding = 10 * Scale;
		style.GrabRounding = 10 * Scale;
		style.FramePadding = ImVec2(5, 5) * Scale;
		style.PopupBorderSize = 0;

		Padding = ImVec2(10, 10) * Scale;
	}

	static void UpdateColours()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		AccentColour = ConfigManager::CurrentConfig.Visuals.UI.AccentColour;
		MenuColour = ConfigManager::CurrentConfig.Visuals.UI.MenuColour;
		ControlColour = ConfigManager::CurrentConfig.Visuals.UI.ControlColour;
		TextColour = ConfigManager::CurrentConfig.Visuals.UI.TextColour;

		style.Colors[ImGuiCol_TextSelectedBg] = AccentColour;

		const float mottoColourMultiplier = 0.2126f * MenuColour.x + 0.7152f * MenuColour.y + 0.0722f * MenuColour.z > 0.5f ? -1.f : 1.f;
		const float menuColourMultiplier = MenuColour.x < 0.1f && MenuColour.y < 0.1f && MenuColour.z < 0.1f ? -1.f : 1.f;

		style.Colors[ImGuiCol_FrameBg] = ControlColour;

		//i can't think of a better way to implement this, sorry
		if (ControlColour.x > 0.85f && ControlColour.y > 0.85f && ControlColour.z > 0.85f)
		{
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(ControlColour.x - 0.1f, ControlColour.y - 0.1f, ControlColour.z - 0.1f, ControlColour.w);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(ControlColour.x - 0.05f, ControlColour.y - 0.05f, ControlColour.z - 0.05f, ControlColour.w);

			CheckMarkColour = ImVec4(ControlColour.x - 0.05f, ControlColour.y - 0.05f, ControlColour.z - 0.05f, ControlColour.w);
			CheckMarkHoveredColour = ImVec4(ControlColour.x - 0.15f, ControlColour.y - 0.15f, ControlColour.z - 0.15f, ControlColour.w);
			CheckMarkActiveColour = ControlColour;

			SliderColour = ControlColour;
			SliderHoveredColour = ImVec4(ControlColour.x - 0.15f, ControlColour.y - 0.15f, ControlColour.z - 0.15f, ControlColour.w);
			SliderGrabColour = ImVec4(ControlColour.x - 0.1f, ControlColour.y - 0.1f, ControlColour.z - 0.1f, ControlColour.w);
			SliderGrabHoveredColour = ImVec4(ControlColour.x - 0.2f, ControlColour.y - 0.2f, ControlColour.z - 0.2f, ControlColour.w);
		}
		else if (ControlColour.x < 0.05f && ControlColour.y < 0.05f && ControlColour.z < 0.05f)
		{
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(ControlColour.x + 0.1f, ControlColour.y + 0.1f, ControlColour.z + 0.1f, ControlColour.w);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(ControlColour.x + 0.05f, ControlColour.y + 0.05f, ControlColour.z + 0.05f, ControlColour.w);

			CheckMarkColour = ImVec4(ControlColour.x + 0.05f, ControlColour.y + 0.05f, ControlColour.z + 0.05f, ControlColour.w);
			CheckMarkHoveredColour = ImVec4(ControlColour.x + 0.15f, ControlColour.y + 0.15f, ControlColour.z + 0.15f, ControlColour.w);
			CheckMarkActiveColour = ControlColour;

			SliderColour = ControlColour;
			SliderHoveredColour = ImVec4(ControlColour.x + 0.15f, ControlColour.y + 0.15f, ControlColour.z + 0.15f, ControlColour.w);
			SliderGrabColour = ImVec4(ControlColour.x + 0.1f, ControlColour.y + 0.1f, ControlColour.z + 0.1f, ControlColour.w);
			SliderGrabHoveredColour = ImVec4(ControlColour.x + 0.2f, ControlColour.y + 0.2f, ControlColour.z + 0.2f, ControlColour.w);
		}
		else
		{
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(ControlColour.x + 0.05f, ControlColour.y + 0.05f, ControlColour.z + 0.05f, ControlColour.w);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(ControlColour.x - 0.05f, ControlColour.y - 0.05f, ControlColour.z - 0.05f, ControlColour.w);

			CheckMarkColour = ImVec4(ControlColour.x + 0.05f, ControlColour.y + 0.05f, ControlColour.z + 0.05f, ControlColour.w);
			CheckMarkHoveredColour = ImVec4(ControlColour.x + 0.1f, ControlColour.y + 0.1f, ControlColour.z + 0.1f, ControlColour.w);
			CheckMarkActiveColour = ControlColour;

			SliderColour = ControlColour;
			SliderHoveredColour = ImVec4(ControlColour.x + 0.1f, ControlColour.y + 0.1f, ControlColour.z + 0.1f, ControlColour.w);
			SliderGrabColour = ImVec4(ControlColour.x + 0.05f, ControlColour.y + 0.05f, ControlColour.z + 0.05f, ControlColour.w);
			SliderGrabHoveredColour = ImVec4(ControlColour.x + 0.15f, ControlColour.y + 0.15f, ControlColour.z + 0.15f, ControlColour.w);
		}

		MottoColour = ImVec4(MenuColour.x + 0.2f * mottoColourMultiplier, MenuColour.y + 0.2f * mottoColourMultiplier, MenuColour.z + 0.2f * mottoColourMultiplier, MenuColour.w);
		MenuColourDark = ImVec4(MenuColour.x - 0.05f * menuColourMultiplier, MenuColour.y - 0.05f * menuColourMultiplier, MenuColour.z - 0.05f * menuColourMultiplier, MenuColour.w);
		MenuColourVeryDark = ImVec4(MenuColour.x - 0.1f * menuColourMultiplier, MenuColour.y - 0.1f * menuColourMultiplier, MenuColour.z - 0.1f * menuColourMultiplier, MenuColour.w);
		PanelGradientStartColour = ImColor(MenuColourVeryDark.x + 0.025f * menuColourMultiplier, MenuColourVeryDark.y + 0.025f * menuColourMultiplier, MenuColourVeryDark.z + 0.025f * menuColourMultiplier, MenuColourVeryDark.w);

		style.Colors[ImGuiCol_WindowBg] = MenuColour;
		style.Colors[ImGuiCol_ChildBg] = MenuColourDark;

		style.Colors[ImGuiCol_PopupBg] = MenuColour;

		style.Colors[ImGuiCol_TitleBg] = MenuColourVeryDark;
		style.Colors[ImGuiCol_TitleBgActive] = MenuColourVeryDark;
		style.Colors[ImGuiCol_TitleBgCollapsed] = MenuColourVeryDark;

		style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_FrameBg];
		style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_FrameBgHovered];
		style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_FrameBgActive];

		style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_FrameBg];
		style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_FrameBgHovered];
		style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_FrameBgActive];

		style.Colors[ImGuiCol_Text] = TextColour;
	}
};