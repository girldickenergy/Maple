#include "MainMenu.h"

#include <filesystem>
#include <fstream>

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
#include "../../Utilities/Clipboard/ClipboardUtilities.h"

#include "../../Features/ReplayEditor/Editor.h"
#include "../../Sdk/Memory.h"

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

#pragma optimize("", off)
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
    ImGui::Begin(xorstr_("Main Menu"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuSize = ImGui::GetCurrentWindow()->Size;
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + StyleProvider::MainMenuSideBarSize, ImColor(StyleProvider::MenuColourDark), style.WindowRounding, expanded ? ImDrawFlags_RoundCornersAll & ~ImDrawFlags_RoundCornersTopRight : ImDrawFlags_RoundCornersAll);

        ImGui::SetCursorPos(StyleProvider::Padding);
        ImGui::BeginChild(xorstr_("Side Bar"), StyleProvider::MainMenuSideBarSize - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
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

            ImGui::BeginChild(xorstr_("User Info"), ImVec2(sideBarSize.x, StyleProvider::MainMenuUserInfoHeight), false, ImGuiWindowFlags_NoBackground);
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

            ImGui::BeginChild(xorstr_("Tabs"), ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (40 * StyleProvider::Scale) * 8; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild(xorstr_("Tabs##001"), ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
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
                        ReplayEditor::Editor::IsOpen = true;/*currentTab = currentTab == 7 ? -1 : 7;*/

                   /* if (Widgets::Tab(xorstr_("Replay Editor"), StyleProvider::ReplaysIconTexture, currentTab == 8, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                		ReplayEditor::Editor::IsOpen = true;*/

                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            ImGui::BeginChild(xorstr_("Build Info"), ImVec2(sideBarSize.x, StyleProvider::MainMenuBuildInfoHeight), false, ImGuiWindowFlags_NoBackground);
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
                const ImVec2 buildStringSize = ImGui::CalcTextSize(xorstr_("mlo-11122022"));
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, xorstr_("mlo-11122022"));
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(StyleProvider::MainMenuSideBarSize.x, 0) + StyleProvider::Padding);
        ImGui::BeginChild(xorstr_("Options"), ImVec2(StyleProvider::MainMenuSize.x - StyleProvider::MainMenuSideBarSize.x, StyleProvider::MainMenuSize.y) - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushFont(StyleProvider::FontDefault);

            const float optionsWidth = ImGui::GetWindowWidth();

            if (currentTab == 0)
            {
                Widgets::BeginPanel(xorstr_("Relax"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::Relax::Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("All hit objects will be automatically tapped by Maple."));
                    Widgets::Hotkey(xorstr_("Toggle key"), &Config::Relax::ToggleKey); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Allows you to toggle relax mid gameplay."));
                    const char* keys[] = { xorstr_("M1"),xorstr_("K1"),xorstr_("M2"),xorstr_("K2") };
                    Widgets::Combo(xorstr_("Primary key"), &Config::Relax::PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::Combo(xorstr_("Secondary key"), &Config::Relax::SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::SliderInt(xorstr_("Alternate BPM"), &Config::Relax::AlternateBPM, 0, 500, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A BPM at which relax will start alternating."));
                    Widgets::Checkbox(xorstr_("Slider alternation override"), &Config::Relax::SliderAlternationOverride); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Changes the way how alternation of sliders is handled.\nIt is recommended to enable this option on techno maps."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Timing"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(8)));
                {
                    Widgets::SliderInt(xorstr_("Offset"), &Config::Relax::Timing::Offset, -50, 50, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Offsets keypresses by the specified amount of milliseconds.\n\nUseful if you don't want your hits to be centered around 0 ms offset or if you're having latency issues."));
                    Widgets::SliderInt(xorstr_("Target unstable rate"), &Config::Relax::Timing::TargetUnstableRate, 0, 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
                    Widgets::SliderInt(xorstr_("Allowable hit range"), &Config::Relax::Timing::AllowableHitRange, 0, 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput);
                    Widgets::HitErrorBar(Config::Relax::Timing::AllowableHitRange);
                    Widgets::SliderInt(xorstr_("Minimum hold time"), &Config::Relax::Timing::MinimumHoldTime, 25, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A minimum duration of a keypress."));
                    Widgets::SliderInt(xorstr_("Maximum hold time"), &Config::Relax::Timing::MaximumHoldTime, 25, 150, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum duration of a keypress."));
                    Widgets::SliderInt(xorstr_("Minimum slider hold time"), &Config::Relax::Timing::MinimumSliderHoldTime, 25, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A minimum duration of a keypress for sliders and spinners."));
                    Widgets::SliderInt(xorstr_("Maximum slider hold time"), &Config::Relax::Timing::MaximumSliderHoldTime, 25, 150, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum duration of a keypress for sliders and spinners."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Hit Scan"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox(xorstr_("Direction prediction"), &Config::Relax::HitScan::DirectionPredictionEnabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Predicts whether or not you're leaving the circle and clicks if you are."));
                    Widgets::SliderInt(xorstr_("Direction prediction angle"), &Config::Relax::HitScan::DirectionPredictionAngle, 0, 90, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("A maximum angle between current cursor position, last cursor position and next circle position for prediction to trigger.\n\nLower value = worse prediction."));
                    Widgets::SliderFloat(xorstr_("Direction prediction scale"), &Config::Relax::HitScan::DirectionPredictionScale, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies a portion of the circle where prediction will trigger.\n\n0 = full circle."));
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Blatant"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(1, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Don't use this on legit servers!"));
                    Widgets::Checkbox(xorstr_("Use lowest possible hold times"), &Config::Relax::Blatant::UseLowestPossibleHoldTimes);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 1)
            {
                if (Config::AimAssist::Algorithm == 0)
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
                Widgets::BeginPanel(xorstr_("Aim Assist"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(Config::AimAssist::Algorithm == 0 ? 10 : 5, Config::AimAssist::Algorithm == 2 && Config::AimAssist::Algorithmv3::Power > 1.f ? 1 : 0)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::AimAssist::Enabled);
                    const char* algorithms[] = { xorstr_("v1"), xorstr_("v2"), xorstr_("v3") };
                    Widgets::Combo(xorstr_("Algorithm"), &Config::AimAssist::Algorithm, algorithms, IM_ARRAYSIZE(algorithms));
                    if (Config::AimAssist::Algorithm == 0)
                    {
                        Widgets::SliderFloat(xorstr_("Strength##algov1strength"), &Config::AimAssist::Algorithmv1::Strength, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the Aim Assist strength, change this value according to how strong you want to be helped with."));
                        Widgets::Checkbox(xorstr_("Assist on sliders##algov1assistonsliders"), &Config::AimAssist::Algorithmv1::AssistOnSliders); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Do you need help on sliders?\nYes?\nTurn this on then."));
                        Widgets::SliderInt(xorstr_("Base FOV##algov1basefov"), &Config::AimAssist::Algorithmv1::BaseFOV, 0, 100, 1, 10, xorstr_("%d"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps."));
                        Widgets::SliderFloat(xorstr_("Maximum FOV (Scaling)##algov1maxfovscale"), &Config::AimAssist::Algorithmv1::MaximumFOVScale, 0, 5, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Minimum FOV (Total)##algov1minfovtotal"), &Config::AimAssist::Algorithmv1::MinimumFOVTotal, 0, 100, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the total minimum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Maximum FOV (Total)##algov1maxfovtotal"), &Config::AimAssist::Algorithmv1::MaximumFOVTotal, 0, 500, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Sets the total maximum FOV of the Aim Assist."));
                        Widgets::SliderFloat(xorstr_("Acceleration factor"), &Config::AimAssist::Algorithmv1::AccelerationFactor, 0, 5, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps."));
                    }
                    else if (Config::AimAssist::Algorithm == 1)
                    {
                        Widgets::SliderFloat(xorstr_("Power"), &Config::AimAssist::Algorithmv2::Power, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox(xorstr_("Assist on sliders##algov2assistonsliders"), &Config::AimAssist::Algorithmv2::AssistOnSliders);
                    }
                    else
                    {
                        Widgets::SliderFloat(xorstr_("Power"), &Config::AimAssist::Algorithmv3::Power, 0.f, 2.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);

                        if (Config::AimAssist::Algorithmv3::Power > 1.f)
                            ImGui::TextColored(StyleProvider::AccentColour, xorstr_("We don't recommend using a power greater than 1."));

                        Widgets::SliderFloat(xorstr_("Slider Assist Power"), &Config::AimAssist::Algorithmv3::SliderAssistPower, 0.f, 1.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_ClampOnInput);
                    }
                    Widgets::Checkbox(xorstr_("Show Debug Overlay"), &Config::AimAssist::DrawDebugOverlay);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 2)
            {
                Widgets::BeginPanel(xorstr_("Timewarp"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::Timewarp::Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Slows down or speeds up the game."));
                    const char* types[] = { xorstr_("Rate"), xorstr_("Multiplier") };
                    Widgets::Combo(xorstr_("Type"), &Config::Timewarp::Type, types, IM_ARRAYSIZE(types));
                    if (Config::Timewarp::Type == 0)
                    {
                        Widgets::SliderInt(xorstr_("Rate"), &Config::Timewarp::Rate, 25, 300, 1, 10, xorstr_("%d"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xorstr_("The desired speed of timewarp.\n\nLower value = slower.\nHigher value = faster.\n\n75 is HalfTime.\n100 is NoMod.\n150 is DoubleTime."));
                    }
                    else
                    {
                        Widgets::SliderFloat(xorstr_("Multiplier"), &Config::Timewarp::Multiplier, 0.25f, 1.5f, .01f, .1f, xorstr_("%.2f"), ImGuiSliderFlags_AlwaysClamp);
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

                Widgets::BeginPanel(xorstr_("Replay Editor"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(1, 1)));
                {
                    bool jitted = Memory::Objects[xorstr_("RulesetOsu::CreateHitObjectManager")] != 0x00000000;
                    if (jitted)
                    {
                        if (Widgets::Button(xorstr_("Open Replay Editor")))
                        {
                            ReplayEditor::Editor::IsOpen = true;
                        }
                    }else
                    {
                        ImGui::Text("Please start one beatmap before using the Replay Editor");
                    }
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
            {
                Widgets::BeginPanel(xorstr_("AR Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::Visuals::ARChanger::Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("AR is short for Approach Rate and defines when hit objects start to fade in relative to when they should be hit or collected."));
                    Widgets::SliderFloat(xorstr_("AR"), &Config::Visuals::ARChanger::AR, 0.f, 12.f, .1f, 1.f, xorstr_("%.1f"), ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Higher value = hit objects will be shown for a shorter period of time = less time to react.\n\nLower value = hit objects will be shown for a longer period of time = more time to react."));
                    Widgets::Checkbox(xorstr_("Adjust to mods"), &Config::Visuals::ARChanger::AdjustToMods); ImGui::SameLine(); Widgets::Tooltip(xorstr_("If this option is enabled, AR Changer will adjust the AR you have set to currently selected mods.\n\nFor example, if you selected Easy mod, AR will be slightly lower."));
                    Widgets::Checkbox(xorstr_("Adjust to rate"), &Config::Visuals::ARChanger::AdjustToRate);
                    Widgets::Checkbox(xorstr_("Draw preemptive dot"), &Config::Visuals::ARChanger::DrawPreemptiveDot);
                    ImGui::ColorEdit4(xorstr_("Preemptive dot colour"), reinterpret_cast<float*>(&Config::Visuals::ARChanger::PreemptiveDotColour), ImGuiColorEditFlags_NoInputs);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("CS Changer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, xorstr_("Don't use this on legit servers!"));
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::Visuals::CSChanger::Enabled);
                    Widgets::SliderFloat(xorstr_("CS"), &Config::Visuals::CSChanger::CS, 0.f, 10.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("HD & FL Removers"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox(xorstr_("Disable Hidden"), &Config::Visuals::Removers::HiddenRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Hidden mod.");
                    Widgets::Checkbox(xorstr_("Disable Flashlight"), &Config::Visuals::Removers::FlashlightRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Flashlight mod.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("User Interface"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 8 : 7, 0, 2)));
                {
                    Widgets::Checkbox(xorstr_("Snow"), &Config::Visuals::UI::Snow);

                    const char* scales[] = { xorstr_("50%"), xorstr_("75%"), xorstr_("100%"), xorstr_("125%"), xorstr_("150%") };
                    if (Widgets::Combo(xorstr_("Menu scale"), &Config::Visuals::UI::MenuScale, scales, IM_ARRAYSIZE(scales)))
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
                        strcpy_s(Config::Visuals::UI::MenuBackground, backgroundImageDialog.GetSelected().string().c_str());
                        backgroundImageDialog.ClearSelected();

                        updateBackground();
                    }

                    if (backgroundTexture)
                    {
                        if (Widgets::Button(xorstr_("Remove background image"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        {
                            Config::Visuals::UI::MenuBackground[0] = '\0';

                            updateBackground();
                        }
                    }

                    ImGui::Spacing();

                    bool coloursChanged = false;
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Accent colour"), reinterpret_cast<float*>(&Config::Visuals::UI::AccentColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Menu colour"), reinterpret_cast<float*>(&Config::Visuals::UI::MenuColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Control colour"), reinterpret_cast<float*>(&Config::Visuals::UI::ControlColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4(xorstr_("Text colour"), reinterpret_cast<float*>(&Config::Visuals::UI::TextColour), ImGuiColorEditFlags_NoInputs);

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

                    Widgets::Combo(xorstr_("Profiles"), &Spoofer::SelectedProfile, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Spoofer::Profiles), Spoofer::Profiles.size());

                    ImGui::Text(xorstr_("Current profile: %s"), Spoofer::Profiles[Spoofer::LoadedProfile].c_str());

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

                    ImGui::InputText(xorstr_("Profile name##profilerename"), Spoofer::RenamedProfileName, IM_ARRAYSIZE(Spoofer::RenamedProfileName));
                    if (Widgets::Button(xorstr_("Rename selected profile"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Rename();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    ImGui::InputText(xorstr_("Profile name##newprofile"), Spoofer::NewProfileName, IM_ARRAYSIZE(Spoofer::NewProfileName));
                    if (Widgets::Button(xorstr_("Create new profile"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Create();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 6)
            {
                Widgets::BeginPanel(xorstr_("Misc"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(Config::Misc::ScoreSubmissionType == 2 ? 5 : 4)));
                {
                    const char* scoreSubmissionTypes[] = { xorstr_("Allow"), xorstr_("Disallow"), xorstr_("Prompt") };
                    Widgets::Combo(xorstr_("Score submission"), &Config::Misc::ScoreSubmissionType, scoreSubmissionTypes, IM_ARRAYSIZE(scoreSubmissionTypes)); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies score submission behavior.\n\nAllow: all scores will be sent to osu! servers.\nDisallow: your scores won't be sent to osu! servers.\nPrompt: before submitting a score Maple will ask you whether or not you really want to submit it."));
                    if (Config::Misc::ScoreSubmissionType == 2)
                    {
                        const char* promptBehaviors[] = { xorstr_("Submit"), xorstr_("Don't submit") };
						Widgets::Combo(xorstr_("Behavior on retry"), &Config::Misc::PromptBehaviorOnRetry, promptBehaviors, IM_ARRAYSIZE(promptBehaviors)); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Specifies what Maple should do with your score when you retry a map."));
                    }
					
                    Widgets::Checkbox(xorstr_("Disable spectators"), &Config::Misc::DisableSpectators); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Spectators will keep buffering infinitely."));

                    bool storageConfigEdited = false;
                    storageConfigEdited |= Widgets::Checkbox(xorstr_("Show menu after injection"), &StorageConfig::ShowMenuAfterInjection);
                    storageConfigEdited |= Widgets::Hotkey(xorstr_("Menu key"), &StorageConfig::MenuKey);

                    if (StorageConfig::MenuKey == 0)
                        StorageConfig::MenuKey = VK_DELETE;

                    if (storageConfigEdited)
                        Storage::SaveStorageConfig();
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Logging"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox(xorstr_("Disable logging"), &Config::Misc::Logging::DisableLogging); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Disables Maple's log output to both console and runtime.log file."));
                    if (Widgets::Button(xorstr_("Copy runtime log to clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        std::ifstream ifs(Storage::LogsDirectory + xorstr_("\\runtime.log"));
                        const std::string logData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
                        ifs.close();

                        ClipboardUtilities::Write(logData);
                    }
                }
                Widgets::EndPanel();

                Widgets::BeginPanel(xorstr_("Discord Rich Presence Spoofer"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(11)));
                {
                    Widgets::Checkbox(xorstr_("Enabled"), &Config::Misc::DiscordRichPresenceSpoofer::Enabled); ImGui::SameLine(); Widgets::Tooltip(xorstr_("Spoofs various fields of your Discord Game Activity"));

                	Widgets::Checkbox(xorstr_("Custom large image text"), &Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled);
                    ImGui::InputText(xorstr_("Large image text"), Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, 128);

                    Widgets::Checkbox(xorstr_("Custom play mode"), &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled);
                    const char* playModes[] = { xorstr_("osu!"), xorstr_("osu!taiko"), xorstr_("osu!catch"), xorstr_("osu!mania") };
                    Widgets::Combo(xorstr_("Play mode"), &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayMode, playModes, IM_ARRAYSIZE(playModes));
					
                    Widgets::Checkbox(xorstr_("Custom state"), &Config::Misc::DiscordRichPresenceSpoofer::CustomStateEnabled);
                    ImGui::InputText(xorstr_("State"), Config::Misc::DiscordRichPresenceSpoofer::CustomState, 128);

                    Widgets::Checkbox(xorstr_("Custom details"), &Config::Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled);
                    ImGui::InputText(xorstr_("Details"), Config::Misc::DiscordRichPresenceSpoofer::CustomDetails, 128);

                    Widgets::Checkbox(xorstr_("Hide spectate button"), &Config::Misc::DiscordRichPresenceSpoofer::HideSpectateButton);

                    Widgets::Checkbox(xorstr_("Hide match button"), &Config::Misc::DiscordRichPresenceSpoofer::HideMatchButton);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 7)
            {
                Widgets::BeginPanel(xorstr_("Config"), ImVec2(optionsWidth, Widgets::CalcPanelHeight(8, 0, 2)));
                {
                    const bool currentConfigIsDefault = Config::CurrentConfig == 0;

                    const float buttonWidth = ((ImGui::GetWindowWidth() * 0.5f) - (style.ItemSpacing.x * 2)) / 3;
                    Widgets::Combo(xorstr_("Config"), &Config::CurrentConfig, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Config::Configs), Config::Configs.size());

                    if (Widgets::Button(xorstr_("Load"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                    {
                        Config::Load();

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
                        Config::Save();

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
                        Config::Delete();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    if (Widgets::Button(xorstr_("Import from clipboard"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        Config::Import();

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
                        Config::Export();

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

                    ImGui::InputText(xorstr_("Config name##configrename"), Config::RenamedConfigName, IM_ARRAYSIZE(Config::RenamedConfigName));
                    if (Widgets::Button(xorstr_("Rename selected config"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Config::Rename();

                    if (currentConfigIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    ImGui::InputText(xorstr_("Config name##newconfig"), Config::NewConfigName, IM_ARRAYSIZE(Config::NewConfigName));
                    if (Widgets::Button(xorstr_("Create new config"), ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
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

    if (Config::Misc::ForceDisableScoreSubmission)
    {
        ImGui::PushFont(StyleProvider::FontDefaultBold);
        const ImVec2 panelHeaderLabelSize = ImGui::CalcTextSize(xorstr_("Attention!"));
        const float panelHeaderHeight = panelHeaderLabelSize.y + StyleProvider::Padding.y * 2;
        ImGui::PopFont();

        ImGui::PushFont(StyleProvider::FontDefault);

        const float panelContentHeight = Widgets::CalcPanelHeight(1, 3);
        const ImVec2 windowSize = ImVec2(ImGui::CalcTextSize(Config::Misc::BypassFailed ? xorstr_("Score submission has been disabled to prevent your account from being flagged or banned.") : xorstr_("Your osu! client is running a newer version of the anti-cheat, which has not yet been confirmed to be safe.")).x, panelHeaderHeight + panelContentHeight) + StyleProvider::Padding * 2;

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(GameBase::GetClientPosition().X + GameBase::GetClientSize().X / 2 - windowSize.x / 2, GameBase::GetClientPosition().Y + GameBase::GetClientSize().Y - windowSize.y - StyleProvider::Padding.y));
        ImGui::Begin(xorstr_("AuthNotice"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        {
            Widgets::BeginPanel(xorstr_("Attention!"), ImVec2(windowSize.x, panelContentHeight));
            {
                if (Config::Misc::BypassFailed)
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
                    Config::Misc::ForceDisableScoreSubmission = false;
                }
            }
            Widgets::EndPanel();
        }
        ImGui::End();

        ImGui::PopFont();
    }
}
#pragma optimize("", on)

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
