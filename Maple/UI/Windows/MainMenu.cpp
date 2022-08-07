#include "MainMenu.h"

#include <filesystem>

#include "imgui.h"

#include "../StyleProvider.h"
#include "../Widgets/Widgets.h"
#include "../../Storage/Storage.h"
#include "../Widgets/3rd-party/FileDialog/imfilebrowser.h"
#include "../../Storage/StorageConfig.h"
#include "../../Features/Spoofer/Spoofer.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../Features/ReplayBot/ReplayBot.h"
#include "../../Utilities/Security/xorstr.hpp"

bool backgroundImageDialogInitialized = false;
ImGui::FileBrowser backgroundImageDialog;

bool replayDialogInitialized = false;
ImGui::FileBrowser replayDialog;

void MainMenu::updateBackground()
{
    if (Config::Visuals::UI::MenuBackground[0] == '\0' || !std::filesystem::exists(Config::Visuals::UI::MenuBackground))
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
        backgroundTexture = TextureLoader::LoadTextureFromFileOGL3(Config::Visuals::UI::MenuBackground);
    else
        backgroundTexture = TextureLoader::LoadTextureFromFileD3D9(UI::D3D9Device, Config::Visuals::UI::MenuBackground);
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
    ImGui::Begin(xor ("Main Menu"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuSize = ImGui::GetCurrentWindow()->Size;
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + StyleProvider::MainMenuSideBarSize, ImColor(StyleProvider::MenuColourDark), style.WindowRounding, expanded ? ImDrawFlags_RoundCornersAll & ~ImDrawFlags_RoundCornersTopRight : ImDrawFlags_RoundCornersAll);

        ImGui::SetCursorPos(StyleProvider::Padding);
        ImGui::BeginChild(xor ("Side Bar"), StyleProvider::MainMenuSideBarSize - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
        {
            const ImVec2 sideBarSize = ImGui::GetCurrentWindow()->Size;

            ImGui::PushFont(StyleProvider::FontHugeBold);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5 * StyleProvider::Scale, 10 * StyleProvider::Scale));
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ((ImGui::CalcTextSize(xor ("Maple")).x / 2) + StyleProvider::MapleLogoSize.x / 2 + style.ItemSpacing.x / 2));
            ImGui::Image(StyleProvider::MapleLogoTexture, StyleProvider::MapleLogoSize, ImVec2(0, 0), ImVec2(1, 1), StyleProvider::AccentColour);
            ImGui::SameLine();
            ImGui::TextColored(StyleProvider::AccentColour, xor ("Maple"));
            ImGui::PopStyleVar();
            ImGui::PopFont();
            ImGui::PushFont(StyleProvider::FontSmall);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style.ItemSpacing.y);
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ImGui::CalcTextSize(xor ("the quickest way to the top")).x / 2);
            ImGui::TextColored(StyleProvider::MottoColour, xor ("the quickest way to the top"));
            ImGui::PopFont();

            ImGui::Spacing();

            ImGui::BeginChild(xor ("User Info"), ImVec2(sideBarSize.x, StyleProvider::MainMenuUserInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 userInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 userInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(userInfoPos, userInfoPos + userInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::GetWindowDrawList()->AddImageRounded(StyleProvider::AvatarTexture, userInfoPos + ImVec2(userInfoSize.y / 4, userInfoSize.y / 4), userInfoPos + ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2, userInfoSize.y / 4 + userInfoSize.y / 2), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), style.FrameRounding);

                ImGui::PushFont(StyleProvider::FontDefaultBold);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 - style.ItemSpacing.y / 4 - ImGui::CalcTextSize("Welcome back").y));
                ImGui::Text(xor ("Welcome back"));
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontDefault);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::AccentColour, "dev");//Communication::CurrentUser->Username.c_str());
                ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::BeginChild(xor ("Tabs"), ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (40 * StyleProvider::Scale) * 8; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild(xor ("Tabs##001"), ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 40 * StyleProvider::Scale);

                    ImGui::PushFont(StyleProvider::FontDefaultBold);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                    if (Widgets::Tab(xor ("Relax"), StyleProvider::RelaxIconTexture, currentTab == 0, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 0 ? -1 : 0;

                    if (Widgets::Tab(xor ("Aim Assist"), StyleProvider::AimAssistIconTexture, currentTab == 1, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 1 ? -1 : 1;

                    if (Widgets::Tab(xor ("Timewarp"), StyleProvider::TimewarpIconTexture, currentTab == 2, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 2 ? -1 : 2;

                    if (Widgets::Tab(xor ("Replays"), StyleProvider::ReplaysIconTexture, currentTab == 3, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 3 ? -1 : 3;

                    if (Widgets::Tab(xor ("Visuals"), StyleProvider::VisualsIconTexture, currentTab == 4, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;

                    if (Widgets::Tab(xor ("Spoofer"), StyleProvider::SpooferIconTexture, currentTab == 5, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;

                    if (Widgets::Tab(xor ("Misc"), StyleProvider::MiscIconTexture, currentTab == 6, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 6 ? -1 : 6;

                    if (Widgets::Tab(xor ("Config"), StyleProvider::ConfigIconTexture, currentTab == 7, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 7 ? -1 : 7;

                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            ImGui::BeginChild(xor ("Build Info"), ImVec2(sideBarSize.x, StyleProvider::MainMenuBuildInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 buildInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 buildInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(buildInfoPos, buildInfoPos + buildInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::PushFont(StyleProvider::FontSmallBold);
                const ImVec2 cheatInfoSize = ImGui::CalcTextSize(xor ("Maple Lite for osu!"));
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - cheatInfoSize.x / 2, buildInfoSize.y / 2 - style.ItemSpacing.y / 4 - cheatInfoSize.y));
                ImGui::TextColored(StyleProvider::AccentColour, xor ("Maple Lite for osu!"));
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontSmall);
                const ImVec2 buildStringSize = ImGui::CalcTextSize(xor ("l16072022"));
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, xor ("l16072022"));
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(StyleProvider::MainMenuSideBarSize.x, 0) + StyleProvider::Padding);
        ImGui::BeginChild(xor ("Options"), ImVec2(StyleProvider::MainMenuSize.x - StyleProvider::MainMenuSideBarSize.x, StyleProvider::MainMenuSize.y) - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushFont(StyleProvider::FontDefault);

            const float optionsWidth = ImGui::GetWindowWidth();

            if (currentTab == 0)
            {
                Widgets::BeginPanel(xor ("Relax"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &Config::Relax::Enabled); ImGui::SameLine(); Widgets::Tooltip(xor ("All hit objects will be automatically tapped by Maple."));
                    Widgets::Hotkey(xor ("Toggle key"), &Config::Relax::ToggleKey); ImGui::SameLine(); Widgets::Tooltip(xor ("Allows you to toggle relax mid gameplay."));
                    const char* keys[] = { xor ("M1"),xor ("K1"),xor ("M2"),xor ("K2") };
                    Widgets::Combo(xor ("Primary key"), &Config::Relax::PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::Combo(xor ("Secondary key"), &Config::Relax::SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::SliderInt(xor ("Alternate BPM"), &Config::Relax::AlternateBPM, 0, 500, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("A BPM at which relax will start alternating."));
                    Widgets::Checkbox(xor ("Slider alternation override"), &Config::Relax::SliderAlternationOverride); ImGui::SameLine(); Widgets::Tooltip(xor ("Changes the way how alternation of sliders is handled.\nIt is recommended to enable this option on techno maps."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("Timing"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::SliderInt(xor ("Offset"), &Config::Relax::Timing::Offset, -50, 50, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Offsets keypresses by the specified amount of milliseconds.\n\nUseful if you don't want your hits to be centered around 0 ms offset or if you're having latency issues."));
                    Widgets::SliderInt(xor ("Target unstable rate"), &Config::Relax::Timing::TargetUnstableRate, 0, 300, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput);

                    if (Widgets::SliderInt(xor ("Average hold time"), &Config::Relax::Timing::AverageHoldTime, 25, 100, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageHoldTime - Config::Relax::Timing::AverageHoldTimeError < 25)
                            Config::Relax::Timing::AverageHoldTimeError = Config::Relax::Timing::AverageHoldTime - 25;

                    ImGui::SameLine(); Widgets::Tooltip(xor ("An average duration of a keypress."));

                    if (Widgets::SliderInt(xor ("Average hold time error"), &Config::Relax::Timing::AverageHoldTimeError, 0, 75, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageHoldTime - Config::Relax::Timing::AverageHoldTimeError < 25)
                            Config::Relax::Timing::AverageHoldTime = Config::Relax::Timing::AverageHoldTimeError + 25;

                    ImGui::SameLine(); Widgets::Tooltip(xor ("An average deviation from the duration of a keypress."));

                    if (Widgets::SliderInt(xor ("Average slider hold time"), &Config::Relax::Timing::AverageSliderHoldTime, 25, 100, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageSliderHoldTime - Config::Relax::Timing::AverageSliderHoldTimeError < 25)
                            Config::Relax::Timing::AverageSliderHoldTimeError = Config::Relax::Timing::AverageSliderHoldTime - 25;

                    ImGui::SameLine(); Widgets::Tooltip(xor ("An average duration of a keypress for sliders and spinners."));

                    if (Widgets::SliderInt(xor ("Average slider hold time error"), &Config::Relax::Timing::AverageSliderHoldTimeError, 0, 75, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageSliderHoldTime - Config::Relax::Timing::AverageSliderHoldTimeError < 25)
                            Config::Relax::Timing::AverageSliderHoldTime = Config::Relax::Timing::AverageSliderHoldTimeError + 25;

                    ImGui::SameLine(); Widgets::Tooltip(xor ("An average deviation from the duration of a keypress for sliders and spinners."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("Hit Scan"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    Widgets::Checkbox(xor ("Wait late"), &Config::Relax::HitScan::WaitLateEnabled); ImGui::SameLine(); Widgets::Tooltip(xor ("Delays a keypress if you failed to aim a hitobject in time."));
                    Widgets::Checkbox(xor ("Direction prediction"), &Config::Relax::HitScan::DirectionPredictionEnabled); ImGui::SameLine(); Widgets::Tooltip(xor ("Predicts whether or not you're leaving the circle and clicks if you are."));
                    Widgets::SliderInt(xor ("Direction prediction angle"), &Config::Relax::HitScan::DirectionPredictionAngle, 0, 90, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("A maximum angle between current cursor position, last cursor position and next circle position for prediction to trigger.\n\nLower value = worse prediction."));
                    Widgets::SliderFloat(xor ("Direction prediction scale"), &Config::Relax::HitScan::DirectionPredictionScale, 0.f, 1.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Specifies a portion of the circle where prediction will trigger.\n\n0 = full circle."));
                }
                Widgets::EndPanel();
            }
            if (currentTab == 1)
            {
                if (Config::AimAssist::Algorithm == 0)
                {
                    Widgets::BeginPanel(xor ("Attention"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(0, 4)));
                    {
                        ImGui::TextColored(StyleProvider::AccentColour, xor ("This algorithm can lead to technical bugs and teleportations on"));
                        ImGui::TextColored(StyleProvider::AccentColour, xor ("certain settings."));
                        ImGui::TextColored(StyleProvider::AccentColour, xor ("We strongly recommend you to use the other algorithms"));
                        ImGui::TextColored(StyleProvider::AccentColour, xor ("unless you're able to configure it to look as legit as possible."));
                    }
                    Widgets::EndPanel();

                    ImGui::Spacing();
                }
                Widgets::BeginPanel(xor ("Aim Assist"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(Config::AimAssist::Algorithm == 0 ? 10 : 5, Config::AimAssist::Algorithm == 2 && Config::AimAssist::Algorithmv3::Power > 1.f ? 1 : 0)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &Config::AimAssist::Enabled);
                    const char* algorithms[] = { xor ("v1"), xor ("v2"), xor ("v3") };
                    Widgets::Combo(xor ("Algorithm"), &Config::AimAssist::Algorithm, algorithms, IM_ARRAYSIZE(algorithms));
                    if (Config::AimAssist::Algorithm == 0)
                    {
                        Widgets::SliderFloat(xor ("Strength##algov1strength"), &Config::AimAssist::Algorithmv1::Strength, 0.f, 1.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Sets the Aim Assist strength, change this value according to how strong you want to be helped with."));
                        Widgets::Checkbox(xor ("Assist on sliders##algov1assistonsliders"), &Config::AimAssist::Algorithmv1::AssistOnSliders); ImGui::SameLine(); Widgets::Tooltip(xor ("Do you need help on sliders?\nYes?\nTurn this on then."));
                        Widgets::SliderInt(xor ("Base FOV##algov1basefov"), &Config::AimAssist::Algorithmv1::BaseFOV, 0, 100, 1, 10, xor ("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps."));
                        Widgets::SliderFloat(xor ("Maximum FOV (Scaling)##algov1maxfovscale"), &Config::AimAssist::Algorithmv1::MaximumFOVScale, 0, 5, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist."));
                        Widgets::SliderFloat(xor ("Minimum FOV (Total)##algov1minfovtotal"), &Config::AimAssist::Algorithmv1::MinimumFOVTotal, 0, 100, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Sets the total minimum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xor ("Maximum FOV (Total)##algov1maxfovtotal"), &Config::AimAssist::Algorithmv1::MaximumFOVTotal, 0, 500, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Sets the total maximum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xor ("Acceleration factor"), &Config::AimAssist::Algorithmv1::AccelerationFactor, 0, 5, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xor ("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps."));
                    }
                    else if (Config::AimAssist::Algorithm == 1)
                    {
                        Widgets::SliderFloat(xor ("Power"), &Config::AimAssist::Algorithmv2::Power, 0.f, 1.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox(xor ("Assist on sliders##algov2assistonsliders"), &Config::AimAssist::Algorithmv2::AssistOnSliders);
                    }
                    else
                    {
                        Widgets::SliderFloat(xor ("Power"), &Config::AimAssist::Algorithmv3::Power, 0.f, 2.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput);

                        if (Config::AimAssist::Algorithmv3::Power > 1.f)
                            ImGui::TextColored(StyleProvider::AccentColour, xor ("We don't recommend using a power greater than 1."));

                        Widgets::SliderFloat(xor ("Slider Assist Power"), &Config::AimAssist::Algorithmv3::SliderAssistPower, 0.f, 1.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_ClampOnInput);
                    }
                    Widgets::Checkbox(xor ("Show Debug Overlay"), &Config::AimAssist::DrawDebugOverlay);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 2)
            {
                Widgets::BeginPanel(xor ("Timewarp"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &Config::Timewarp::Enabled); ImGui::SameLine(); Widgets::Tooltip(xor ("Slows down or speeds up the game."));
                    const char* types[] = { xor ("Rate"), xor ("Multiplier") };
                    Widgets::Combo(xor ("Type"), &Config::Timewarp::Type, types, IM_ARRAYSIZE(types));
                    if (Config::Timewarp::Type == 0)
                    {
                        Widgets::SliderInt(xor ("Rate"), &Config::Timewarp::Rate, 25, 300, 1, 10, xor ("%d"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xor ("The desired speed of timewarp.\n\nLower value = slower.\nHigher value = faster.\n\n75 is HalfTime.\n100 is NoMod.\n150 is DoubleTime."));
                    }
                    else
                    {
                        Widgets::SliderFloat(xor ("Multiplier"), &Config::Timewarp::Multiplier, 0.25f, 1.5f, .01f, .1f, xor ("%.2f"), ImGuiSliderFlags_AlwaysClamp);
                    }
                }
                Widgets::EndPanel();
            }
            if (currentTab == 3)
            {
                Widgets::BeginPanel(xor ("Replay Bot"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 1)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &ReplayBot::Enabled);

                    if (Widgets::Checkbox(xor ("Disable aiming"), &ReplayBot::DisableAiming))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableTapping = false;
                    }

                    if (Widgets::Checkbox(xor ("Disable tapping"), &ReplayBot::DisableTapping))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableAiming = false;
                    }

                    if (Widgets::Button(xor ("Select replay")))
                    {
                        if (!replayDialogInitialized)
                        {
                            replayDialog.SetTitle(xor ("Select replay"));
                            replayDialog.SetTypeFilters({ xor (".osr") });

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

                    const std::string selectedReplayText = xor ("Selected replay: ") + ReplayBot::GetReplayString();
                    ImGui::Text(selectedReplayText.c_str());
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
            {
                Widgets::BeginPanel(xor ("AR Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &Config::Visuals::ARChanger::Enabled); ImGui::SameLine(); Widgets::Tooltip(xor ("AR is short for Approach Rate and defines when hit objects start to fade in relative to when they should be hit or collected."));
                    Widgets::SliderFloat(xor ("AR"), &Config::Visuals::ARChanger::AR, 0.f, 12.f, .1f, 1.f, xor ("%.1f"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xor ("Higher value = hit objects will be shown for a shorter period of time = less time to react.\n\nLower value = hit objects will be shown for a longer period of time = more time to react."));
                    Widgets::Checkbox(xor ("Adjust to mods"), &Config::Visuals::ARChanger::AdjustToMods); ImGui::SameLine(); Widgets::Tooltip(xor ("If this option is enabled, AR Changer will adjust the AR you have set to currently selected mods.\n\nFor example, if you selected Easy mod, AR will be slightly lower."));
                    Widgets::Checkbox(xor ("Adjust to rate"), &Config::Visuals::ARChanger::AdjustToRate);
                    Widgets::Checkbox(xor ("Draw preemptive dot"), &Config::Visuals::ARChanger::DrawPreemptiveDot);
                    ImGui::ColorEdit4(xor ("Preemptive dot colour"), reinterpret_cast<float*>(&Config::Visuals::ARChanger::PreemptiveDotColour), ImGuiColorEditFlags_NoInputs);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("CS Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, xor ("Don't use this on legit servers!"));
                    Widgets::Checkbox(xor ("Enabled"), &Config::Visuals::CSChanger::Enabled);
                    Widgets::SliderFloat(xor ("CS"), &Config::Visuals::CSChanger::CS, 0.f, 10.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("HD & FL Removers"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox(xor ("Disable Hidden"), &Config::Visuals::Removers::HiddenRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Hidden mod.");
                    Widgets::Checkbox(xor ("Disable Flashlight"), &Config::Visuals::Removers::FlashlightRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Flashlight mod.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("User Interface"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 8 : 7, 0, 2)));
                {
                    Widgets::Checkbox(xor ("Snow"), &Config::Visuals::UI::Snow);

                    const char* scales[] = { xor ("50%"), xor ("75%"), xor ("100%"), xor ("125%"), xor ("150%") };
                    if (Widgets::Combo(xor ("Menu scale"), &Config::Visuals::UI::MenuScale, scales, IM_ARRAYSIZE(scales)))
                        StyleProvider::UpdateScale();

                    ImGui::Spacing();

                    if (Widgets::Button(xor ("Load background image"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        if (!backgroundImageDialogInitialized)
                        {
                            backgroundImageDialog.SetTitle(xor ("Select background image"));
                            backgroundImageDialog.SetTypeFilters({ xor (".png"), xor (".jpg"), xor (".jpeg"), xor (".bmp"), xor (".tga") });

                            backgroundImageDialogInitialized = true;
                        }

                        backgroundImageDialog.Open();
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
                    backgroundImageDialog.Display();
                    ImGui::PopStyleVar();

                    if (backgroundImageDialog.HasSelected())
                    {
                        strcpy_s(Config::Visuals::UI::MenuBackground, backgroundImageDialog.GetSelected().string().c_str());
                        backgroundImageDialog.ClearSelected();

                        updateBackground();
                    }

                    if (backgroundTexture)
                    {
                        if (Widgets::Button(xor ("Remove background image"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        {
                            Config::Visuals::UI::MenuBackground[0] = '\0';

                            updateBackground();
                        }
                    }

                    ImGui::Spacing();

                    bool coloursChanged = false;
                    coloursChanged |= ImGui::ColorEdit4(xor ("Accent colour"), reinterpret_cast<float*>(&Config::Visuals::UI::AccentColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xor ("Menu colour"), reinterpret_cast<float*>(&Config::Visuals::UI::MenuColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xor ("Control colour"), reinterpret_cast<float*>(&Config::Visuals::UI::ControlColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xor ("Text colour"), reinterpret_cast<float*>(&Config::Visuals::UI::TextColour), ImGuiColorEditFlags_NoInputs);

                    if (coloursChanged)
                        StyleProvider::UpdateColours();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 5)
            {
                Widgets::BeginPanel(xor ("Spoofer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 1, 1)));
                {
                    const bool sameProfile = Spoofer::SelectedProfile == Spoofer::LoadedProfile;
                    const bool currentProfileIsDefault = Spoofer::SelectedProfile == 0;

                    const float buttonWidth = (ImGui::GetWindowWidth() * 0.5f - style.ItemSpacing.x) / 2;

                    Widgets::Combo(xor ("Profiles"), &Spoofer::SelectedProfile, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Spoofer::Profiles), Spoofer::Profiles.size());

                    ImGui::Text(xor ("Current profile: %s"), Spoofer::Profiles[Spoofer::LoadedProfile].c_str());

                    if (sameProfile)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xor ("Load"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
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

                    if (Widgets::Button(xor ("Delete"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Spoofer::Delete();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    ImGui::InputText(xor ("Profile name"), Spoofer::NewProfileName, IM_ARRAYSIZE(Spoofer::NewProfileName));

                    if (Widgets::Button(xor ("Create new profile"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Create();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 6)
            {
                Widgets::BeginPanel(xor ("Misc"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    const char* scoreSubmissionTypes[] = { xor ("Allow"), xor ("Disallow"), xor ("Prompt") };
                    Widgets::Combo(xor ("Score submission"), &Config::Misc::ScoreSubmissionType, scoreSubmissionTypes, IM_ARRAYSIZE(scoreSubmissionTypes)); ImGui::SameLine(); Widgets::Tooltip(xor ("Specifies score submission behavior.\n\nAllow: all scores will be sent to osu! servers.\nDisallow: your scores won't be sent to osu! servers.\nPrompt: before submitting a score Maple will ask you whether or not you really want to submit it."));
                    Widgets::Checkbox(xor ("Disable spectators"), &Config::Misc::DisableSpectators); ImGui::SameLine(); Widgets::Tooltip(xor ("Spectators will keep buffering infinitely."));
                    Widgets::Checkbox(xor ("Disable logging"), &Config::Misc::DisableLogging); ImGui::SameLine(); Widgets::Tooltip(xor ("Disables Maple's log output to both console and runtime.log file."));

                    bool storageConfigEdited = false;
                    storageConfigEdited |= Widgets::Checkbox(xor ("Show menu after injection"), &StorageConfig::ShowMenuAfterInjection);
                    storageConfigEdited |= Widgets::Hotkey(xor ("Menu key"), &StorageConfig::MenuKey);

                    if (StorageConfig::MenuKey == 0)
                        StorageConfig::MenuKey = VK_DELETE;

                    if (storageConfigEdited)
                        Storage::SaveStorageConfig();

                    if (Widgets::Button(xor ("Open Maple folder"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        std::wstring wPath = std::wstring(Storage::StorageDirectory.begin(), Storage::StorageDirectory.end());
                        LPCWSTR path = wPath.c_str();

                        ShellExecute(NULL, L"open", path, NULL, NULL, SW_RESTORE);
                    }
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xor ("Discord Rich Presence Spoofer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(11)));
                {
                    Widgets::Checkbox(xor ("Enabled"), &Config::Misc::DiscordRichPresenceSpoofer::Enabled); ImGui::SameLine(); Widgets::Tooltip(xor ("Spoofs various fields of your Discord Game Activity"));

                	Widgets::Checkbox(xor ("Custom large image text"), &Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled);
                    ImGui::InputText(xor ("Large image text"), Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, 128);

                    Widgets::Checkbox(xor ("Custom play mode"), &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled);
                    const char* playModes[] = { xor ("osu!"), xor ("osu!taiko"), xor ("osu!catch"), xor ("osu!mania") };
                    Widgets::Combo(xor ("Play mode"), &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayMode, playModes, IM_ARRAYSIZE(playModes));
					
                    Widgets::Checkbox(xor ("Custom state"), &Config::Misc::DiscordRichPresenceSpoofer::CustomStateEnabled);
                    ImGui::InputText(xor ("State"), Config::Misc::DiscordRichPresenceSpoofer::CustomState, 128);

                    Widgets::Checkbox(xor ("Custom details"), &Config::Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled);
                    ImGui::InputText(xor ("Details"), Config::Misc::DiscordRichPresenceSpoofer::CustomDetails, 128);

                    Widgets::Checkbox(xor ("Hide spectate button"), &Config::Misc::DiscordRichPresenceSpoofer::HideSpectateButton);

                    Widgets::Checkbox(xor ("Hide match button"), &Config::Misc::DiscordRichPresenceSpoofer::HideMatchButton);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 7)
            {
                Widgets::BeginPanel(xor ("Config"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 0, 1)));
                {
                    const float buttonWidth = ((ImGui::GetWindowWidth() * 0.5f) - (style.ItemSpacing.x * 2)) / 3;
                    Widgets::Combo(xor ("Config"), &Config::CurrentConfig, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Config::Configs), Config::Configs.size());

                    if (Widgets::Button(xor ("Load"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                    {
                        Config::Load();

                        updateBackground();
                        StyleProvider::UpdateColours();
                        StyleProvider::UpdateScale();
                    }

                    ImGui::SameLine();

                    if (Config::CurrentConfig == 0)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button(xor ("Save"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Config::Save();

                    if (Config::CurrentConfig == 0)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::SameLine();

                    if (Widgets::Button(xor ("Refresh"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Config::Refresh();

                    ImGui::Spacing();

                    ImGui::InputText(xor ("Config name"), Config::NewConfigName, IM_ARRAYSIZE(Config::NewConfigName));
                    if (Widgets::Button(xor ("Create new config"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        Config::Create();

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
