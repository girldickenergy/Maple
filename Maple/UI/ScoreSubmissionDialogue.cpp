#include "ScoreSubmissionDialogue.h"

#include <imgui.h>

#include "StyleProvider.h"
#include "Widgets.h"
#include "../Features/Misc/ScoreSubmission.h"

void ScoreSubmissionDialogue::Render()
{
	//hack hack hack
	ImGui::PushFont(StyleProvider::FontDefaultBold);
	const ImVec2 panelHeaderLabelSize = ImGui::CalcTextSize("Score Submission");
	const float panelHeaderHeight = panelHeaderLabelSize.y + StyleProvider::Padding.y * 2;
	ImGui::PopFont();
	
	ImGui::PushFont(StyleProvider::FontDefault);

	const float panelContentHeight = Widgets::CalcPanelHeight(2, 1);
	const ImVec2 windowSize = ImVec2(ImGui::CalcTextSize("Do you really want to submit this score?").x, panelHeaderHeight + panelContentHeight) + StyleProvider::Padding * 2;

	if (resetPosition)
	{
		ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize / 2 - windowSize / 2);

		resetPosition = false;
	}
	
	ImGui::SetNextWindowSize(windowSize);
	ImGui::Begin("Score Submission Dialogue", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	{
		Widgets::BeginPanel("Score Submission", ImVec2(windowSize.x, panelContentHeight));
		{
			const float panelContentWidth = ImGui::GetWindowWidth();
			
			ImGui::Text("Do you really want to submit this score?");

			const float buttonWidth = panelContentWidth / 1.5f;
			ImGui::SetCursorPosX(panelContentWidth / 2 - buttonWidth / 2);
			if (Widgets::Button("Yes", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
			{
				ScoreSubmission::Submit();

				IsOpen = false;
				resetPosition = true;
			}

			ImGui::SetCursorPosX(panelContentWidth / 2 - buttonWidth / 2);
			if (Widgets::Button("No", ImVec2(buttonWidth, ImGui::GetFrameHeight())))
			{
				IsOpen = false;
				resetPosition = true;
			}
		}
		Widgets::EndPanel();
	}
	ImGui::End();

	ImGui::PopFont();
}
