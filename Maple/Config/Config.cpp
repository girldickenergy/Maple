#include "Config.h"

#include <filesystem>
#include <fstream>

#include <ThemidaSDK.h>

#include "../../Utilities/Security/xorstr.hpp"
#include "../Storage/Storage.h"
#include "../Storage/StorageConfig.h"

ImVec4 Config::parseImVec4(std::string vec)
{
	float result[4];
	int pos = 0;
	int index = 0;
	while ((pos = vec.find(',')) != std::string::npos)
	{
		result[index] = std::stof(vec.substr(0, pos));
		vec.erase(0, pos + 1);

		index++;
	}

	result[3] = std::stof(vec);

	return ImVec4(result[0], result[1], result[2], result[3]);
}

void Config::loadDefaults()
{
	STR_ENCRYPT_START
	
	Relax::Enabled = false;
	Relax::Distribution = 1;
	Relax::Playstyle = 0;
	Relax::PrimaryKey = 1;
	Relax::SecondaryKey = 3;
	Relax::MaxSingletapBPM = 100;
	Relax::HitSpread = 70;
	Relax::AlternationHitSpread = 90;
	Relax::HoldConsecutiveSpinners = false;
	Relax::SliderAlternationOverride = false;
	Relax::Prediction::Enabled = false;
	Relax::Prediction::SliderPredictionEnabled = false;
	Relax::Prediction::Angle = 45;
	Relax::Prediction::Scale = 0.8f;
	Relax::Blatant::UseLowestPossibleHoldTimes = false;

	AimAssist::Enabled = false;
	AimAssist::Algorithm = 0;
	AimAssist::DrawDebugOverlay = false;
	AimAssist::Algorithmv1::EasyMode::Enabled = true;
	AimAssist::Algorithmv1::EasyMode::Strength = 0.5f;
	AimAssist::Algorithmv1::AdvancedMode::Strength = 0.6f;
	AimAssist::Algorithmv1::AdvancedMode::BaseFOV = 45;
	AimAssist::Algorithmv1::AdvancedMode::MaximumFOVScale = 2.5f;
	AimAssist::Algorithmv1::AdvancedMode::MinimumFOVTotal = 0.f;
	AimAssist::Algorithmv1::AdvancedMode::MaximumFOVTotal = 225.f;
	AimAssist::Algorithmv1::AdvancedMode::SliderballDeadzone = 12.5f;
	AimAssist::Algorithmv1::AdvancedMode::FlipSliderballDeadzone = true;
	AimAssist::Algorithmv1::AdvancedMode::AssistOnSliders = true;
	AimAssist::Algorithmv1::AdvancedMode::StrengthMultiplier = 1.f;
	AimAssist::Algorithmv1::AdvancedMode::AssistDeadzone = 3.f;
	AimAssist::Algorithmv1::AdvancedMode::ResyncLeniency = 3.5f;
	AimAssist::Algorithmv1::AdvancedMode::ResyncLeniencyFactor = 0.698f;
	AimAssist::Algorithmv2::Power = 0.5f;
	AimAssist::Algorithmv2::AssistOnSliders = true;
	AimAssist::Algorithmv3::Strength = 0.6f;
	AimAssist::Algorithmv3::AssistOnSliders = true;
	AimAssist::Algorithmv3::BaseFOV = 45;
	AimAssist::Algorithmv3::MaximumFOVScale = 2.5f;
	AimAssist::Algorithmv3::MinimumFOVTotal = 0.f;
	AimAssist::Algorithmv3::MaximumFOVTotal = 225.f;
	AimAssist::Algorithmv3::AccelerationFactor = 1.f;

	Timewarp::Enabled = false;
	Timewarp::Type = 0;
	Timewarp::Rate = 100;
	Timewarp::Multiplier = 1.;

	Visuals::ARChanger::Enabled = false;
	Visuals::ARChanger::AR = 9.2f;
	Visuals::ARChanger::AdjustToMods = false;
	Visuals::ARChanger::AdjustToRate = false;
	Visuals::ARChanger::DrawPreemptiveDot = false;
	Visuals::ARChanger::PreemptiveDotColour = ImColor(232, 93, 155, 255).Value;
	Visuals::CSChanger::Enabled = false;
	Visuals::CSChanger::CS = 4.2f;
	Visuals::Removers::HiddenRemoverEnabled = false;
	Visuals::Removers::FlashlightRemoverEnabled = false;
	Visuals::UI::MenuScale = 2;
	Visuals::UI::MenuBackground[0] = '\0';
	Visuals::UI::Snow = false;
	Visuals::UI::AccentColour = ImColor(232, 93, 155, 255).Value;
	Visuals::UI::MenuColour = ImColor(65, 65, 65, 255).Value;
	Visuals::UI::ControlColour = ImColor(76, 76, 76, 255).Value;
	Visuals::UI::TextColour = ImVec4(ImColor(255, 255, 255, 255));

	Misc::DisableSpectators = false;
	Misc::PromptOnScoreSubmissionEnabled = false;
	Misc::DisableLogging = false;
	Misc::RichPresenceSpoofer::Enabled = false;
	strcpy_s(Misc::RichPresenceSpoofer::Name, xor ("maple.software"));
	strcpy_s(Misc::RichPresenceSpoofer::Rank, xor ("rocking osu! since 2021"));

	STR_ENCRYPT_END
}

void Config::Initialize()
{
	Refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), StorageConfig::DefaultConfig);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);
	else
		CurrentConfig = 0;

	Load();
}

void Config::Load()
{
	STR_ENCRYPT_START
	
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	loadDefaults(); //load default config first to ensure that old configs are fully initialized

	StorageConfig::DefaultConfig = Configs[CurrentConfig];
	Storage::SaveStorageConfig();

	if (CurrentConfig == 0)
		return;

	std::string configFilePath = Storage::ConfigsDirectory + "\\" + Configs[CurrentConfig] + ".cfg";

	if (!std::filesystem::exists(configFilePath))
		return;

	std::ifstream file(configFilePath);
	std::string line;

	while (std::getline(file, line))
	{
		const int delimiterIndex = line.find('=');
		std::string variable = line.substr(0, delimiterIndex);
		std::string value = line.substr(delimiterIndex + 1, std::string::npos);

		if (variable == "Relax_Enabled")
			Relax::Enabled = value == "1";
		if (variable == "Relax_Distribution")
			Relax::Distribution = std::stoi(value);
		if (variable == "Relax_Playstyle")
			Relax::Playstyle = std::stoi(value);
		if (variable == "Relax_PrimaryKey")
			Relax::PrimaryKey = std::stoi(value);
		if (variable == "Relax_SecondaryKey")
			Relax::SecondaryKey = std::stoi(value);
		if (variable == "Relax_MaxSingletapBPM")
			Relax::MaxSingletapBPM = std::stoi(value);
		if (variable == "Relax_HitSpread")
			Relax::HitSpread = std::stoi(value);
		if (variable == "Relax_AlternationHitSpread")
			Relax::AlternationHitSpread = std::stoi(value);
		if (variable == "Relax_HoldConsecutiveSpinners")
			Relax::HoldConsecutiveSpinners = value == "1";
		if (variable == "Relax_SliderAlternationOverride")
			Relax::SliderAlternationOverride = value == "1";
		if (variable == "Relax_Prediction_Enabled")
			Relax::Prediction::Enabled = value == "1";
		if (variable == "Relax_Prediction_SliderPredictionEnabled")
			Relax::Prediction::SliderPredictionEnabled = value == "1";
		if (variable == "Relax_Prediction_Angle")
			Relax::Prediction::Angle = std::stoi(value);
		if (variable == "Relax_Prediction_Scale")
			Relax::Prediction::Scale = std::stof(value);
		if (variable == "Relax_Blatant_UseLowestPossibleHoldTimes")
			Relax::Blatant::UseLowestPossibleHoldTimes = value == "1";

		if (variable == "AimAssist_Enabled")
			AimAssist::Enabled = value == "1";
		if (variable == "AimAssist_Algorithm")
			AimAssist::Algorithm = std::stoi(value);
		if (variable == "AimAssist_DrawDebugOverlay")
			AimAssist::DrawDebugOverlay = value == "1";
		if (variable == "AimAssist_Algorithmv1_EasyMode_Enabled")
			AimAssist::Algorithmv1::EasyMode::Enabled = value == "1";
		if (variable == "AimAssist_Algorithmv1_EasyMode_Strength")
			AimAssist::Algorithmv1::EasyMode::Strength = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_Strength")
			AimAssist::Algorithmv1::AdvancedMode::Strength = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_BaseFOV")
			AimAssist::Algorithmv1::AdvancedMode::BaseFOV = std::stoi(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_MaximumFOVScale")
			AimAssist::Algorithmv1::AdvancedMode::MaximumFOVScale = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_MinimumFOVTotal")
			AimAssist::Algorithmv1::AdvancedMode::MinimumFOVTotal = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_MaximumFOVTotal")
			AimAssist::Algorithmv1::AdvancedMode::MaximumFOVTotal = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_AssistOnSliders")
			AimAssist::Algorithmv1::AdvancedMode::AssistOnSliders = value == "1";
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_StrengthMultiplier")
			AimAssist::Algorithmv1::AdvancedMode::StrengthMultiplier = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_AssistDeadzone")
			AimAssist::Algorithmv1::AdvancedMode::AssistDeadzone = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_ResyncLeniency")
			AimAssist::Algorithmv1::AdvancedMode::ResyncLeniency = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_ResyncLeniencyFactor")
			AimAssist::Algorithmv1::AdvancedMode::ResyncLeniencyFactor = std::stof(value);
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_FlipSliderballDeadzone")
			AimAssist::Algorithmv1::AdvancedMode::FlipSliderballDeadzone = value == "1";
		if (variable == "AimAssist_Algorithmv1_AdvancedMode_SliderballDeadzone")
			AimAssist::Algorithmv1::AdvancedMode::SliderballDeadzone = std::stof(value);
		if (variable == "AimAssist_Algorithmv2_Power")
			AimAssist::Algorithmv2::Power = std::stof(value);
		if (variable == "AimAssist_Algorithmv2_AssistOnSliders")
			AimAssist::Algorithmv2::AssistOnSliders = value == "1";
		if (variable == "AimAssist_Algorithmv3_Strength")
			AimAssist::Algorithmv3::Strength = std::stof(value);
		if (variable == "AimAssist_Algorithmv3_AssistOnSliders")
			AimAssist::Algorithmv3::AssistOnSliders = value == "1";
		if (variable == "AimAssist_Algorithmv3_BaseFOV")
			AimAssist::Algorithmv3::BaseFOV = std::stoi(value);
		if (variable == "AimAssist_Algorithmv3_MaximumFOVScale")
			AimAssist::Algorithmv3::MaximumFOVScale = std::stof(value);
		if (variable == "AimAssist_Algorithmv3_MinimumFOVTotal")
			AimAssist::Algorithmv3::MinimumFOVTotal = std::stof(value);
		if (variable == "AimAssist_Algorithmv3_MaximumFOVTotal")
			AimAssist::Algorithmv3::MaximumFOVTotal = std::stof(value);
		if (variable == "AimAssist_Algorithmv3_AccelerationFactor")
			AimAssist::Algorithmv3::AccelerationFactor = std::stof(value);

		if (variable == "Timewarp_Enabled")
			Timewarp::Enabled = value == "1";
		if (variable == "Timewarp_Type")
			Timewarp::Type = std::stoi(value);
		if (variable == "Timewarp_Rate")
			Timewarp::Rate = std::stoi(value);
		if (variable == "Timewarp_Multiplier")
			Timewarp::Multiplier = std::stof(value);

		if (variable == "Visuals_ARChanger_Enabled")
			Visuals::ARChanger::Enabled = value == "1";
		if (variable == "Visuals_ARChanger_AR")
			Visuals::ARChanger::AR = std::stof(value);
		if (variable == "Visuals_ARChanger_AdjustToMods")
			Visuals::ARChanger::AdjustToMods = value == "1";
		if (variable == "Visuals_ARChanger_AdjustToRate")
			Visuals::ARChanger::AdjustToRate = value == "1";
		if (variable == "Visuals_ARChanger_DrawPreemptiveDot")
			Visuals::ARChanger::DrawPreemptiveDot = value == "1";
		if (variable == "Visuals_ARChanger_PreemptiveDotColour")
			Visuals::ARChanger::PreemptiveDotColour = parseImVec4(value);
		if (variable == "Visuals_CSChanger_Enabled")
			Visuals::CSChanger::Enabled = value == "1";
		if (variable == "Visuals_CSChanger_CS")
			Visuals::CSChanger::CS = std::stof(value);
		if (variable == "Visuals_Removers_HiddenRemoverEnabled")
			Visuals::Removers::HiddenRemoverEnabled = value == "1";
		if (variable == "Visuals_Removers_FlashlightRemoverEnabled")
			Visuals::Removers::FlashlightRemoverEnabled = value == "1";
		if (variable == "Visuals_UI_MenuScale")
			Visuals::UI::MenuScale = std::stoi(value);
		if (variable == "Visuals_UI_MenuBackground")
			strcpy_s(Visuals::UI::MenuBackground, value.c_str());
		if (variable == "Visuals_UI_Snow")
			Visuals::UI::Snow = value == "1";
		if (variable == "Visuals_UI_AccentColour")
			Visuals::UI::AccentColour = parseImVec4(value);
		if (variable == "Visuals_UI_MenuColour")
			Visuals::UI::MenuColour = parseImVec4(value);
		if (variable == "Visuals_UI_ControlColour")
			Visuals::UI::ControlColour = parseImVec4(value);
		if (variable == "Visuals_UI_TextColour")
			Visuals::UI::TextColour = parseImVec4(value);

		if (variable == "Misc_DisableSpectators")
			Misc::DisableSpectators = value == "1";
		if (variable == "Misc_PromptOnScoreSubmissionEnabled")
			Misc::PromptOnScoreSubmissionEnabled = value == "1";
		if (variable == "Misc_DisableLogging")
			Misc::DisableLogging = value == "1";
		if (variable == "Misc_RichPresenceSpoofer_Enabled")
			Misc::RichPresenceSpoofer::Enabled = value == "1";
		if (variable == "Misc_RichPresenceSpoofer_Name")
			strcpy_s(Misc::RichPresenceSpoofer::Name, value.c_str());
		if (variable == "Misc_RichPresenceSpoofer_Rank")
			strcpy_s(Misc::RichPresenceSpoofer::Rank, value.c_str());
	}

	file.close();

	STR_ENCRYPT_END
}

void Config::Save()
{
	STR_ENCRYPT_START
	
	if (CurrentConfig == 0)
		return;

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string configFilePath = Storage::ConfigsDirectory + "\\" + Configs[CurrentConfig] + ".cfg";

	std::ofstream ofs;
	ofs.open(configFilePath, std::ofstream::out | std::ofstream::trunc);

	ofs << "Relax_Enabled=" << Relax::Enabled << std::endl;
	ofs << "Relax_Distribution=" << Relax::Distribution << std::endl;
	ofs << "Relax_Playstyle=" << Relax::Playstyle << std::endl;
	ofs << "Relax_PrimaryKey=" << Relax::PrimaryKey << std::endl;
	ofs << "Relax_SecondaryKey=" << Relax::SecondaryKey << std::endl;
	ofs << "Relax_MaxSingletapBPM=" << Relax::MaxSingletapBPM << std::endl;
	ofs << "Relax_HitSpread=" << Relax::HitSpread << std::endl;
	ofs << "Relax_AlternationHitSpread=" << Relax::AlternationHitSpread << std::endl;
	ofs << "Relax_HoldConsecutiveSpinners=" << Relax::HoldConsecutiveSpinners << std::endl;
	ofs << "Relax_SliderAlternationOverride=" << Relax::SliderAlternationOverride << std::endl;
	ofs << "Relax_Prediction_Enabled=" << Relax::Prediction::Enabled << std::endl;
	ofs << "Relax_Prediction_SliderPredictionEnabled=" << Relax::Prediction::SliderPredictionEnabled << std::endl;
	ofs << "Relax_Prediction_Angle=" << Relax::Prediction::Angle << std::endl;
	ofs << "Relax_Prediction_Scale=" << Relax::Prediction::Scale << std::endl;
	ofs << "Relax_Blatant_UseLowestPossibleHoldTimes=" << Relax::Blatant::UseLowestPossibleHoldTimes << std::endl;

	ofs << "AimAssist_Enabled=" << AimAssist::Enabled << std::endl;
	ofs << "AimAssist_Algorithm=" << AimAssist::Algorithm << std::endl;
	ofs << "AimAssist_DrawDebugOverlay=" << AimAssist::DrawDebugOverlay << std::endl;
	ofs << "AimAssist_Algorithmv1_EasyMode_Enabled=" << AimAssist::Algorithmv1::EasyMode::Enabled << std::endl;
	ofs << "AimAssist_Algorithmv1_EasyMode_Strength=" << AimAssist::Algorithmv1::EasyMode::Strength << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_Strength=" << AimAssist::Algorithmv1::AdvancedMode::Strength << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_BaseFOV=" << AimAssist::Algorithmv1::AdvancedMode::BaseFOV << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_MaximumFOVScale=" << AimAssist::Algorithmv1::AdvancedMode::MaximumFOVScale << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_MinimumFOVTotal=" << AimAssist::Algorithmv1::AdvancedMode::MinimumFOVTotal << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_MaximumFOVTotal=" << AimAssist::Algorithmv1::AdvancedMode::MaximumFOVTotal << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_AssistOnSliders=" << AimAssist::Algorithmv1::AdvancedMode::AssistOnSliders << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_StrengthMultiplier=" << AimAssist::Algorithmv1::AdvancedMode::StrengthMultiplier << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_AssistDeadzone=" << AimAssist::Algorithmv1::AdvancedMode::AssistDeadzone << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_ResyncLeniency=" << AimAssist::Algorithmv1::AdvancedMode::ResyncLeniency << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_ResyncLeniencyFactor=" << AimAssist::Algorithmv1::AdvancedMode::ResyncLeniencyFactor << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_FlipSliderballDeadzone=" << AimAssist::Algorithmv1::AdvancedMode::FlipSliderballDeadzone << std::endl;
	ofs << "AimAssist_Algorithmv1_AdvancedMode_SliderballDeadzone=" << AimAssist::Algorithmv1::AdvancedMode::SliderballDeadzone << std::endl;
	ofs << "AimAssist_Algorithmv2_Power=" << AimAssist::Algorithmv2::Power << std::endl;
	ofs << "AimAssist_Algorithmv2_AssistOnSliders=" << AimAssist::Algorithmv2::AssistOnSliders << std::endl;
	ofs << "AimAssist_Algorithmv3_Strength=" << AimAssist::Algorithmv3::Strength << std::endl;
	ofs << "AimAssist_Algorithmv3_AssistOnSliders=" << AimAssist::Algorithmv3::AssistOnSliders << std::endl;
	ofs << "AimAssist_Algorithmv3_BaseFOV=" << AimAssist::Algorithmv3::BaseFOV << std::endl;
	ofs << "AimAssist_Algorithmv3_MaximumFOVScale=" << AimAssist::Algorithmv3::MaximumFOVScale << std::endl;
	ofs << "AimAssist_Algorithmv3_MinimumFOVTotal=" << AimAssist::Algorithmv3::MinimumFOVTotal << std::endl;
	ofs << "AimAssist_Algorithmv3_MaximumFOVTotal=" << AimAssist::Algorithmv3::MaximumFOVTotal << std::endl;
	ofs << "AimAssist_Algorithmv3_AccelerationFactor=" << AimAssist::Algorithmv3::AccelerationFactor << std::endl;

	ofs << "Timewarp_Enabled=" << Timewarp::Enabled << std::endl;
	ofs << "Timewarp_Type=" << Timewarp::Type << std::endl;
	ofs << "Timewarp_Rate=" << Timewarp::Rate << std::endl;
	ofs << "Timewarp_Multiplier=" << Timewarp::Multiplier << std::endl;

	ofs << "Visuals_ARChanger_Enabled=" << Visuals::ARChanger::Enabled << std::endl;
	ofs << "Visuals_ARChanger_AR=" << Visuals::ARChanger::AR << std::endl;
	ofs << "Visuals_ARChanger_AdjustToMods=" << Visuals::ARChanger::AdjustToMods << std::endl;
	ofs << "Visuals_ARChanger_AdjustToRate=" << Visuals::ARChanger::AdjustToRate << std::endl;
	ofs << "Visuals_ARChanger_DrawPreemptiveDot=" << Visuals::ARChanger::DrawPreemptiveDot << std::endl;
	ofs << "Visuals_ARChanger_PreemptiveDotColour=" << Visuals::ARChanger::PreemptiveDotColour.x << "," << Visuals::ARChanger::PreemptiveDotColour.y << "," << Visuals::ARChanger::PreemptiveDotColour.z << "," << Visuals::ARChanger::PreemptiveDotColour.w << std::endl;
	ofs << "Visuals_CSChanger_Enabled=" << Visuals::CSChanger::Enabled << std::endl;
	ofs << "Visuals_CSChanger_CS=" << Visuals::CSChanger::CS << std::endl;
	ofs << "Visuals_Removers_HiddenRemoverEnabled=" << Visuals::Removers::HiddenRemoverEnabled << std::endl;
	ofs << "Visuals_Removers_FlashlightRemoverEnabled=" << Visuals::Removers::FlashlightRemoverEnabled << std::endl;
	ofs << "Visuals_UI_MenuScale=" << Visuals::UI::MenuScale << std::endl;
	ofs << "Visuals_UI_MenuBackground=" << Visuals::UI::MenuBackground << std::endl;
	ofs << "Visuals_UI_Snow=" << Visuals::UI::Snow << std::endl;
	ofs << "Visuals_UI_AccentColour=" << Visuals::UI::AccentColour.x << "," << Visuals::UI::AccentColour.y << "," << Visuals::UI::AccentColour.z << "," << Visuals::UI::AccentColour.w << std::endl;
	ofs << "Visuals_UI_MenuColour=" << Visuals::UI::MenuColour.x << "," << Visuals::UI::MenuColour.y << "," << Visuals::UI::MenuColour.z << "," << Visuals::UI::MenuColour.w << std::endl;
	ofs << "Visuals_UI_ControlColour=" << Visuals::UI::ControlColour.x << "," << Visuals::UI::ControlColour.y << "," << Visuals::UI::ControlColour.z << "," << Visuals::UI::ControlColour.w << std::endl;
	ofs << "Visuals_UI_TextColour=" << Visuals::UI::TextColour.x << "," << Visuals::UI::TextColour.y << "," << Visuals::UI::TextColour.z << "," << Visuals::UI::TextColour.w << std::endl;

	ofs << "Misc_DisableSpectators=" << Misc::DisableSpectators << std::endl;
	ofs << "Misc_PromptOnScoreSubmissionEnabled=" << Misc::PromptOnScoreSubmissionEnabled << std::endl;
	ofs << "Misc_DisableLogging=" << Misc::DisableLogging << std::endl;
	ofs << "Misc_RichPresenceSpoofer_Enabled=" << Misc::RichPresenceSpoofer::Enabled << std::endl;
	ofs << "Misc_RichPresenceSpoofer_Name=" << Misc::RichPresenceSpoofer::Name << std::endl;
	ofs << "Misc_RichPresenceSpoofer_Rank=" << Misc::RichPresenceSpoofer::Rank << std::endl;

	ofs.close();

	STR_ENCRYPT_END
}

void Config::Create()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string configFilePath = Storage::ConfigsDirectory + "\\" + NewConfigName + ".cfg";

	if (!Storage::IsValidFileName(NewConfigName) || std::filesystem::exists(configFilePath))
		return;

	std::ofstream ofs(configFilePath);
	ofs.close();

	Refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), NewConfigName);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);

	loadDefaults();
}

void Config::Refresh()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	Configs.clear();
	Configs.emplace_back("default");

	for (const auto& file : std::filesystem::directory_iterator(Storage::ConfigsDirectory))
		if (file.path().extension() == ".cfg" && Storage::IsValidFileName(file.path().filename().stem().string()))
			Configs.push_back(file.path().filename().stem().string());
}