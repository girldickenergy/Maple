#include "MainMenu.h"

#include <filesystem>
#include <fstream>

#include "imgui.h"
#include "xorstr.hpp"

#include "../StyleProvider.h"
#include "../Widgets/Widgets.h"
#include "../../Storage/Storage.h"
#include "../Widgets/3rd-party/FileDialog/imfilebrowser.h"
#include "../../Storage/StorageConfig.h"
#include "../../Features/Spoofer/Spoofer.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Utilities/Clipboard/ClipboardUtilities.h"
#include "../../Logging/Logger.h"
#include "../../Utilities/Strings/StringUtilities.h"
#include "../../Configuration/ConfigManager.h"

bool backgroundImageDialogInitialized = false;
ImGui::FileBrowser backgroundImageDialog;

bool replayDialogInitialized = false;
ImGui::FileBrowser replayDialog;

void MainMenu::updateBackground()
{
    char menuBackground[ConfigManager::CurrentConfig.Visuals.UI.MenuBackground.GetSize()];
    ConfigManager::CurrentConfig.Visuals.UI.MenuBackground.GetData(menuBackground);

    if (menuBackground[0] == '\0' || !std::filesystem::exists(std::string(menuBackground)))
    {
        if (backgroundTexture != nullptr)
        {
            if (UI::Renderer == Renderer::OGL3)
                TextureLoader::FreeTextureOGL3(backgroundTexture);

            backgroundTexture = nullptr;
        }

        return;
    }

    if (UI::Renderer == Renderer::OGL3)
		backgroundTexture = TextureLoader::LoadTextureFromFileOGL3(menuBackground);
	else
		backgroundTexture = TextureLoader::LoadTextureFromFileD3D9(UI::D3D9Device, menuBackground);
}

void MainMenu::Render()
{
    if (!isVisible)
        return;

	const ImGuiIO& io = ImGui::GetIO();
	const ImGuiStyle& style = ImGui::GetStyle();

    if (backgroundTexture != nullptr)
        ImGui::GetBackgroundDrawList()->AddImage(backgroundTexture, ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    const Vector2 clientPos = GameBase::GetClientPosition();
    const Vector2 clientSize = GameBase::GetClientSize();

    const bool expanded = currentTab != -1;
    ImGui::SetNextWindowSize(expanded ? StyleProvider::MainMenuSize : StyleProvider::MainMenuSideBarSize);
    ImGui::SetNextWindowPos(ImVec2(clientSize.X / 2 - StyleProvider::MainMenuSize.x / 2, clientSize.Y / 2 - StyleProvider::MainMenuSize.y / 2), ImGuiCond_Once);
    ImGui::Begin(StringUtilities::GenerateRandomString(16, xorstr_("Main Menu")).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuSize = ImGui::GetCurrentWindow()->Size;
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + StyleProvider::MainMenuSideBarSize, ImColor(StyleProvider::MenuColourDark), style.WindowRounding, expanded ? ImDrawFlags_RoundCornersAll & ~ImDrawFlags_RoundCornersTopRight : ImDrawFlags_RoundCornersAll);

        ImGui::SetCursorPos(StyleProvider::Padding);
        ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("Side Bar")).c_str(), StyleProvider::MainMenuSideBarSize - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
        {
            const ImVec2 sideBarSize = ImGui::GetCurrentWindow()->Size;

            ImGui::PushFont(StyleProvider::FontHugeBold);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5 * StyleProvider::Scale, 10 * StyleProvider::Scale));
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ((ImGui::CalcTextSize(xorstr_("Maple")).x / 2) + StyleProvider::MapleLogoSize.x / 2 + style.ItemSpacing.x / 2));
            ImGui::Image(StyleProvider::MapleLogoTexture, StyleProvider::MapleLogoSize, ImVec2(0, 0), ImVec2(1, 1), StyleProvider::AccentColour);
            ImGui::SameLine();
            ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Maple"));
            ImGui::PopStyleVar();
            ImGui::PopFont();
            ImGui::PushFont(StyleProvider::FontSmall);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style.ItemSpacing.y);
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ImGui::CalcTextSize(xorstr_("the quickest way to the top")).x / 2);
            ImGui::TextColored(StyleProvider::MottoColour, xorstr_("the quickest way to the top"));
            ImGui::PopFont();

            ImGui::Spacing();

            ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("User Info")).c_str(), ImVec2(sideBarSize.x, StyleProvider::MainMenuUserInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 userInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 userInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(userInfoPos, userInfoPos + userInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::GetWindowDrawList()->AddImageRounded(StyleProvider::AvatarTexture, userInfoPos + ImVec2(userInfoSize.y / 4, userInfoSize.y / 4), userInfoPos + ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2, userInfoSize.y / 4 + userInfoSize.y / 2), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), style.FrameRounding);

                ImGui::PushFont(StyleProvider::FontDefaultBold);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 - style.ItemSpacing.y / 4 - ImGui::CalcTextSize("Welcome back").y));
                ImGui::Text(xorstr_("Welcome back"));
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontDefault);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::AccentColour, Communication::GetUser()->GetUsername().c_str());
                ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("Tabs")).c_str(), ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (40 * StyleProvider::Scale) * 8; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("Tabs##001")).c_str(), ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 40 * StyleProvider::Scale);

                    ImGui::PushFont(StyleProvider::FontDefaultBold);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                    if (Widgets::Tab(xorstr_("Relax"), StyleProvider::RelaxIconTexture, currentTab == 0, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 0 ? -1 : 0;

                    if (Widgets::Tab(xorstr_("Aim Assist"), StyleProvider::AimAssistIconTexture, currentTab == 1, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 1 ? -1 : 1;

                    if (Widgets::Tab(xorstr_("Timewarp"), StyleProvider::TimewarpIconTexture, currentTab == 2, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 2 ? -1 : 2;

                    if (Widgets::Tab(xorstr_("Replays"), StyleProvider::ReplaysIconTexture, currentTab == 3, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 3 ? -1 : 3;

                    if (Widgets::Tab(xorstr_("Visuals"), StyleProvider::VisualsIconTexture, currentTab == 4, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;

                    if (Widgets::Tab(xorstr_("Spoofer"), StyleProvider::SpooferIconTexture, currentTab == 5, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;

                    if (Widgets::Tab(xorstr_("Misc"), StyleProvider::MiscIconTexture, currentTab == 6, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 6 ? -1 : 6;

                    if (Widgets::Tab(xorstr_("Config"), StyleProvider::ConfigIconTexture, currentTab == 7, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 7 ? -1 : 7;

                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("Build Info")).c_str(), ImVec2(sideBarSize.x, StyleProvider::MainMenuBuildInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 buildInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 buildInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(buildInfoPos, buildInfoPos + buildInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::PushFont(StyleProvider::FontSmallBold);
                const ImVec2 cheatInfoSize = ImGui::CalcTextSize(xorstr_("Maple Lite for osu!"));
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - cheatInfoSize.x / 2, buildInfoSize.y / 2 - style.ItemSpacing.y / 4 - cheatInfoSize.y));
                ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Maple Lite for osu!"));
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontSmall);
                const ImVec2 buildStringSize = ImGui::CalcTextSize(xorstr_("mlo-05112023"));
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, xorstr_("mlo-05112023"));
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(StyleProvider::MainMenuSideBarSize.x, 0) + StyleProvider::Padding);
        ImGui::BeginChild(StringUtilities::GenerateRandomString(16, xorstr_("Options")).c_str(), ImVec2(StyleProvider::MainMenuSize.x - StyleProvider::MainMenuSideBarSize.x, StyleProvider::MainMenuSize.y) - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushFont(StyleProvider::FontDefault);

            const float optionsWidth = ImGui::GetWindowWidth();

            if (currentTab == 0)
            {
                Widgets::BeginPanel(xorstr_("Relax"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.Relax.Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("All hit objects will be automatically tapped by Maple."));
                    Widgets::Hotkey(xorstr_("Toggle key"), &ConfigManager::CurrentConfig.Relax.ToggleKey); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Allows you to toggle relax mid gameplay."));
                   
                    const auto m1 = std::string(xorstr_("M1"));
                    const auto k1 = std::string(xorstr_("K1"));
                    const auto m2 = std::string(xorstr_("M2"));
                    const auto k2 = std::string(xorstr_("K2"));
                    const char* keys[] = { m1.c_str(), k1.c_str(), m2.c_str(), k2.c_str() };
                    Widgets::Combo(xorstr_("Primary key"), &ConfigManager::CurrentConfig.Relax.PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::Combo(xorstr_("Secondary key"), &ConfigManager::CurrentConfig.Relax.SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    
                    Widgets::SliderInt(xorstr_("Alternate BPM"), &ConfigManager::CurrentConfig.Relax.AlternateBPM, 0, 500, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A BPM at which relax will start alternating."));
                    Widgets::Checkbox(xorstr_("Slider alternation override"), &ConfigManager::CurrentConfig.Relax.SliderAlternationOverride); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Changes the way how alternation of sliders is handled.\nIt is recommended to enable this option on techno maps."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Timing"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(8)));
                {
                    Widgets::SliderInt(xorstr_("Offset"), &ConfigManager::CurrentConfig.Relax.Timing.Offset, -50, 50, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Offsets keypresses by the specified amount of milliseconds.\n\nUseful if you don't want your hits to be centered around 0 ms offset or if you're having latency issues."));
                    Widgets::SliderInt(xorstr_("Target unstable rate"), &ConfigManager::CurrentConfig.Relax.Timing.TargetUnstableRate, 0, 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
                    Widgets::SliderInt(xorstr_("Allowable hit range"), &ConfigManager::CurrentConfig.Relax.Timing.AllowableHitRange, 0, 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
                    Widgets::HitErrorBar(ConfigManager::CurrentConfig.Relax.Timing.AllowableHitRange);
                    Widgets::SliderInt(xorstr_("Minimum hold time"), &ConfigManager::CurrentConfig.Relax.Timing.MinimumHoldTime, 25, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A minimum duration of a keypress."));
                    Widgets::SliderInt(xorstr_("Maximum hold time"), &ConfigManager::CurrentConfig.Relax.Timing.MaximumHoldTime, 25, 150, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum duration of a keypress."));
                    Widgets::SliderInt(xorstr_("Minimum slider hold time"), &ConfigManager::CurrentConfig.Relax.Timing.MinimumSliderHoldTime, 25, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A minimum duration of a keypress for sliders and spinners."));
                    Widgets::SliderInt(xorstr_("Maximum slider hold time"), &ConfigManager::CurrentConfig.Relax.Timing.MaximumSliderHoldTime, 25, 150, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum duration of a keypress for sliders and spinners."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Hit Scan"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox(xorstr_("Direction prediction"), &ConfigManager::CurrentConfig.Relax.HitScan.DirectionPredictionEnabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Predicts whether or not you're leaving the circle and clicks if you are."));
                    Widgets::SliderInt(xorstr_("Direction prediction angle"), &ConfigManager::CurrentConfig.Relax.HitScan.DirectionPredictionAngle, 0, 90, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum angle between current cursor position, last cursor position and next circle position for prediction to trigger.\n\nLower value = worse prediction."));
                    Widgets::SliderFloat(xorstr_("Direction prediction scale"), &ConfigManager::CurrentConfig.Relax.HitScan.DirectionPredictionScale, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies a portion of the circle where prediction will trigger.\n\n0 = full circle."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Blatant"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(1, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Don't use this on legit servers!"));
                    Widgets::Checkbox(xorstr_("Use lowest possible hold times"), &ConfigManager::CurrentConfig.Relax.Blatant.UseLowestPossibleHoldTimes);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 1)
            {
                if (ConfigManager::CurrentConfig.AimAssist.Algorithm == 0)
                {
                    Widgets::BeginPanel(xorstr_("Attention"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(0, 4)));
                    {
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("This algorithm can lead to technical bugs and teleportations on"));
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("certain settings."));
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("We strongly recommend you to use the other algorithms"));
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("unless you're able to configure it to look as legit as possible."));
                    }
                    Widgets::EndPanel();

                    ImGui::Spacing();
                }
                Widgets::BeginPanel(xorstr_("Aim Assist"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(ConfigManager::CurrentConfig.AimAssist.Algorithm == 0 ? 10 : 5, ConfigManager::CurrentConfig.AimAssist.Algorithm == 2 && ConfigManager::CurrentConfig.AimAssist.Algorithmv3.Power > 1.f ? 1 : 0)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.AimAssist.Enabled);

                    const auto v1 = std::string(xorstr_("v1"));
                    const auto v2 = std::string(xorstr_("v2"));
                    const auto v3 = std::string(xorstr_("v3"));
                    const char* algorithms[] = { v1.c_str(), v2.c_str(), v3.c_str() };
                    Widgets::Combo(xorstr_("Algorithm"), &ConfigManager::CurrentConfig.AimAssist.Algorithm, algorithms, IM_ARRAYSIZE(algorithms));

                    if (ConfigManager::CurrentConfig.AimAssist.Algorithm == 0)
                    {
                        Widgets::SliderFloat(xorstr_("Strength##algov1strength"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.Strength, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the Aim Assist strength, change this value according to how strong you want to be helped with."));
                        Widgets::Checkbox(xorstr_("Assist on sliders##algov1assistonsliders"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.AssistOnSliders); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Do you need help on sliders?\nYes?\nTurn this on then."));
                        Widgets::SliderInt(xorstr_("Base FOV##algov1basefov"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.BaseFOV, 0, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps."));
                        Widgets::SliderFloat(xorstr_("Maximum FOV (Scaling)##algov1maxfovscale"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.MaximumFOVScale, 0, 5, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Minimum FOV (Total)##algov1minfovtotal"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.MinimumFOVTotal, 0, 100, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the total minimum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Maximum FOV (Total)##algov1maxfovtotal"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.MaximumFOVTotal, 0, 500, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the total maximum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Acceleration factor"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv1.AccelerationFactor, 0, 5, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps."));
                    }
                    else if (ConfigManager::CurrentConfig.AimAssist.Algorithm == 1)
                    {
                        Widgets::SliderFloat(xorstr_("Power"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv2.Power, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox(xorstr_("Assist on sliders##algov2assistonsliders"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv2.AssistOnSliders);
                    }
                    else
                    {
                        Widgets::SliderFloat(xorstr_("Power"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv3.Power, 0.f, 2.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);

                        if (ConfigManager::CurrentConfig.AimAssist.Algorithmv3.Power > 1.f)
                            ImGui::TextColored(StyleProvider::AccentColour, xorstr_("We don't recommend using a power greater than 1."));

                        Widgets::SliderFloat(xorstr_("Slider Assist Power"), &ConfigManager::CurrentConfig.AimAssist.Algorithmv3.SliderAssistPower, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);
                    }
                    Widgets::Checkbox(xorstr_("Show Debug Overlay"), &ConfigManager::CurrentConfig.AimAssist.DrawDebugOverlay);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 2)
            {
                Widgets::BeginPanel(xorstr_("Timewarp"), ImVec2(optionsWidth, Widgets::CalcPanelHeight( 4, ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled ? 0 : 2)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.Timewarp.Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Slows down or speeds up the game."));
                   
                    const auto rate = std::string(xorstr_("Rate"));
                    const auto multiplier = std::string(xorstr_("Multiplier"));
                    const char* types[] = { rate.c_str(), multiplier.c_str() };
                    Widgets::Combo(xorstr_("Type"), &ConfigManager::CurrentConfig.Timewarp.Type, types, IM_ARRAYSIZE(types));
                    
                    if (ConfigManager::CurrentConfig.Timewarp.Type == 0)
                    {
                        Widgets::SliderInt(xorstr_("Rate"), &ConfigManager::CurrentConfig.Timewarp.Rate, 25, ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled ? 150 : 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xorstr_("The desired speed of timewarp.\n\nLower value = slower.\nHigher value = faster.\n\n75 is HalfTime.\n100 is NoMod.\n150 is DoubleTime."));
                    }
                    else
                    {
                        Widgets::SliderFloat(xorstr_("Multiplier"), &ConfigManager::CurrentConfig.Timewarp.Multiplier, 0.25f, ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled ? 1.0f : 1.5f, .01f, .1f, xorstr_("%.2f"), ImGuiSliderFlags_AlwaysClamp);
                    }

		    Widgets::Checkbox(xorstr_("Rate Limit"), &ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Limits the maximum speed to original speed."));

		    if (!ConfigManager::CurrentConfig.Timewarp.RateLimitEnabled)
		    {
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("WARNING: Playing with the speed higher than original speed"));
                        ImGui::TextColored(StyleProvider::AccentColour, xorstr_("will inevitably get your account banned!"));
		    }
                }
                Widgets::EndPanel();
            }
            if (currentTab == 3)
            {
                Widgets::BeginPanel(xorstr_("Replay Bot"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 1)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ReplayBot::Enabled);

                    if (Widgets::Checkbox(xorstr_("Disable aiming"), &ReplayBot::DisableAiming))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableTapping = false;
                    }

                    if (Widgets::Checkbox(xorstr_("Disable tapping"), &ReplayBot::DisableTapping))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableAiming = false;
                    }

                    if (Widgets::Button(xorstr_("Select replay")))
                    {
                        if (!replayDialogInitialized)
                        {
                            replayDialog.SetTitle(xorstr_("Select replay"));
                            replayDialog.SetTypeFilters({ xorstr_(".osr") });

                            replayDialogInitialized = true;
                        }

                        replayDialog.Open();
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
                    replayDialog.Display();
                    ImGui::PopStyleVar();

                    if (replayDialog.HasSelected())
                    {
                        ReplayBot::LoadReplay(replayDialog.GetSelected().string().c_str());
                        replayDialog.ClearSelected();
                    }

                    const std::string selectedReplayText = std::string(xorstr_("Selected replay: ")) + ReplayBot::GetReplayString();
                    ImGui::Text(selectedReplayText.c_str());
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
            {
                Widgets::BeginPanel(xorstr_("AR Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.Visuals.ARChanger.Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("AR is short for Approach Rate and defines when hit objects start to fade in relative to when they should be hit or collected."));
                    Widgets::SliderFloat(xorstr_("AR"), &ConfigManager::CurrentConfig.Visuals.ARChanger.AR, 0.f, 12.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Higher value = hit objects will be shown for a shorter period of time = less time to react.\n\nLower value = hit objects will be shown for a longer period of time = more time to react."));
                    Widgets::Checkbox(xorstr_("Adjust to mods"), &ConfigManager::CurrentConfig.Visuals.ARChanger.AdjustToMods); ImGui::SameLine(); Widgets::Tooltip(xorstr_("If this option is enabled, AR Changer will adjust the AR you have set to currently selected mods.\n\nFor example, if you selected Easy mod, AR will be slightly lower."));
                    Widgets::Checkbox(xorstr_("Adjust to rate"), &ConfigManager::CurrentConfig.Visuals.ARChanger.AdjustToRate);
                    Widgets::Checkbox(xorstr_("Draw preemptive dot"), &ConfigManager::CurrentConfig.Visuals.ARChanger.DrawPreemptiveDot);
                    ImGui::ColorEdit4(xorstr_("Preemptive dot colour"), reinterpret_cast<float*>(&ConfigManager::CurrentConfig.Visuals.ARChanger.PreemptiveDotColour), ImGuiColorEditFlags_NoInputs);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("CS Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Don't use this on legit servers!"));
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.Visuals.CSChanger.Enabled);
                    Widgets::SliderFloat(xorstr_("CS"), &ConfigManager::CurrentConfig.Visuals.CSChanger.CS, 0.f, 10.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("HD & FL Removers"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox(xorstr_("Disable Hidden"), &ConfigManager::CurrentConfig.Visuals.Removers.HiddenRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Hidden mod.");
                    Widgets::Checkbox(xorstr_("Disable Flashlight"), &ConfigManager::CurrentConfig.Visuals.Removers.FlashlightRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Flashlight mod.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("User Interface"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 8 : 7, 0, 2)));
                {
                    Widgets::Checkbox(xorstr_("Snow"), &ConfigManager::CurrentConfig.Visuals.UI.Snow);

                    const auto fifty = std::string(xorstr_("50%"));
                    const auto seventyfive = std::string(xorstr_("75%"));
                    const auto hundred = std::string(xorstr_("100%"));
                    const auto hundredtwentyfive = std::string(xorstr_("125%"));
                    const auto hundredfifty = std::string(xorstr_("150%"));
                    const char* scales[] = { fifty.c_str(), seventyfive.c_str(), hundred.c_str(), hundredtwentyfive.c_str(), hundredfifty.c_str()};
                    if (Widgets::Combo(xorstr_("Menu scale"), &ConfigManager::CurrentConfig.Visuals.UI.MenuScale, scales, IM_ARRAYSIZE(scales)))
                        StyleProvider::UpdateScale();

                    ImGui::Spacing();

                    if (Widgets::Button(xorstr_("Load background image"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        if (!backgroundImageDialogInitialized)
                        {
                            backgroundImageDialog.SetTitle(xorstr_("Select background image"));
                            backgroundImageDialog.SetTypeFilters({ xorstr_(".png"), xorstr_(".jpg"), xorstr_(".jpeg"), xorstr_(".bmp"), xorstr_(".tga") });

                            backgroundImageDialogInitialized = true;
                        }

                        backgroundImageDialog.Open();
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
                    backgroundImageDialog.Display();
                    ImGui::PopStyleVar();

                    if (backgroundImageDialog.HasSelected())
                    {
						ConfigManager::CurrentConfig.Visuals.UI.MenuBackground = backgroundImageDialog.GetSelected().string().c_str();
                        backgroundImageDialog.ClearSelected();

                        updateBackground();
                    }

                    if (backgroundTexture)
                    {
                        if (Widgets::Button(xorstr_("Remove background image"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        {
                            ConfigManager::CurrentConfig.Visuals.UI.MenuBackground[0] = '\0';

                            updateBackground();
                        }
                    }

                    ImGui::Spacing();

                    bool coloursChanged = false;
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Accent colour"), reinterpret_cast<float*>(&ConfigManager::CurrentConfig.Visuals.UI.AccentColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Menu colour"), reinterpret_cast<float*>(&ConfigManager::CurrentConfig.Visuals.UI.MenuColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Control colour"), reinterpret_cast<float*>(&ConfigManager::CurrentConfig.Visuals.UI.ControlColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Text colour"), reinterpret_cast<float*>(&ConfigManager::CurrentConfig.Visuals.UI.TextColour), ImGuiColorEditFlags_NoInputs);

                    if (coloursChanged)
                        StyleProvider::UpdateColours();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 5)
            {
                Widgets::BeginPanel(xorstr_("Spoofer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(8, 1, 2)));
                {
                    const bool sameProfile = Spoofer::SelectedProfile == Spoofer::LoadedProfile;
                    const bool currentProfileIsDefault = Spoofer::SelectedProfile == 0;

                    const float buttonWidth = (ImGui::GetWindowWidth() * 0.5f - style.ItemSpacing.x) / 2;

                    Widgets::Combo(xorstr_("Profiles"), &Spoofer::SelectedProfile, Spoofer::Profiles);

					char currentProfileName[Spoofer::Profiles[Spoofer::LoadedProfile].GetSize()];
                    Spoofer::Profiles[Spoofer::LoadedProfile].GetData(currentProfileName);

                    ImGui::Text(xorstr_("Current profile: %s"), currentProfileName);

                    if (sameProfile)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Load"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Spoofer::Load();

                    if (sameProfile)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::SameLine();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Delete"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Spoofer::Delete();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    if (Widgets::Button(xorstr_("Import from clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Import();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Export to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Export();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    Widgets::InputText(xorstr_("Profile name##profilerename"), Spoofer::RenamedProfileName, 32);
                    if (Widgets::Button(xorstr_("Rename selected profile"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Rename();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    Widgets::InputText(xorstr_("Profile name##newprofile"), Spoofer::NewProfileName, 32);
                    if (Widgets::Button(xorstr_("Create new profile"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Create();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 6)
            {
                Widgets::BeginPanel(xorstr_("Misc"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2 ? 5 : 4)));
                {
                    const auto allow = std::string(xorstr_("Allow"));
                    const auto disallow = std::string(xorstr_("Disallow"));
                    const auto prompt = std::string(xorstr_("Prompt"));
                    const char* scoreSubmissionTypes[] = { allow.c_str(), disallow.c_str(), prompt.c_str() };
                    Widgets::Combo("Score submission", &ConfigManager::CurrentConfig.Misc.ScoreSubmissionType, scoreSubmissionTypes, IM_ARRAYSIZE(scoreSubmissionTypes)); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies score submission behavior.\n\nAllow: all scores will be sent to osu! servers.\nDisallow: your scores won't be sent to osu! servers.\nPrompt: before submitting a score Maple will ask you whether or not you really want to submit it."));
                    if (ConfigManager::CurrentConfig.Misc.ScoreSubmissionType == 2)
                    {
                        const auto submit = std::string(xorstr_("Submit"));
                        const auto dontSubmit = std::string(xorstr_("Don't submit"));
                        const char* promptBehaviors[] = { submit.c_str(), dontSubmit.c_str() };
						Widgets::Combo("Behavior on retry", &ConfigManager::CurrentConfig.Misc.PromptBehaviorOnRetry, promptBehaviors, IM_ARRAYSIZE(promptBehaviors)); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies what Maple should do with your score when you retry a map."));
                    }
					
                    Widgets::Checkbox(xorstr_("Disable spectators"), &ConfigManager::CurrentConfig.Misc.DisableSpectators); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Spectators will keep buffering infinitely."));

                    bool storageConfigEdited = false;
                    storageConfigEdited |= Widgets::Checkbox(xorstr_("Show menu after injection"), &StorageConfig::ShowMenuAfterInjection);
                    storageConfigEdited |= Widgets::Hotkey(xorstr_("Menu key"), &StorageConfig::MenuKey);

                    if (StorageConfig::MenuKey == 0)
                        StorageConfig::MenuKey = VK_DELETE;

                    if (storageConfigEdited)
                        Storage::SaveStorageConfig();
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Logging"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    if (Widgets::Button(xorstr_("Copy previous runtime log to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.75f, ImGui::GetFrameHeight())))
                        ClipboardUtilities::Write(Logger::GetPreviousRuntimeLogData());

                    if (Widgets::Button(xorstr_("Copy runtime log to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.75f, ImGui::GetFrameHeight())))
                        ClipboardUtilities::Write(Logger::GetRuntimeLogData());

                    if (Widgets::Button(xorstr_("Copy crash log to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.75f, ImGui::GetFrameHeight())))
                        ClipboardUtilities::Write(Logger::GetCrashLogData());

                    if (Widgets::Button(xorstr_("Copy performance log to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.75f, ImGui::GetFrameHeight())))
                        ClipboardUtilities::Write(Logger::GetPerformanceLogData());
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Discord Rich Presence Spoofer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(11)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Spoofs various fields of your Discord Game Activity"));

                	Widgets::Checkbox(xorstr_("Custom large image text"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomLargeImageTextEnabled);
                    Widgets::InputText(xorstr_("Large image text"), ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomLargeImageText, 128);

                    Widgets::Checkbox(xorstr_("Custom play mode"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomPlayModeEnabled);

                    const auto osu = std::string(xorstr_("osu!"));
                    const auto osuTaiko = std::string(xorstr_("osu!taiko"));
                    const auto osuCatch = std::string(xorstr_("osu!catch"));
                    const auto osuMania = std::string(xorstr_("osu!mania"));
                    const char* playModes[] = { osu.c_str(), osuTaiko.c_str(), osuCatch.c_str(), osuMania.c_str()};
                    Widgets::Combo(xorstr_("Play mode"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomPlayMode, playModes, IM_ARRAYSIZE(playModes));

                    Widgets::Checkbox(xorstr_("Custom state"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomStateEnabled);
                    Widgets::InputText(xorstr_("State"), ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomState, 128);

                    Widgets::Checkbox(xorstr_("Custom details"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomDetailsEnabled);
                    Widgets::InputText(xorstr_("Details"), ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.CustomDetails, 128);

                    Widgets::Checkbox(xorstr_("Hide spectate button"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.HideSpectateButton);

                    Widgets::Checkbox(xorstr_("Hide match button"), &ConfigManager::CurrentConfig.Misc.DiscordRichPresenceSpoofer.HideMatchButton);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 7)
            {
                Widgets::BeginPanel(xorstr_("Config"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(8, 0, 2)));
                {
                    const bool currentConfigIsDefault = ConfigManager::CurrentConfigIndex == 0;

                    const float buttonWidth = ((ImGui::GetWindowWidth() * 0.5f) - (style.ItemSpacing.x * 2)) / 3;
                    Widgets::Combo(xorstr_("Config"), &ConfigManager::CurrentConfigIndex, ConfigManager::Configs);

                    if (Widgets::Button(xorstr_("Load"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                    {
                        ConfigManager::Load();

                        updateBackground();
                        StyleProvider::UpdateColours();
                        StyleProvider::UpdateScale();
                    }

                    ImGui::SameLine();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Save"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        ConfigManager::Save();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::SameLine();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Delete"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        ConfigManager::Delete();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    if (Widgets::Button(xorstr_("Import from clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        ConfigManager::Import();

                        updateBackground();
                        StyleProvider::UpdateColours();
                        StyleProvider::UpdateScale();
                    }

                    if (currentConfigIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xorstr_("Export to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        ConfigManager::Export();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    Widgets::InputText(xorstr_("Config name##configrename"), ConfigManager::RenamedConfigName, 32);
                    if (Widgets::Button(xorstr_("Rename selected config"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        ConfigManager::Rename();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    Widgets::InputText(xorstr_("Config name##newconfig"), ConfigManager::NewConfigName, 32);

                    if (Widgets::Button(xorstr_("Create new config"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        ConfigManager::Create();

                        updateBackground();
                        StyleProvider::UpdateColours();
                        StyleProvider::UpdateScale();
                    }
                }
                Widgets::EndPanel();
            }

            ImGui::PopFont();
        }
        ImGui::EndChild();

        ImGui::SetWindowPos(ImVec2(std::clamp(menuPos.x, clientPos.X, clientPos.X + clientSize.X - menuSize.x), std::clamp(menuPos.y, clientPos.Y, clientPos.Y + clientSize.Y - menuSize.y)));
    }

    ImGui::End();

    if (ConfigManager::ForceDisableScoreSubmission)
    {
        ImGui::PushFont(StyleProvider::FontDefaultBold);
        const ImVec2 panelHeaderLabelSize = ImGui::CalcTextSize(xorstr_("Attention!"));
        const float panelHeaderHeight = panelHeaderLabelSize.y + StyleProvider::Padding.y * 2;
        ImGui::PopFont();

        ImGui::PushFont(StyleProvider::FontDefault);

        const float panelContentHeight = Widgets::CalcPanelHeight(1, 3);
        const ImVec2 windowSize = ImVec2(ImGui::CalcTextSize(ConfigManager::BypassFailed ? xorstr_("Score submission has been disabled to prevent your account from being flagged or banned.") : xorstr_("Your osu! client is running a newer version of the anti-cheat, which has not yet been confirmed to be safe.")).x, panelHeaderHeight + panelContentHeight) + StyleProvider::Padding * 2;

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(GameBase::GetClientPosition().X + GameBase::GetClientSize().X / 2 - windowSize.x / 2, GameBase::GetClientPosition().Y + GameBase::GetClientSize().Y - windowSize.y - StyleProvider::Padding.y));
        ImGui::Begin(xorstr_("AuthNotice"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        {
            Widgets::BeginPanel(xorstr_("Attention!"), ImVec2(windowSize.x, panelContentHeight));
            {
                if (ConfigManager::BypassFailed)
                {
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(xorstr_("Maple was unable to confirm the detection status. Please report this.")).x / 2);
                    ImGui::Text(xorstr_("Maple was unable to confirm the detection status. Please report this."));
                }
                else
                {
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(xorstr_("Your osu! client is running a newer version of the anti-cheat, which has not yet been confirmed to be safe.")).x / 2);
                    ImGui::Text(xorstr_("Your osu! client is running a newer version of the anti-cheat, which has not yet been confirmed to be safe."));
                }

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(xorstr_("Score submission has been disabled to prevent your account from being flagged or banned.")).x / 2);
                ImGui::Text(xorstr_("Score submission has been disabled to prevent your account from being flagged or banned."));
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(xorstr_("If you want to re-enable score submission, click the button below.")).x / 2);
                ImGui::Text(xorstr_("If you want to re-enable score submission, click the button below."));
                if (Widgets::Button(xorstr_("I understand the risks, enable score submission"), ImVec2(ImGui::GetWindowWidth(), ImGui::GetFrameHeight())))
                {
                    ConfigManager::ForceDisableScoreSubmission = false;
                }
            }
            Widgets::EndPanel();
        }
        ImGui::End();

        ImGui::PopFont();
    }
}

void MainMenu::Show()
{
    if (GameBase::GetMode() != OsuModes::Play)
		isVisible = true;
}

void MainMenu::Hide()
{
    isVisible = false;
}

void MainMenu::ToggleVisibility()
{
    if (GameBase::GetMode() != OsuModes::Play || isVisible)
		isVisible = !isVisible;
}

bool MainMenu::GetIsVisible()
{
    return isVisible;
}
