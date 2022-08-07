#include "ScoreSubmissionDialog.h"

#include "imgui.h"
#include "Math/Vector2.h"

#include "../StyleProvider.h"
#include "../Widgets/Widgets.h"
#include "../../SDK/Scoring/Score.h"
#include "../../SDK/Osu/GameBase.h"
#include "../../Utilities/Security/xorstr.hpp"

void ScoreSubmissionDialog::Render()
{
	if (!isVisible)
		return;
	
	//hack hack hack
	ImGui::PushFont(StyleProvider::FontDefaultBold);
	const ImVec2 panelHeaderLabelSize = ImGui::CalcTextSize(xor ("Score Submission"));
	const float panelHeaderHeight = panelHeaderLabelSize.y + StyleProvider::Padding.y * 2;
	ImGui::PopFont();

	ImGui::PushFont(StyleProvider::FontDefault);

	const float panelContentHeight = Widgets::CalcPanelHeight(2, 1);
	const ImVec2 windowSize = ImVec2(ImGui::CalcTextSize(xor ("Do you really want to submit this score?")).x, panelHeaderHeight + panelContentHeight) + StyleProvider::Padding * 2;

	if (resetPosition)
	{
		const Vector2 clientSize = GameBase::GetClientSize();
		const Vector2 clientPosition = GameBase::GetClientPosition();

		ImGui::SetNextWindowPos(ImVec2(clientPosition.X, clientPosition.Y) + ImVec2(clientSize.X / 2 - windowSize.x / 2, clientSize.Y / 2 - windowSize.y / 2));

		resetPosition = false;
	}

	ImGui::SetNextWindowSize(windowSize);
	ImGui::Begin(xor ("Score Submission Dialog"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	{
		Widgets::BeginPanel(xor ("Score Submission"), ImVec2(windowSize.x, panelContentHeight));
		{
			const float panelContentWidth = ImGui::GetWindowWidth();

			ImGui::Text(xor ("Do you really want to submit this score?"));

			const float buttonWidth = panelContentWidth / 1.5f;
			ImGui::SetCursorPosX(panelContentWidth / 2 - buttonWidth / 2);
			if (Widgets::Button(xor ("Yes"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
			{
				Score::Submit();
				
				Hide();
			}

			ImGui::SetCursorPosX(panelContentWidth / 2 - buttonWidth / 2);
			if (Widgets::Button(xor ("No"), ImVec2(buttonWidth, ImGui::GetFrameHeight())))
			{
				Score::AbortSubmission();

				Hide();
			}
		}
		Widgets::EndPanel();
	}
	ImGui::End();

	ImGui::PopFont();
}

void ScoreSubmissionDialog::Show()
{
	isVisible = true;
	resetPosition = true;
}

void ScoreSubmissionDialog::Hide()
{
	isVisible = false;
}

bool ScoreSubmissionDialog::GetIsVisible()
{
	return isVisible;
}
