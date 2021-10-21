#include "MainMenu.h"

#include <imgui.h>

#include "StyleProvider.h"
#include "Widgets.h"
#include "../Config/Config.h"

void MainMenu::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    const bool expanded = currentTab != -1;
    ImGui::SetNextWindowSize(ImVec2(expanded ? 800 : 250, 600) * StyleProvider::Scale);
    ImGui::Begin("Maple menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        const ImVec2 menuPos = ImGui::GetCurrentWindow()->Pos;

        ImGui::GetWindowDrawList()->AddRectFilled(menuPos, menuPos + ImVec2(250, 600) * StyleProvider::Scale, ImColor(StyleProvider::MenuColourDark), style.WindowRounding);
        if (expanded)
            ImGui::GetWindowDrawList()->AddRectFilled(menuPos + ImVec2(250, 20) * StyleProvider::Scale, ImGui::GetCurrentWindow()->Pos + ImVec2(230, 0) * StyleProvider::Scale, ImColor(StyleProvider::MenuColourDark));

        ImGui::SetCursorPos(StyleProvider::Padding);
        ImGui::BeginChild("Maple Side Bar", ImVec2(250, 600) * StyleProvider::Scale - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
        {
            const ImVec2 sideBarSize = ImGui::GetCurrentWindow()->Size;

            ImGui::PushFont(StyleProvider::FontHugeBold);
            const ImVec2 logoSize = ImVec2(24, 24) * StyleProvider::Scale;
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5 * StyleProvider::Scale, 10 * StyleProvider::Scale));
            ImGui::SetCursorPosX(sideBarSize.x / 2 - ((ImGui::CalcTextSize("Maple").x / 2) + logoSize.x / 2 + style.ItemSpacing.x / 2));
            ImGui::Image(StyleProvider::MapleLogoTexture, logoSize);
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

            ImGui::BeginChild("User Info", ImVec2(sideBarSize.x, 100 * StyleProvider::Scale), false, ImGuiWindowFlags_NoBackground);
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

            ImGui::BeginChild("Tabs", ImVec2(sideBarSize.x, sideBarSize.y - ImGui::GetCursorPosY() - 50 * StyleProvider::Scale - style.ItemSpacing.y), false, ImGuiWindowFlags_NoBackground);
            {
                const ImVec2 tabsPos = ImGui::GetCurrentWindow()->Pos;
                const ImVec2 tabsSize = ImGui::GetCurrentWindow()->Size;

                ImGui::GetWindowDrawList()->AddRectFilled(tabsPos, tabsPos + tabsSize, ImColor(StyleProvider::MenuColourVeryDark), style.WindowRounding);

                const float tabsHeight = (50 * StyleProvider::Scale) * 6;
                ImGui::SetCursorPos(ImVec2(StyleProvider::Padding.x, tabsSize.y / 2 - tabsHeight / 2));
                ImGui::BeginChild("Tabs##001", ImVec2(tabsSize.x - (StyleProvider::Padding.x * 2), tabsHeight), false, ImGuiWindowFlags_NoBackground);
                {
                    const ImVec2 tabSize = ImVec2(ImGui::GetCurrentWindow()->Size.x, 50 * StyleProvider::Scale);

                    ImGui::PushFont(StyleProvider::FontDefaultBold);
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                    if (Widgets::Tab("Relax", StyleProvider::RelaxIconTexture, currentTab == 0, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 0 ? -1 : 0;
                    if (Widgets::Tab("Aim Assist", StyleProvider::AimAssistIconTexture, currentTab == 1, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 1 ? -1 : 1;
                    if (Widgets::Tab("Timewarp", StyleProvider::TimewarpIconTexture, currentTab == 2, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 2 ? -1 : 2;
                    if (Widgets::Tab("Visuals", StyleProvider::VisualsIconTexture, currentTab == 3, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 3 ? -1 : 3;
                    if (Widgets::Tab("Misc", StyleProvider::MiscIconTexture, currentTab == 4, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 4 ? -1 : 4;
                    if (Widgets::Tab("Config", StyleProvider::ConfigIconTexture, currentTab == 5, ImGuiSelectableFlags_NoPadWithHalfSpacing, tabSize))
                        currentTab = currentTab == 5 ? -1 : 5;
                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            ImGui::BeginChild("Build Info", ImVec2(sideBarSize.x, 50 * StyleProvider::Scale), false, ImGuiWindowFlags_NoBackground);
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
                ImGui::TextColored(StyleProvider::MottoColour, "l20102021");
                ImGui::PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::PushFont(StyleProvider::FontDefault);
        ImGui::SetCursorPos(ImVec2(250, 0) * StyleProvider::Scale + StyleProvider::Padding);
        ImGui::BeginChild("Options", ImVec2(550, 600) * StyleProvider::Scale - StyleProvider::Padding * 2, false, ImGuiWindowFlags_NoBackground);
        {
            const float optionsWidth = ImGui::GetWindowWidth();
        	
        	if (currentTab == 0)
        	{
                Widgets::BeginPanel("Relax", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();

                ImGui::Spacing();
        		
                Widgets::BeginPanel("Prediction", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 1)
            {
                Widgets::BeginPanel("Aim Assist", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();
            }
        	if (currentTab == 2)
        	{
                Widgets::BeginPanel("Timewarp", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    ImGui::Checkbox("Enabled", &Config::Timewarp::Enabled);
                    ImGui::SliderInt("Rate", &Config::Timewarp::Rate, 25, 150, "%1f", ImGuiSliderFlags_ClampOnInput);
                }
                Widgets::EndPanel();
        	}
            if (currentTab == 3)
            {
                Widgets::BeginPanel("AR Changer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(3)));
                {
                    ImGui::Checkbox("Enabled", &Config::Visuals::ARChangerEnabled);
                    ImGui::SliderFloat("AR", &Config::Visuals::AR, 0, 12);
                    ImGui::Checkbox("Adjust to mods", &Config::Visuals::ARChangerAdjustToMods);
                }
                Widgets::EndPanel();

                ImGui::Spacing();
            	
                Widgets::BeginPanel("HD & FL Removers", ImVec2(optionsWidth, Widgets::CalcPanelHeight(2)));
                {
                    ImGui::Checkbox("Disable Hidden", &Config::Visuals::HiddenDisabled);
                    ImGui::Checkbox("Disable Flashlight", &Config::Visuals::FlashlightDisabled);
                }
                Widgets::EndPanel();

                ImGui::Spacing();

                Widgets::BeginPanel("User Interface", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();
            }
            if (currentTab == 4)
            {
                Widgets::BeginPanel("Misc", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Checkbox("Disable spectators", &Config::Misc::DisableSpectators);
                }
                Widgets::EndPanel();

                ImGui::Spacing();

                Widgets::BeginPanel("Rich Presence Spoofer", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();
            }
            if (currentTab == 5)
            {
                Widgets::BeginPanel("Config", ImVec2(optionsWidth, Widgets::CalcPanelHeight(1)));
                {
                    ImGui::Text("Nothing to see here yet uwu");
                }
                Widgets::EndPanel();
            }
        }
        ImGui::PopFont();
        ImGui::EndChild();
    }
    ImGui::End();
}
