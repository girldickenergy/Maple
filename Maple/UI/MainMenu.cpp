#include "MainMenu.h"

#include <filesystem>
#include <imgui.h>

#include "StyleProvider.h"
#include "Widgets.h"
#include "../Communication/Communication.h"
#include "../Config/Config.h"
#include "3rd-party/FileDialog/imfilebrowser.h"
#include "../Features/Spoofer/Spoofer.h"
#include "../Utilities/Directories/DirectoryHelper.h"
#include "../Sdk/Osu/WindowManager.h"

bool fileDialogInitialized = false;
ImGui::FileBrowser fileDialog;

void MainMenu::updateBackground()
{
    if (Config::Visuals::MenuBackground[0] == '\0' || !std::filesystem::exists(Config::Visuals::MenuBackground))
    {
        if (backgroundTexture != nullptr)
        {
            if (Overlay::Renderer == Renderer::OGL3)
                TextureHelper::FreeTextureOGL3(backgroundTexture);

            backgroundTexture = nullptr;
        }

        return;
    }

    if (Overlay::Renderer == Renderer::OGL3)
        backgroundTexture = TextureHelper::LoadTextureFromFileOGL3(Config::Visuals::MenuBackground);
    else
        backgroundTexture = TextureHelper::LoadTextureFromFileD3D9(Overlay::D3D9Device, Config::Visuals::MenuBackground);
}

void MainMenu::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    if (backgroundTexture != nullptr)
        ImGui::GetBackgroundDrawList()->AddImage(backgroundTexture, ImVec2(0, 0), ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    const bool expanded = currentTab != -1;
    ImGui::SetNextWindowSize(expanded ? StyleProvider::MainMenuSize : StyleProvider::MainMenuSideBarSize);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
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
                ImGui::TextColored(StyleProvider::AccentColour, Communication::CurrentUser->Username.c_str());
                ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::BeginChild("Tabs", ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (45 * StyleProvider::Scale) * 7; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild("Tabs##001", ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 45 * StyleProvider::Scale);

                    ImGui::PushFont(StyleProvider::FontDefaultBold);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                    if (Widgets::Tab("Relax", StyleProvider::RelaxIconTexture, currentTab == 0, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 0 ? -1 : 0;
                    if (Widgets::Tab("Aim Assist", StyleProvider::AimAssistIconTexture, currentTab == 1, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 1 ? -1 : 1;
                    if (Widgets::Tab("Timewarp", StyleProvider::TimewarpIconTexture, currentTab == 2, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 2 ? -1 : 2;
                    if (Widgets::Tab("Visuals", StyleProvider::VisualsIconTexture, currentTab == 3, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 3 ? -1 : 3;
                    if (Widgets::Tab("Spoofer", StyleProvider::SpooferIconTexture, currentTab == 4, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;
                    if (Widgets::Tab("Misc", StyleProvider::MiscIconTexture, currentTab == 5, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;
                    if (Widgets::Tab("Config", StyleProvider::ConfigIconTexture, currentTab == 6, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 6 ? -1 : 6;
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
                const ImVec2 buildStringSize = ImGui::CalcTextSize("l04022022");
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, "l04022022");
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
                Widgets::BeginPanel("Note", ImVec2(optionsWidth, Widgets::CalcPanelHeight(0, 3)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, "Current relax can be detected. A better version of it is coming");
                    ImGui::TextColored(StyleProvider::AccentColour, "in the next updates.");
                    ImGui::TextColored(StyleProvider::AccentColour, "We don't recommend using relax unless you really want to.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Relax", ImVec2(optionsWidth, Widgets::CalcPanelHeight(12)));
                {
                    Widgets::Checkbox("Enabled", &Config::Relax::Enabled); Widgets::Tooltip("All hit objects will be automatically tapped by Maple.");
                    const char* playstyles[] = { "Singletap", "Alternate", "TapX" };
                    Widgets::Combo("Playstyle", &Config::Relax::Playstyle, playstyles, IM_ARRAYSIZE(playstyles)); Widgets::Tooltip("Singletap - taps with the primary key and alternates only above certain BPM.\n\nAlternate - alternates all the time, regardless of the BPM.\n\nTapX - singletaps with M1 and alternates above certain BPM with Primary and Secondary keys.");
                    const char* distributions[] = { "Uniform", "Normal" };
                    Widgets::Combo("Distribution", &Config::Relax::Distribution, distributions, IM_ARRAYSIZE(distributions)); Widgets::Tooltip("Specifies how hits will look on hit error bar.\n\nUniform - a widely used random distribution, looks like a rectangle on hit error bar.\n\nNormal - aka Gauss Distribution, looks like a bell on hit error bar. It will look legit to most players, however it may yield very low unstable rates on certain configurations and beatmaps.");
                    const char* keys[] = { "M1", "K1", "M2", "K2" };
                    Widgets::Combo("Primary key", &Config::Relax::PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::Combo("Secondary key", &Config::Relax::SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::SliderInt("Max singletap BPM", &Config::Relax::MaxSingletapBPM, 0, 500, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("A maximum BPM at which relax will singletap.");
                    Widgets::SliderInt("Hit spread", &Config::Relax::HitSpread, 0, 300, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Specifies a spread for distribution function.\n\n100 is a full hit window 300.\n200 is a full hit window 100.\n300 is a full hit window50.");
                    Widgets::HitErrorBar(Config::Relax::HitSpread);
                    Widgets::SliderInt("Alternation hit spread", &Config::Relax::AlternationHitSpread, 0, 300, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Same as above, but for alternation.");
                    Widgets::HitErrorBar(Config::Relax::AlternationHitSpread);
                    Widgets::Checkbox("Hold consecutive spinners", &Config::Relax::HoldConsecutiveSpinners); Widgets::Tooltip("Relax will keep holding the key if there's a spinner right after the last hit object.");
                    Widgets::Checkbox("Slider alternation override", &Config::Relax::SliderAlternationOverride); Widgets::Tooltip("Changes the way how alternation of sliders is handled.\nIt is recommended to enable this option on techno maps.");
                }
                Widgets::EndPanel();
        		
                Widgets::BeginPanel("Prediction", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    Widgets::Checkbox("Enabled", &Config::Relax::PredictionEnabled); Widgets::Tooltip("Predicts whether or not you're leaving the circle and clicks if you are.");
                    Widgets::Checkbox("Slider prediction", &Config::Relax::SliderPredictionEnabled); Widgets::Tooltip("Same as above, but for sliders.\n\nOften yields false positive results. Enable this only if you really have to.");
                    Widgets::SliderInt("Direction angle tolerance", &Config::Relax::PredictionAngle, 0, 90, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("A maximum angle between current cursor position, last cursor position and next circle position for prediction to trigger.\n\nLower value = worse prediction.");
                    Widgets::SliderFloat("Scale", &Config::Relax::PredictionScale, 0, 1, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Specifies a portion of the circle where prediction will trigger.\n\n0 = full circle.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Blatant", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, "Don't use this on legit servers!");
                    Widgets::Checkbox("Use lowest possible hold times", &Config::Relax::UseLowestPossibleHoldTimes);
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 1)
            {
                Widgets::BeginPanel("Aim Assist", ImVec2(optionsWidth, Widgets::CalcPanelHeight(Config::AimAssist::Algorithm == 0 ? 4 : Config::AimAssist::Algorithm == 1 ? 5 : 10, Config::AimAssist::Algorithm == 0 ? 4 : 0)));
                {
                    Widgets::Checkbox("Enabled", &Config::AimAssist::Enabled);
                    const char* algorithms[] = { "v1", "v2", "v3" };
                    Widgets::Combo("Algorithm", &Config::AimAssist::Algorithm, algorithms, IM_ARRAYSIZE(algorithms));
                    if (Config::AimAssist::Algorithm == 0)
                    {
                        ImGui::TextColored(StyleProvider::AccentColour, "This algorithm can lead to technical bugs and teleportations on certain settings.");
                        ImGui::TextColored(StyleProvider::AccentColour, "certain settings.");
                        ImGui::TextColored(StyleProvider::AccentColour, "Instead, we strongly advise you to use algorithm v3 which is just a");
                        ImGui::TextColored(StyleProvider::AccentColour, "polished version of v1.");

                        Widgets::Checkbox("Easy Mode", &Config::AimAssist::EasyMode);
                    }
                    else if (Config::AimAssist::Algorithm == 1)
                    {
                        Widgets::SliderFloat("Power", &Config::AimAssist::Algorithmv2Power, 0.f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox("Assist on sliders##algov2assistonsliders", &Config::AimAssist::Algorithmv2AssistOnSliders);
                    }
                    else
                    {
                        Widgets::SliderFloat("Strength##algov3strength", &Config::AimAssist::Algorithmv3Strength, 0.f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the Aim Assist strength, change this value according to how strong you want to be helped with.");
                        Widgets::Checkbox("Assist on sliders##algov3assistonsliders", &Config::AimAssist::Algorithmv3AssistOnSliders); Widgets::Tooltip("Do you need help on sliders?\nYes?\nTurn this on then.");
                        Widgets::SliderInt("Base FOV##algov3basefov", &Config::AimAssist::Algorithmv3BaseFOV, 0, 100, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps.");
                        Widgets::SliderFloat("Maximum FOV (Scaling)##algov3maxfovscale", &Config::AimAssist::Algorithmv3MaximumFOVScale, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist.");
                        Widgets::SliderFloat("Minimum FOV (Total)##algov3minfovtotal", &Config::AimAssist::Algorithmv3MinimumFOVTotal, 0, 100, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the total minimum FOV of the Aim Assist.");
                        Widgets::SliderFloat("Maximum FOV (Total)##algov3maxfovtotal", &Config::AimAssist::Algorithmv3MaximumFOVTotal, 0, 500, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the total maximum FOV of the Aim Assist.");
                        Widgets::SliderFloat("Acceleration factor", &Config::AimAssist::Algorithmv3AccelerationFactor, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps.");
                    }
                    Widgets::Checkbox("Show Debug Overlay", &Config::AimAssist::DrawDebugOverlay);
                }
                Widgets::EndPanel();

                if (Config::AimAssist::Algorithm == 0)
                {
                    if (Config::AimAssist::EasyMode)
                    {
                        Widgets::BeginPanel("Easy Mode", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                        {
                            Widgets::SliderFloat("Easy Mode Strength", &Config::AimAssist::EasyModeStrength, 0.f, 2.f, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        }
                        Widgets::EndPanel();
                    }
                    else
                    {
                        Widgets::BeginPanel("Advanced Mode", ImVec2(optionsWidth, Widgets::CalcPanelHeight(12)));
                        {
                            Widgets::SliderFloat("Strength", &Config::AimAssist::Strength, 0.f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the Aim Assist strength, change this value according to how strong you want to be helped with.");
                            Widgets::SliderInt("Base FOV", &Config::AimAssist::BaseFOV, 0, 100, "%d", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("This basically acts as the Aim Assist's Field of View. If the next object distance is too far from the cursor, the aim assist will not assist.\nIf you're in range of the object, but still far away, setting Distance to a high value will trigger visible snaps.");
                            Widgets::SliderFloat("Maximum FOV (Scaling)", &Config::AimAssist::MaximumFOVScale, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the maximum amount that the AR & Time will influence the FOV of the Aim Assist.");
                            Widgets::SliderFloat("Minimum FOV (Total)", &Config::AimAssist::MinimumFOVTotal, 0, 100, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the total minimum FOV of the Aim Assist.");
                            Widgets::SliderFloat("Maximum FOV (Total)", &Config::AimAssist::MaximumFOVTotal, 0, 500, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the total maximum FOV of the Aim Assist.");
                            Widgets::Checkbox("Assist on sliders", &Config::AimAssist::AssistOnSliders); Widgets::Tooltip("Do you need help on sliders?\nYes?\nTurn this on then.");
                            Widgets::SliderFloat("Sliderball Deadzone", &Config::AimAssist::SliderballDeadzone, 0, 25, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the deadzone of the slider Aim Assist.\nDepending on the 'Flip Sliderball Deadzone' checkbox this deadzone behaves differently.");
                            Widgets::Checkbox("Flip Sliderball Deadzone", &Config::AimAssist::FlipSliderballDeadzone); Widgets::Tooltip("Flips the behavior of the Sliderball Deadzone.\nChecked = Aim Assist only assists when inside of the deadzone.\nUnchecked = Aim Assist only assists when outside of the deadzone.");
                            Widgets::SliderFloat("Strength Multiplier", &Config::AimAssist::StrengthMultiplier, 0, 2, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("This value gets multiplied ontop of the internal calculated strength. Meaning if you think your cursor is too slow, you can try setting this value higher, and vice versa.\nWarning: Don't change this if you don't feel like something is wrong, using the wrong settings here can make your cursor visibly snap.");
                            Widgets::SliderFloat("Assist Deadzone", &Config::AimAssist::AssistDeadzone, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Setting this to a high value will make the Aim Assist only assist you when you throw your cursor around the screen.\nUseful to negate a self concious Aim Assist and also useful to limit Aim Assist to cross-screen jumps.");
                            Widgets::SliderFloat("Resync Leniency", &Config::AimAssist::ResyncLeniency, 0, 15, "%.1f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the value where the Aim Assist would counteract teleporations and desyncs by easing back to original position through the factor defined below.");
                            Widgets::SliderFloat("Resync Leniency Factor", &Config::AimAssist::ResyncLeniencyFactor, 0, 0.999f, "%.3f", ImGuiSliderFlags_ClampOnInput); Widgets::Tooltip("Sets the multiplication factor for easing-back on a cursor desync. This also acts like a smoothing filter, a higher value will make the cursor drag behind.\n1 = Slow synchronization (slow filter)\n0 = Ease back almost instantly");
                        }
                        Widgets::EndPanel();
                    }
                }
            }
        	if (currentTab == 2)
        	{
                Widgets::BeginPanel("Timewarp", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::Timewarp::Enabled); Widgets::Tooltip("Slows down or speeds up the game.");
                    const char* types[] = { "Rate", "Multiplier" };
                    Widgets::Combo("Type", &Config::Timewarp::Type, types, IM_ARRAYSIZE(types));
                    if (Config::Timewarp::Type == 0)
                    {
                        Widgets::SliderInt("Rate", &Config::Timewarp::Rate, 25, 150, "%d", ImGuiSliderFlags_AlwaysClamp); Widgets::Tooltip("The desired speed of timewarp.\n\nLower value = slower.\nHigher value = faster.\n\n75 is HalfTime.\n100 is NoMod.\n150 is DoubleTime.");
                    }
                    else
                    {
                        Widgets::SliderFloat("Multiplier", &Config::Timewarp::Multiplier, 0.25f, 1.5f, "%.01f", ImGuiSliderFlags_AlwaysClamp);
                    }
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 3)
            {
                Widgets::BeginPanel("AR Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(6)));
                {
                    Widgets::Checkbox("Enabled", &Config::Visuals::ARChangerEnabled); Widgets::Tooltip("AR is short for Approach Rate and defines when hit objects start to fade in relative to when they should be hit or collected.");
                    Widgets::SliderFloat("AR", &Config::Visuals::AR, 0, 12, "%.1f", ImGuiSliderFlags_AlwaysClamp); Widgets::Tooltip("Higher value = hit objects will be shown for a shorter period of time = less time to react.\n\nLower value = hit objects will be shown for a longer period of time = more time to react.");
                    Widgets::Checkbox("Adjust to mods", &Config::Visuals::ARChangerAdjustToMods); Widgets::Tooltip("If this option is enabled, AR Changer will adjust the AR you have set to currently selected mods.\n\nFor example, if you selected Easy mod, AR will be slightly lower.");
                    Widgets::Checkbox("Adjust to rate", &Config::Visuals::ARChangerAdjustToRate);
                    Widgets::Checkbox("Draw preemptive dot", &Config::Visuals::ARChangerDrawPreemptiveDot);
                    ImGui::ColorEdit4("Preemptive dot colour", reinterpret_cast<float*>(&Config::Visuals::ARChangerPreemptiveDotColour), ImGuiColorEditFlags_NoInputs);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("CS Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2, 1)));
                {
                    ImGui::TextColored(StyleProvider::AccentColour, "Don't use this on legit servers!");
                    Widgets::Checkbox("Enabled", &Config::Visuals::CSChangerEnabled);
                    Widgets::SliderFloat("CS", &Config::Visuals::CS, 0, 10, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();
            	
                Widgets::BeginPanel("HD & FL Removers", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox("Disable Hidden", &Config::Visuals::HiddenDisabled); Widgets::Tooltip("Disables Hidden mod.");
                    Widgets::Checkbox("Disable Flashlight", &Config::Visuals::FlashlightDisabled); Widgets::Tooltip("Disables Flashlight mod.");
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("User Interface", ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 7 : 6, 0, 2)));
                {
                    const char* scales[] = { "50%", "75%", "100%", "125%", "150%" };
                    if (Widgets::Combo("Menu scale", &Config::Visuals::MenuScale, scales, IM_ARRAYSIZE(scales)))
                        StyleProvider::UpdateScale();

                    ImGui::Spacing();

                    if (Widgets::Button("Load background image", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                    	if (!fileDialogInitialized)
                    	{
                            fileDialog.SetTitle("Select background image");
                            fileDialog.SetTypeFilters({ ".png", ".jpg", ".jpeg", ".bmp", ".tga" });

                            fileDialogInitialized = true;
                    	}

                        fileDialog.Open();
                    }

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, StyleProvider::Padding);
                    fileDialog.Display();
                    ImGui::PopStyleVar();

                    if (fileDialog.HasSelected())
                    {
                        strcpy_s(Config::Visuals::MenuBackground, fileDialog.GetSelected().string().c_str());
                        fileDialog.ClearSelected();

                        updateBackground();
                    }

                	if (backgroundTexture)
                	{
                        if (Widgets::Button("Remove background image", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                        {
                            Config::Visuals::MenuBackground[0] = '\0';

                            updateBackground();
                        }
                	}

                    ImGui::Spacing();

                    bool coloursChanged = false;
                    coloursChanged |= ImGui::ColorEdit4("Accent colour", reinterpret_cast<float*>(&Config::Visuals::AccentColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Menu colour", reinterpret_cast<float*>(&Config::Visuals::MenuColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Control colour", reinterpret_cast<float*>(&Config::Visuals::ControlColour), ImGuiColorEditFlags_NoInputs);
                    coloursChanged |= ImGui::ColorEdit4("Text colour", reinterpret_cast<float*>(&Config::Visuals::TextColour), ImGuiColorEditFlags_NoInputs);

                    if (coloursChanged)
                        StyleProvider::UpdateColours();
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
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
            if (currentTab == 5)
            {
                Widgets::BeginPanel("Misc", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    Widgets::Checkbox("Disable spectators", &Config::Misc::DisableSpectators); Widgets::Tooltip("Spectators will keep buffering infinitely.");
                    Widgets::Checkbox("Prompt on score submission", &Config::Misc::PromptOnScoreSubmissionEnabled); Widgets::Tooltip("Before submitting the score Maple will ask you whether or not you really want to submit it.");
                    Widgets::Checkbox("Disable logging", &Config::Misc::DisableLogging); Widgets::Tooltip("Disables Maple's log output to both console and runtime.log file.");

                    if (Widgets::Button("Open Maple folder", ImVec2(ImGui::GetWindowWidth() * 0.5f, ImGui::GetFrameHeight())))
                    {
                        std::wstring wPath = std::wstring(DirectoryHelper::WorkingDirectory.begin(), DirectoryHelper::WorkingDirectory.end());
                        LPCWSTR path = wPath.c_str();

                        ShellExecute(NULL, L"open", path, NULL, NULL, SW_RESTORE);
                    }
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Rich Presence Spoofer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::Misc::RichPresenceSpooferEnabled); Widgets::Tooltip("Spoofs Name and Rank fields of your Discord Activity (aka Discord Rich Presence)");
                    ImGui::InputText("Name", Config::Misc::SpoofedName, 64);
                    ImGui::InputText("Rank", Config::Misc::SpoofedRank, 64);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 6)
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

        ImGui::SetWindowPos(ImVec2(std::clamp(menuPos.x, WindowManager::ViewportPosition().X, WindowManager::ViewportPosition().X + WindowManager::Width() - menuSize.x), std::clamp(menuPos.y, WindowManager::ViewportPosition().Y, WindowManager::ViewportPosition().Y + WindowManager::Height() - menuSize.y)));
    }

    ImGui::End();
}
