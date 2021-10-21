#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui.h>

#include "Fonts.h"
#include "Textures.h"
#include "../Utilities/Textures/TextureHelper.h"
#include "Overlay.h"
#include "../Config/Config.h"

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
	static inline ImVec2 MapleLogoSize;
	static inline ImVec2 Padding;

	static inline ImVec4 AccentColour;
    static inline ImVec4 MenuColour;
    static inline ImVec4 ControlColour;
    static inline ImVec4 TextColour;
	static inline ImVec4 MottoColour;
    static inline ImVec4 MenuColourDark;
    static inline ImVec4 MenuColourVeryDark;

	static inline void* MapleLogoTexture;
	static inline void* AvatarTexture;
	static inline void* RelaxIconTexture;
	static inline void* AimAssistIconTexture;
	static inline void* TimewarpIconTexture;
	static inline void* VisualsIconTexture;
	static inline void* MiscIconTexture;
	static inline void* ConfigIconTexture;

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
		MapleLogoTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::MapleLogo, Textures::MapleLogoSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::MapleLogo, Textures::MapleLogoSize);
		//AvatarTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromFileOGL3("C:\\Users\\Flash\\Desktop\\a_f0b6abaa98e5a0c64e0d1fabdeb5926b.png") : TextureHelper::LoadTextureFromFileD3D9(Overlay::D3D9Device, "C:\\Users\\Flash\\Desktop\\a_f0b6abaa98e5a0c64e0d1fabdeb5926b.png");
		AvatarTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::DefaultAvatar, Textures::DefaultAvatarSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::DefaultAvatar, Textures::DefaultAvatarSize);
		RelaxIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::RelaxIcon, Textures::RelaxIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::RelaxIcon, Textures::RelaxIconSize);
		AimAssistIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::AimAssistIcon, Textures::AimAssistIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::AimAssistIcon, Textures::AimAssistIconSize);
		TimewarpIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::TimewarpIcon, Textures::TimewarpIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::TimewarpIcon, Textures::TimewarpIconSize);
		VisualsIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::VisualsIcon, Textures::VisualsIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::VisualsIcon, Textures::VisualsIconSize);
		MiscIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::MiscIcon, Textures::MiscIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::MiscIcon, Textures::MiscIconSize);
		ConfigIconTexture = Overlay::Renderer == Renderer::OGL3 ? TextureHelper::LoadTextureFromMemoryOGL3(Textures::ConfigIcon, Textures::ConfigIconSize) : TextureHelper::LoadTextureFromMemoryD3D9(Overlay::D3D9Device, Textures::ConfigIcon, Textures::ConfigIconSize);
	}

	static void UpdateScale()
	{
		if (Config::Visuals::MenuScale == 0)
			Scale = 0.5f;
		else if (Config::Visuals::MenuScale == 1)
			Scale = 0.75f;
		else if (Config::Visuals::MenuScale == 3)
			Scale = 1.25f;
		else if (Config::Visuals::MenuScale == 4)
			Scale = 1.5f;
		else Scale = 1;

		ImGui::GetIO().FontGlobalScale = Scale;

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
		style.PopupBorderSize = 0;

		Padding = ImVec2(10, 10) * Scale;
	}

	static void UpdateColours()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		AccentColour = Config::Visuals::AccentColour;
		MenuColour = Config::Visuals::MenuColour;
		ControlColour = Config::Visuals::ControlColour;
		TextColour = Config::Visuals::TextColour;

		const float mottoColourMultiplier = 0.2126f * MenuColour.x + 0.7152f * MenuColour.y + 0.0722f * MenuColour.z > 0.5f ? -1.f : 1.f;
		const float menuColourMultiplier = MenuColour.x < 0.1f && MenuColour.y < 0.1f && MenuColour.z < 0.1f ? -1.f : 1.f;
		const float controlColourMultiplier = (ControlColour.x < 0.05f && ControlColour.y < 0.05f && ControlColour.z < 0.05f) || (ControlColour.x > 0.95f && ControlColour.y > 0.95f && ControlColour.z > 0.95f) ? -1.f : 1.f;

		
		MottoColour = ImVec4(MenuColour.x + 0.2f * mottoColourMultiplier, MenuColour.y + 0.2f * mottoColourMultiplier, MenuColour.z + 0.2f * mottoColourMultiplier, MenuColour.w);
		MenuColourDark = ImVec4(MenuColour.x - 0.05f * menuColourMultiplier, MenuColour.y - 0.05f * menuColourMultiplier, MenuColour.z - 0.05f * menuColourMultiplier, MenuColour.w);
		MenuColourVeryDark = ImVec4(MenuColour.x - 0.1f * menuColourMultiplier, MenuColour.y - 0.1f * menuColourMultiplier, MenuColour.z - 0.1f * menuColourMultiplier, MenuColour.w);
		
		style.Colors[ImGuiCol_WindowBg] = MenuColour;
		style.Colors[ImGuiCol_ChildBg] = MenuColourDark;
		
		style.Colors[ImGuiCol_FrameBg] = ControlColour;
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(ControlColour.x + 0.05f * controlColourMultiplier, ControlColour.y + 0.05f * controlColourMultiplier, ControlColour.z + 0.05f * controlColourMultiplier, ControlColour.w);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(ControlColour.x - 0.05f * controlColourMultiplier, ControlColour.y - 0.05f * controlColourMultiplier, ControlColour.z - 0.05f * controlColourMultiplier, ControlColour.w);

		style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_FrameBg];
		style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_FrameBgHovered];
		style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_FrameBgActive];

		style.Colors[ImGuiCol_Text] = TextColour;
	}
};
