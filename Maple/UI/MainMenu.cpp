#include "MainMenu.h"

#include <filesystem>
#include <imgui.h>

#include "StyleProvider.h"
#include "Widgets.h"
#include "../Config/Config.h"
#include "3rd-party/FileDialog/imfilebrowser.h"

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
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + StyleProvider::MainMenuSideBarSize, ImColor(StyleProvider::MenuColourDark), style.WindowRounding);
        if (expanded)
            ImGui::GetWindowDrawList()->AddRectFilled(menuPos + ImVec2(StyleProvider::MainMenuSideBarSize.x, 0), menuPos + ImVec2(StyleProvider::MainMenuSideBarSize.x - style.WindowRounding, style.WindowRounding), ImColor(StyleProvider::MenuColourDark));

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
                ImGui::TextColored(StyleProvider::AccentColour, "developer");
                ImGui::PopFont();
            }
            ImGui::EndChild();

            ImGui::BeginChild("Tabs", ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - StyleProvider::MainMenuBuildInfoHeight - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (50 * StyleProvider::Scale) * 6; //scaled tab height * tab count
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild("Tabs##001", ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 50 * StyleProvider::Scale);

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
                    if (Widgets::Tab("Misc", StyleProvider::MiscIconTexture, currentTab == 4, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;
                    if (Widgets::Tab("Config", StyleProvider::ConfigIconTexture, currentTab == 5, ImGuiSelectableFlags_SpanAllColumns, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;
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
                const ImVec2 buildStringSize = ImGui::CalcTextSize("l20102021");
                ImGui::SetCursorPos(ImVec2(buildInfoSize.x / 2 - buildStringSize.x / 2, buildInfoSize.y / 2 + style.ItemSpacing.y / 4));
                ImGui::TextColored(StyleProvider::MottoColour, "l21102021");
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
                Widgets::BeginPanel("Relax", ImVec2(optionsWidth, Widgets::CalcPanelHeight(11)));
                {
                    Widgets::Checkbox("Enabled", &Config::Relax::Enabled);
                    const char* playstyles[] = { "Singletap", "Alternate", "TapX" };
                    ImGui::Combo("Playstyle", &Config::Relax::Playstyle, playstyles, IM_ARRAYSIZE(playstyles));
                    const char* distributions[] = { "Uniform", "Normal" };
                    ImGui::Combo("Distribution", &Config::Relax::Distribution, distributions, IM_ARRAYSIZE(distributions));
                    const char* keys[] = { "M1", "K1", "M2", "K2" };
                    ImGui::Combo("Primary key", &Config::Relax::PrimaryKey, keys, IM_ARRAYSIZE(keys));
                    ImGui::Combo("Secondary key", &Config::Relax::SecondaryKey, keys, IM_ARRAYSIZE(keys));
                    Widgets::SliderInt("Max singletap BPM", &Config::Relax::MaxSingletapBPM, 0, 500, "%d", ImGuiSliderFlags_ClampOnInput);
                    Widgets::SliderInt("Hit spread", &Config::Relax::HitSpread, 0, 300, "%d", ImGuiSliderFlags_ClampOnInput);
                    //Widgets::HitErrorBar(Config::Relax::HitSpread);
                    Widgets::SliderInt("Alternation hit spread", &Config::Relax::AlternationHitSpread, 0, 300, "%d", ImGuiSliderFlags_ClampOnInput);
                    //Widgets::HitErrorBar(Config::Relax::AlternationHitSpread);
                    Widgets::Checkbox("Hold consecutive spinners", &Config::Relax::HoldConsecutiveSpinners);
                    Widgets::Checkbox("Slider alternation override", &Config::Relax::SliderAlternationOverride);
                    Widgets::Checkbox("Accurate calculations", &Config::Relax::AccurateCalculations);
                }
                Widgets::EndPanel();
        		
                Widgets::BeginPanel("Prediction", ImVec2(optionsWidth, Widgets::CalcPanelHeight(4)));
                {
                    Widgets::Checkbox("Enabled", &Config::Relax::PredictionEnabled);
                    Widgets::Checkbox("Slider prediction", &Config::Relax::SliderPredictionEnabled);
                    Widgets::SliderInt("Direction angle tolerance", &Config::Relax::PredictionAngle, 0, 90, "%d", ImGuiSliderFlags_ClampOnInput);
                    Widgets::SliderFloat("Scale", &Config::Relax::PredictionScale, 0, 1, "%.1f", ImGuiSliderFlags_ClampOnInput);
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 1)
            {
                Widgets::BeginPanel("Aim Assist", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::AimAssist::Enabled);
                    Widgets::Checkbox("Easy Mode", &Config::AimAssist::EasyMode);
                    Widgets::Checkbox("Show Debug Overlay", &Config::AimAssist::DrawDebugOverlay);
                }
                Widgets::EndPanel();

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
                        Widgets::SliderFloat("Strength", &Config::AimAssist::Strength, 0.f, 1.f, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderInt("Base FOV", &Config::AimAssist::BaseFOV, 0, 100, "%d", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Maximum FOV (Scaling)", &Config::AimAssist::MaximumFOVScale, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Minimum FOV (Total)", &Config::AimAssist::MinimumFOVTotal, 0, 100, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Maximum FOV (Total)", &Config::AimAssist::MaximumFOVTotal, 0, 500, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox("Assist on sliders", &Config::AimAssist::AssistOnSliders);
                        Widgets::SliderFloat("Sliderball Deadzone", &Config::AimAssist::SliderballDeadzone, 0, 25, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::Checkbox("Flip Sliderball Deadzone", &Config::AimAssist::FlipSliderballDeadzone);
                        Widgets::SliderFloat("Strength Multiplier", &Config::AimAssist::StrengthMultiplier, 0, 2, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Assist Deadzone", &Config::AimAssist::AssistDeadzone, 0, 5, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Resync Leniency", &Config::AimAssist::ResyncLeniency, 0, 15, "%.1f", ImGuiSliderFlags_ClampOnInput);
                        Widgets::SliderFloat("Resync Leniency Factor", &Config::AimAssist::ResyncLeniencyFactor, 0, 0.999, "%.3f", ImGuiSliderFlags_ClampOnInput);
                    }
                    Widgets::EndPanel();
                }
            }
        	if (currentTab == 2)
        	{
                Widgets::BeginPanel("Timewarp", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox("Enabled", &Config::Timewarp::Enabled);
                    Widgets::SliderInt("Rate", &Config::Timewarp::Rate, 25, 150, "%d", ImGuiSliderFlags_AlwaysClamp);
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 3)
            {
                Widgets::BeginPanel("AR Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::Visuals::ARChangerEnabled);
                    Widgets::SliderFloat("AR", &Config::Visuals::AR, 0, 12, "%.1f", ImGuiSliderFlags_AlwaysClamp);
                    Widgets::Checkbox("Adjust to mods", &Config::Visuals::ARChangerAdjustToMods);
                }
                Widgets::EndPanel();
            	
                Widgets::BeginPanel("HD & FL Removers", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox("Disable Hidden", &Config::Visuals::HiddenDisabled);
                    Widgets::Checkbox("Disable Flashlight", &Config::Visuals::FlashlightDisabled);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("User Interface", ImVec2(optionsWidth, Widgets::CalcPanelHeight(backgroundTexture ? 7 : 6, 0, 2)));
                {
                    const char* scales[] = { "50%", "75%", "100%", "125%", "150%" };
                    if (ImGui::Combo("Menu scale", &Config::Visuals::MenuScale, scales, IM_ARRAYSIZE(scales)))
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
                Widgets::BeginPanel("Misc", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    Widgets::Checkbox("Disable spectators", &Config::Misc::DisableSpectators);
                    Widgets::Checkbox("Prompt on score submission", &Config::Misc::PromptOnScoreSubmissionEnabled);
                }
                Widgets::EndPanel();

                Widgets::BeginPanel("Rich Presence Spoofer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    Widgets::Checkbox("Enabled", &Config::Misc::RichPresenceSpooferEnabled);
                    ImGui::InputText("Name", Config::Misc::SpoofedName, 64);
                    ImGui::InputText("Rank", Config::Misc::SpoofedRank, 64);
                }
                Widgets::EndPanel();
            }
            if (currentTab == 5)
            {
                Widgets::BeginPanel("Config", ImVec2(optionsWidth, Widgets::CalcPanelHeight(5, 0, 2)));
                {
	                const float buttonWidth1 = ((ImGui::GetWindowWidth() * 0.5f) - (style.ItemSpacing.x * 2)) / 3;
	                const float buttonWidth2 = ImGui::GetWindowWidth() * 0.5f;
                    ImGui::Combo("Config", &Config::CurrentConfig, [](void* vec, int idx, const char** out_text)
                    {
                        auto& vector = *static_cast<std::vector<std::string>*>(vec);
                        if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
                        *out_text = vector.at(idx).c_str();
                        return true;
                    }, reinterpret_cast<void*>(&Config::Configs), Config::Configs.size());

                    if (Widgets::Button("Load", ImVec2(buttonWidth1, ImGui::GetFrameHeight())))
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
                	
                    if (Widgets::Button("Save", ImVec2(buttonWidth1, ImGui::GetFrameHeight())))
                        Config::Save();
                	
                    if (Config::CurrentConfig == 0)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }

                    ImGui::SameLine();
                	
                    if (Widgets::Button("Refresh", ImVec2(buttonWidth1, ImGui::GetFrameHeight())))
                        Config::Refresh();

                    ImGui::Spacing();

                    ImGui::InputText("Config name", Config::NewConfigName, IM_ARRAYSIZE(Config::NewConfigName));
                    if (Widgets::Button("Create new config", ImVec2(buttonWidth2, ImGui::GetFrameHeight())))
                    {
                        Config::Create();

                        updateBackground();
                        StyleProvider::UpdateColours();
                        StyleProvider::UpdateScale();
                    }

                    ImGui::Spacing();

                    if (Widgets::Button("Open Maple folder", ImVec2(buttonWidth2, ImGui::GetFrameHeight())))
                    {
                        std::wstring wPath = std::wstring(Config::Directory.begin(), Config::Directory.end());
                        LPCWSTR path = wPath.c_str();

                        ShellExecute(NULL, L"open", path, NULL, NULL, SW_RESTORE);
                    }
                }
                Widgets::EndPanel();
            }

            ImGui::PopFont();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
