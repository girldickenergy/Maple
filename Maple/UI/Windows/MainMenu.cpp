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
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuSize = ImGui::GetCurrentWindow()->Size;
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + StyleProvider::MainMenuSideBarSize, ImColor(StyleProvider::MenuColourDark), style.WindowRounding, expanded ? ImDrawFlags_RoundCornersAll & ~ImDrawFlags_RoundCornersTopRight : ImDrawFlags_RoundCornersAll);

        ImGui::SetCursorPos(StyleProvider::Padding);
        ImGui::BeginChild("Side Bar", StyleProvider::MainMenuSideBarSize - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
        {
            const ImVec2 sideBarSize = ImGui::GetCurrentWindow()->Size;

            ImGui::PushFont(StyleProvider::FontHugeBold);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5 * StyleProvider::Scale, 10 * StyleProvider::Scale));
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ((ImGui::CalcTextSize("Maple").x / 2) + StyleProvider::MapleLogoSize.x / 2 + style.ItemSpacing.x / 2));
            ImGui::Image(StyleProvider::MapleLogoTexture, StyleProvider::MapleLogoSize, ImVec2(0, 0), ImVec2(1, 1), StyleProvider::AccentColour);
            ImGui::SameLine();
            ImGui::TextColored(StyleProvider::AccentColour, "Maple");
            ImGui::PopStyleVar();
            ImGui::PopFont();
            ImGui::PushFont(StyleProvider::FontSmall);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style.ItemSpacing.y);
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ImGui::CalcTextSize("the quickest way to the top").x / 2);
            ImGui::TextColored(StyleProvider::MottoColour, "the quickest way to the top");
            ImGui::PopFont();

            ImGui::Spacing();

            ImGui::BeginChild("User Info", ImVec2(sideBarSize.x, StyleProvider::MainMenuUserInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 userInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 userInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(userInfoPos, userInfoPos + userInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::GetWindowDrawList()->AddImageRounded(StyleProvider::AvatarTexture, userInfoPos + ImVec2(userInfoSize.y / 4, userInfoSize.y / 4), userInfoPos + ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2, userInfoSize.y / 4 + userInfoSize.y / 2), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), style.FrameRounding);

                ImGui::PushFont(StyleProvider::FontDefaultBold);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 - style.ItemSpacing.y / 4 - ImGui::CalcTextSize("Welcome back").y));
                ImGui::Text("Welcome back");
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontDefault);
                ImGui::SetCursorPos(ImVec2(userInfoSize.y / 4 + userInfoSize.y / 2 + style.ItemSpacing.x, userInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::AccentColour, "dev");//Communication::CurrentUser->Username.c_str());
                ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::BeginChild("Tabs", ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (40 * StyleProvider::Scale) * 8; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild("Tabs##001", ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 40 * StyleProvider::Scale);

                    ImGui::PushFont(StyleProvider::FontDefaultBold);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                    if (Widgets::Tab("Relax", StyleProvider::RelaxIconTexture, currentTab == 0, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 0 ? -1 : 0;

                    if (Widgets::Tab("Aim Assist", StyleProvider::AimAssistIconTexture, currentTab == 1, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 1 ? -1 : 1;

                    if (Widgets::Tab("Timewarp", StyleProvider::TimewarpIconTexture, currentTab == 2, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 2 ? -1 : 2;

                    if (Widgets::Tab("Replays", StyleProvider::ReplaysIconTexture, currentTab == 3, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 3 ? -1 : 3;

                    if (Widgets::Tab("Visuals", StyleProvider::VisualsIconTexture, currentTab == 4, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;

                    if (Widgets::Tab("Spoofer", StyleProvider::SpooferIconTexture, currentTab == 5, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;

                    if (Widgets::Tab("Misc", StyleProvider::MiscIconTexture, currentTab == 6, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 6 ? -1 : 6;

                    if (Widgets::Tab("Config", StyleProvider::ConfigIconTexture, currentTab == 7, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 7 ? -1 : 7;

                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            ImGui::BeginChild("Build Info", ImVec2(sideBarSize.x, StyleProvider::MainMenuBuildInfoHeight), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 buildInfoPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 buildInfoSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(buildInfoPos, buildInfoPos + buildInfoSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                ImGui::PushFont(StyleProvider::FontSmallBold);
                const ImVec2 cheatInfoSize = ImGui::CalcTextSize("Maple Lite for osu!");
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - cheatInfoSize.x / 2, buildInfoSize.y / 2 - style.ItemSpacing.y / 4 - cheatInfoSize.y));
                ImGui::TextColored(StyleProvider::AccentColour, "Maple Lite for osu!");
                ImGui::PopFont();

                ImGui::PushFont(StyleProvider::FontSmall);
                const ImVec2 buildStringSize = ImGui::CalcTextSize("l16072022");
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, "l16072022");
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(StyleProvider::MainMenuSideBarSize.x, 0) + StyleProvider::Padding);
        ImGui::BeginChild("Options", ImVec2(StyleProvider::MainMenuSize.x - StyleProvider::MainMenuSideBarSize.x, StyleProvider::MainMenuSize.y) - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushFont(StyleProvider::FontDefault);

            const float optionsWidth = ImGui::GetWindowWidth();

            if (currentTab == 0)
            {
                Widgets::BeginPanel("Relax", ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox("Enabled", &Config::Relax::Enabled); ImGui::SameLine(); Widgets::Tooltip("All hit objects will be automatically tapped by Maple.");
                    Widgets::Hotkey("Toggle key", &Config::Relax::ToggleKey); ImGui::SameLine(); Widgets::Tooltip("Allows you to toggle relax mid gameplay.");
                    const char* keys[] = { "M1", "K1", "M2", "K2" };
                    Widgets::Combo("Primary key", &Config::Relax::PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::Combo("Secondary key", &Config::Relax::SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::SliderInt("Alternate BPM", &Config::Relax::AlternateBPM, 0, 500, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("A BPM at which relax will start alternating.");
                    Widgets::Checkbox("Slider alternation override", &Config::Relax::SliderAlternationOverride); ImGui::SameLine(); Widgets::Tooltip("Changes the way how alternation of sliders is handled.\nIt is recommended to enable this option on techno maps.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Timing", ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::SliderInt("Offset", &Config::Relax::Timing::Offset, -50, 50, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Offsets keypresses by the specified amount of milliseconds.\n\nUseful if you don't want your hits to be centered around 0 ms offset or if you're having latency issues.");
                    Widgets::SliderInt("Target unstable rate", &Config::Relax::Timing::TargetUnstableRate, 0, 300, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput);

                    if (Widgets::SliderInt("Average hold time", &Config::Relax::Timing::AverageHoldTime, 25, 100, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageHoldTime - Config::Relax::Timing::AverageHoldTimeError < 25)
                            Config::Relax::Timing::AverageHoldTimeError = Config::Relax::Timing::AverageHoldTime - 25;

                    ImGui::SameLine(); Widgets::Tooltip("An average duration of a keypress.");

                    if (Widgets::SliderInt("Average hold time error", &Config::Relax::Timing::AverageHoldTimeError, 0, 75, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageHoldTime - Config::Relax::Timing::AverageHoldTimeError < 25)
                            Config::Relax::Timing::AverageHoldTime = Config::Relax::Timing::AverageHoldTimeError + 25;

                    ImGui::SameLine(); Widgets::Tooltip("An average deviation from the duration of a keypress.");

                    if (Widgets::SliderInt("Average slider hold time", &Config::Relax::Timing::AverageSliderHoldTime, 25, 100, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageSliderHoldTime - Config::Relax::Timing::AverageSliderHoldTimeError < 25)
                            Config::Relax::Timing::AverageSliderHoldTimeError = Config::Relax::Timing::AverageSliderHoldTime - 25;

                    ImGui::SameLine(); Widgets::Tooltip("An average duration of a keypress for sliders and spinners.");

                    if (Widgets::SliderInt("Average slider hold time error", &Config::Relax::Timing::AverageSliderHoldTimeError, 0, 75, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput))
                        if (Config::Relax::Timing::AverageSliderHoldTime - Config::Relax::Timing::AverageSliderHoldTimeError < 25)
                            Config::Relax::Timing::AverageSliderHoldTime = Config::Relax::Timing::AverageSliderHoldTimeError + 25;

                    ImGui::SameLine(); Widgets::Tooltip("An average deviation from the duration of a keypress for sliders and spinners.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Hit Scan", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    Widgets::Checkbox("Wait late", &Config::Relax::HitScan::WaitLateEnabled); ImGui::SameLine(); Widgets::Tooltip("Delays a keypress if you failed to aim a hitobject in time.");
                    Widgets::Checkbox("Direction prediction", &Config::Relax::HitScan::DirectionPredictionEnabled); ImGui::SameLine(); Widgets::Tooltip("Predicts whether or not you're leaving the circle and clicks if you are.");
                    Widgets::SliderInt("Direction prediction angle", &Config::Relax::HitScan::DirectionPredictionAngle, 0, 90, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("A maximum angle between current cursor position, last cursor position and next circle position for prediction to trigger.\n\nLower value = worse prediction.");
                    Widgets::SliderFloat("Direction prediction scale", &Config::Relax::HitScan::DirectionPredictionScale, 0.f, 1.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Specifies a portion of the circle where prediction will trigger.\n\n0 = full circle.");
                }
                Widgets::EndPanel();
            }
            if (currentTab == 1)
            {
                if (Config::AimAssist::Algorithm == 0)
                {
                    Widgets::BeginPanel("Attention", ImVec2(optionsWidth, Widgets::CalcPanelHeight(0, 4)));
                    {
                        ImGui::TextColored(StyleProvider::AccentColour, "This algorithm can lead to technical bugs and teleportations on");
                        ImGui::TextColored(StyleProvider::AccentColour, "certain settings.");
                        ImGui::TextColored(StyleProvider::AccentColour, "We strongly recommend you to use the other algorithms");
                        ImGui::TextColored(StyleProvider::AccentColour, "unless you're able to configure it to look as legit as possible.");
                    }
                    Widgets::EndPanel();

                    ImGui::Spacing();
                }
                Widgets::BeginPanel("Aim Assist", ImVec2(optionsWidth, Widgets::CalcPanelHeight(Config::AimAssist::Algorithm == 0 ? 10 : 5, Config::AimAssist::Algorithm == 2 && Config::AimAssist::Algorithmv3::Power > 1.f ? 1 : 0)));
                {
                    Widgets::Checkbox("Enabled", &Config::AimAssist::Enabled);
                    const char* algorithms[] = { "v1", "v2", "v3" };
                    Widgets::Combo("Algorithm", &Config::AimAssist::Algorithm, algorithms, IM_ARRAYSIZE(algorithms));
                    if (Config::AimAssist::Algorithm == 0)
                    {
                        Widgets::SliderFloat("Strength##algov1strength", &Config::AimAssist::Algorithmv1::Strength, 0.f, 1.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Sets the Aim Assist strength, change this value according to how strong you want to be helped with.");
                        Widgets::Checkbox("Assist on sliders##algov1assistonsliders", &Config::AimAssist::Algorithmv1::AssistOnSliders); ImGui::SameLine(); Widgets::Tooltip("Do you need help on sliders?\nYes?\nTurn this on then.");
                        Widgets::SliderInt("Base FOV##algov1basefov", &Config::AimAssist::Algorithmv1::BaseFOV, 0, 100, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps.");
                        Widgets::SliderFloat("Maximum FOV (Scaling)##algov1maxfovscale", &Config::AimAssist::Algorithmv1::MaximumFOVScale, 0, 5, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist.");
                        Widgets::SliderFloat("Minimum FOV (Total)##algov1minfovtotal", &Config::AimAssist::Algorithmv1::MinimumFOVTotal, 0, 100, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Sets the total minimum FOV of the Aim Assist.");
                        Widgets::SliderFloat("Maximum FOV (Total)##algov1maxfovtotal", &Config::AimAssist::Algorithmv1::MaximumFOVTotal, 0, 500, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Sets the total maximum FOV of the Aim Assist.");
                        Widgets::SliderFloat("Acceleration factor", &Config::AimAssist::Algorithmv1::AccelerationFactor, 0, 5, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); ImGui::SameLine(); Widgets::Tooltip("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps.");
                    }
                    else if (Config::AimAssist::Algorithm == 1)
                    {
                        Widgets::SliderFloat("Power", &Config::AimAssist::Algorithmv2::Power, 0.f, 1.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox("Assist on sliders##algov2assistonsliders", &Config::AimAssist::Algorithmv2::AssistOnSliders);
                    }
                    else
                    {
                        Widgets::SliderFloat("Power", &Config::AimAssist::Algorithmv3::Power, 0.f, 2.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput);

                        if (Config::AimAssist::Algorithmv3::Power > 1.f)
                            ImGui::TextColored(StyleProvider::AccentColour, "We don't recommend using a power greater than 1.");

                        Widgets::SliderFloat("Slider Assist Power", &Config::AimAssist::Algorithmv3::SliderAssistPower, 0.f, 1.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput);
                    }
                    Widgets::Checkbox("Show Debug Overlay", &Config::AimAssist::DrawDebugOverlay);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 2)
            {
                Widgets::BeginPanel("Timewarp", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::Timewarp::Enabled); ImGui::SameLine(); Widgets::Tooltip("Slows down or speeds up the game.");
                    const char* types[] = { "Rate", "Multiplier" };
                    Widgets::Combo("Type", &Config::Timewarp::Type, types, IM_ARRAYSIZE(types));
                    if (Config::Timewarp::Type == 0)
                    {
                        Widgets::SliderInt("Rate", &Config::Timewarp::Rate, 25, 300, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip("The desired speed of timewarp.\n\nLower value = slower.\nHigher value = faster.\n\n75 is HalfTime.\n100 is NoMod.\n150 is DoubleTime.");
                    }
                    else
                    {
                        Widgets::SliderFloat("Multiplier", &Config::Timewarp::Multiplier, 0.25f, 1.5f, .01f, .1f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    }
                }
                Widgets::EndPanel();
            }
            if (currentTab == 3)
            {
                Widgets::BeginPanel("Replay Bot", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 1)));
                {
                    Widgets::Checkbox("Enabled", &ReplayBot::Enabled);

                    if (Widgets::Checkbox("Disable aiming", &ReplayBot::DisableAiming))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableTapping = false;
                    }

                    if (Widgets::Checkbox("Disable tapping", &ReplayBot::DisableTapping))
                    {
                        if (ReplayBot::DisableAiming && ReplayBot::DisableTapping)
                            ReplayBot::DisableAiming = false;
                    }

                    if (Widgets::Button("Select replay"))
                    {
                        if (!replayDialogInitialized)
                        {
                            replayDialog.SetTitle("Select replay");
                            replayDialog.SetTypeFilters({ ".osr" });

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

                    std::string selectedReplayText = "Selected replay: " + ReplayBot::GetReplayString();
                    ImGui::Text(selectedReplayText.c_str());
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
            {
                Widgets::BeginPanel("AR Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox("Enabled", &Config::Visuals::ARChanger::Enabled); ImGui::SameLine(); Widgets::Tooltip("AR is short for Approach Rate and defines when hit objects start to fade in relative to when they should be hit or collected.");
                    Widgets::SliderFloat("AR", &Config::Visuals::ARChanger::AR, 0.f, 12.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); ImGui::SameLine(); Widgets::Tooltip("Higher value = hit objects will be shown for a shorter period of time = less time to react.\n\nLower value = hit objects will be shown for a longer period of time = more time to react.");
                    Widgets::Checkbox("Adjust to mods", &Config::Visuals::ARChanger::AdjustToMods); ImGui::SameLine(); Widgets::Tooltip("If this option is enabled, AR Changer will adjust the AR you have set to currently selected mods.\n\nFor example, if you selected Easy mod, AR will be slightly lower.");
                    Widgets::Checkbox("Adjust to rate", &Config::Visuals::ARChanger::AdjustToRate);
                    Widgets::Checkbox("Draw preemptive dot", &Config::Visuals::ARChanger::DrawPreemptiveDot);
                    ImGui::ColorEdit4("Preemptive dot colour", reinterpret_cast<float*>(&Config::Visuals::ARChanger::PreemptiveDotColour), ImGuiColorEditFlags_NoInputs);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("CS Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, "Don't use this on legit servers!");
                    Widgets::Checkbox("Enabled", &Config::Visuals::CSChanger::Enabled);
                    Widgets::SliderFloat("CS", &Config::Visuals::CSChanger::CS, 0.f, 10.f, .1f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("HD & FL Removers", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox("Disable Hidden", &Config::Visuals::Removers::HiddenRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Hidden mod.");
                    Widgets::Checkbox("Disable Flashlight", &Config::Visuals::Removers::FlashlightRemoverEnabled); ImGui::SameLine(); Widgets::Tooltip("Disables Flashlight mod.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("User Interface", ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 8 : 7, 0, 2)));
                {
                    Widgets::Checkbox("Snow", &Config::Visuals::UI::Snow);

                    const char* scales[] = { "50%", "75%", "100%", "125%", "150%" };
                    if (Widgets::Combo("Menu scale", &Config::Visuals::UI::MenuScale, scales, IM_ARRAYSIZE(scales)))
                        StyleProvider::UpdateScale();

                    ImGui::Spacing();

                    if (Widgets::Button("Load background image", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        if (!backgroundImageDialogInitialized)
                        {
                            backgroundImageDialog.SetTitle("Select background image");
                            backgroundImageDialog.SetTypeFilters({ ".png", ".jpg", ".jpeg", ".bmp", ".tga" });

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
                        if (Widgets::Button("Remove background image", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        {
                            Config::Visuals::UI::MenuBackground[0] = '\0';

                            updateBackground();
                        }
                    }

                    ImGui::Spacing();

                    bool coloursChanged = false;
                    coloursChanged |= ImGui::ColorEdit4("Accent colour", reinterpret_cast<float*>(&Config::Visuals::UI::AccentColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Menu colour", reinterpret_cast<float*>(&Config::Visuals::UI::MenuColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Control colour", reinterpret_cast<float*>(&Config::Visuals::UI::ControlColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Text colour", reinterpret_cast<float*>(&Config::Visuals::UI::TextColour), ImGuiColorEditFlags_NoInputs);

                    if (coloursChanged)
                        StyleProvider::UpdateColours();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 5)
            {
                Widgets::BeginPanel("Spoofer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 1, 1)));
                {
                    const bool sameProfile = Spoofer::SelectedProfile == Spoofer::LoadedProfile;
                    const bool currentProfileIsDefault = Spoofer::SelectedProfile == 0;

                    const float buttonWidth = (ImGui::GetWindowWidth() * 0.5f - style.ItemSpacing.x) / 2;

                    Widgets::Combo("Profiles", &Spoofer::SelectedProfile, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Spoofer::Profiles), Spoofer::Profiles.size());

                    ImGui::Text("Current profile: %s", Spoofer::Profiles[Spoofer::LoadedProfile].c_str());

                    if (sameProfile)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                    }

                    if (Widgets::Button("Load", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
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

                    if (Widgets::Button("Delete", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Spoofer::Delete();

                    if (currentProfileIsDefault)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::Spacing();

                    ImGui::InputText("Profile name", Spoofer::NewProfileName, IM_ARRAYSIZE(Spoofer::NewProfileName));

                    if (Widgets::Button("Create new profile", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        Spoofer::Create();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 6)
            {
                Widgets::BeginPanel("Misc", ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    const char* scoreSubmissionTypes[] = { "Allow", "Disallow", "Prompt" };
                    Widgets::Combo("Score submission", &Config::Misc::ScoreSubmissionType, scoreSubmissionTypes, IM_ARRAYSIZE(scoreSubmissionTypes)); ImGui::SameLine(); Widgets::Tooltip("Specifies score submission behavior.\n\nAllow: all scores will be sent to osu! servers.\nDisallow: your scores won't be sent to osu! servers.\nPrompt: before submitting a score Maple will ask you whether or not you really want to submit it.");
                    Widgets::Checkbox("Disable spectators", &Config::Misc::DisableSpectators); ImGui::SameLine(); Widgets::Tooltip("Spectators will keep buffering infinitely.");
                    Widgets::Checkbox("Disable logging", &Config::Misc::DisableLogging); ImGui::SameLine(); Widgets::Tooltip("Disables Maple's log output to both console and runtime.log file.");

                    bool storageConfigEdited = false;
                    storageConfigEdited |= Widgets::Checkbox("Show menu after injection", &StorageConfig::ShowMenuAfterInjection);
                    storageConfigEdited |= Widgets::Hotkey("Menu key", &StorageConfig::MenuKey);

                    if (StorageConfig::MenuKey == 0)
                        StorageConfig::MenuKey = VK_DELETE;

                    if (storageConfigEdited)
                        Storage::SaveStorageConfig();

                    if (Widgets::Button("Open Maple folder", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        std::wstring wPath = std::wstring(Storage::StorageDirectory.begin(), Storage::StorageDirectory.end());
                        LPCWSTR path = wPath.c_str();

                        ShellExecute(NULL, L"open", path, NULL, NULL, SW_RESTORE);
                    }
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Discord Rich Presence Spoofer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(11)));
                {
                    Widgets::Checkbox("Enabled", &Config::Misc::DiscordRichPresenceSpoofer::Enabled); ImGui::SameLine(); Widgets::Tooltip("Spoofs various fields of your Discord Game Activity");

                	Widgets::Checkbox("Custom large image text", &Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled);
                    ImGui::InputText("Large image text", Config::Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, 128);

                    Widgets::Checkbox("Custom play mode", &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled);
                    const char* playModes[] = { "osu!", "osu!taiko", "osu!catch", "osu!mania" };
                    Widgets::Combo("Play mode", &Config::Misc::DiscordRichPresenceSpoofer::CustomPlayMode, playModes, IM_ARRAYSIZE(playModes));

                    Widgets::Checkbox("Custom state", &Config::Misc::DiscordRichPresenceSpoofer::CustomStateEnabled);
                    ImGui::InputText("State", Config::Misc::DiscordRichPresenceSpoofer::CustomState, 128);

                    Widgets::Checkbox("Custom details", &Config::Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled);
                    ImGui::InputText("Details", Config::Misc::DiscordRichPresenceSpoofer::CustomDetails, 128);

                    Widgets::Checkbox("Hide spectate button", &Config::Misc::DiscordRichPresenceSpoofer::HideSpectateButton);

                    Widgets::Checkbox("Hide match button", &Config::Misc::DiscordRichPresenceSpoofer::HideMatchButton);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 7)
            {
                Widgets::BeginPanel("Config", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4, 0, 1)));
                {
                    const float buttonWidth = ((ImGui::GetWindowWidth() * 0.5f) - (style.ItemSpacing.x * 2)) / 3;
                    Widgets::Combo("Config", &Config::CurrentConfig, [](void* vec, int idx, const char** out_text)
                        {
                            auto& vector = *static_cast<std::vector<std::string>*>(vec);
                            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                            *out_text = vector.at(idx).c_str();
                            return true;
                        }, reinterpret_cast<void*>(&Config::Configs), Config::Configs.size());

                    if (Widgets::Button("Load", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
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

                    if (Widgets::Button("Save", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Config::Save();

                    if (Config::CurrentConfig == 0)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::SameLine();

                    if (Widgets::Button("Refresh", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
                        Config::Refresh();

                    ImGui::Spacing();

                    ImGui::InputText("Config name", Config::NewConfigName, IM_ARRAYSIZE(Config::NewConfigName));
                    if (Widgets::Button("Create new config", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
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
    isVisible = true;
}

void MainMenu::Hide()
{
    isVisible = false;
}

void MainMenu::ToggleVisibility()
{
    isVisible = !isVisible;
}

bool MainMenu::GetIsVisible()
{
    return isVisible;
}
