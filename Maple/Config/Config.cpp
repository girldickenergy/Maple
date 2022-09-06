#include "Config.h"

#include <filesystem>
#include <fstream>

#include "ThemidaSDK.h"

#include "../../Utilities/Security/xorstr.hpp"
#include "../Storage/Storage.h"
#include "../Storage/StorageConfig.h"
#include "../Utilities/Clipboard/ClipboardUtilities.h"
#include "../Utilities/Crypto/CryptoUtilities.h"
#include "../Utilities/Strings/StringUtilities.h"

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

	return {result[0], result[1], result[2], result[3]};
}

void Config::loadDefaults()
{
	STR_ENCRYPT_START

	Relax::Enabled = false;
	Relax::ToggleKey = VK_RCONTROL;
	Relax::PrimaryKey = 1;
	Relax::SecondaryKey = 3;
	Relax::AlternateBPM = 100;
	Relax::SliderAlternationOverride = false;
	Relax::Timing::Offset = 0;
	Relax::Timing::TargetUnstableRate = 100;
	Relax::Timing::AverageHoldTime = 80;
	Relax::Timing::AverageHoldTimeError = 20;
	Relax::Timing::AverageSliderHoldTime = 65;
	Relax::Timing::AverageSliderHoldTimeError = 15;
	Relax::HitScan::WaitLateEnabled = true;
	Relax::HitScan::DirectionPredictionEnabled = true;
	Relax::HitScan::DirectionPredictionAngle = 45;
	Relax::HitScan::DirectionPredictionScale = 0.7f;
	Relax::Blatant::UseLowestPossibleHoldTimes = false;

	AimAssist::Enabled = false;
	AimAssist::Algorithm = 0;
	AimAssist::DrawDebugOverlay = false;
	AimAssist::Algorithmv1::Strength = 0.6f;
	AimAssist::Algorithmv1::AssistOnSliders = true;
	AimAssist::Algorithmv1::BaseFOV = 45;
	AimAssist::Algorithmv1::MaximumFOVScale = 2.5f;
	AimAssist::Algorithmv1::MinimumFOVTotal = 0.f;
	AimAssist::Algorithmv1::MaximumFOVTotal = 225.f;
	AimAssist::Algorithmv1::AccelerationFactor = 1.f;
	AimAssist::Algorithmv2::Power = 0.5f;
	AimAssist::Algorithmv2::AssistOnSliders = true;
	AimAssist::Algorithmv3::Power = 0.5f;
	AimAssist::Algorithmv3::SliderAssistPower = 1.f;

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

	Misc::ScoreSubmissionType = 0;
	Misc::DisableSpectators = false;
	Misc::Logging::DisableLogging = false;
	Misc::DiscordRichPresenceSpoofer::Enabled = false;
	Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, xor ("peppy (rank #18,267,309)"));
	Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled = false;
	Misc::DiscordRichPresenceSpoofer::CustomPlayMode = 0;
	Misc::DiscordRichPresenceSpoofer::CustomStateEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomState, xor ("Trying to detect Maple since 2021"));
	Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomDetails, xor ("Kenji Ninuma - DISCO PRINCE [Normal]"));
	Misc::DiscordRichPresenceSpoofer::HideSpectateButton = false;
	Misc::DiscordRichPresenceSpoofer::HideMatchButton = false;

	STR_ENCRYPT_END
}

void Config::refresh()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	Configs.clear();
	Configs.emplace_back(xor ("default"));

	for (const auto& file : std::filesystem::directory_iterator(Storage::ConfigsDirectory))
		if (file.path().extension() == xor (".cfg") && Storage::IsValidFileName(file.path().filename().stem().string()))
			Configs.push_back(file.path().filename().stem().string());
}

void Config::Initialize()
{
	refresh();

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

	std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + Configs[CurrentConfig] + xor (".cfg");

	if (!std::filesystem::exists(configFilePath))
		return;

	std::ifstream file(configFilePath);
	std::string line;

	float configVersion = 0.f;
	while (std::getline(file, line))
	{
		const int delimiterIndex = line.find('=');
		std::string variable = line.substr(0, delimiterIndex);
		std::string value = line.substr(delimiterIndex + 1, std::string::npos);

		if (variable == xor ("Version"))
			configVersion = std::stof(value);

		if (variable == xor ("Relax_Enabled"))
			Relax::Enabled = value == xor ("1");
		if (variable == xor ("Relax_ToggleKey"))
			Relax::ToggleKey = std::stoi(value);
		if (variable == xor ("Relax_PrimaryKey"))
			Relax::PrimaryKey = std::stoi(value);
		if (variable == xor ("Relax_SecondaryKey"))
			Relax::SecondaryKey = std::stoi(value);
		if (variable == xor ("Relax_AlternateBPM"))
			Relax::AlternateBPM = std::stoi(value);
		if (variable == xor ("Relax_SliderAlternationOverride"))
			Relax::SliderAlternationOverride = value == xor ("1");
		if (variable == xor ("Relax_Timing_Offset"))
			Relax::Timing::Offset = std::stoi(value);
		if (variable == xor ("Relax_Timing_TargetUnstableRate"))
			Relax::Timing::TargetUnstableRate = std::stoi(value);
		if (variable == xor ("Relax_Timing_AverageHoldTime"))
			Relax::Timing::AverageHoldTime = std::stoi(value);
		if (variable == xor ("Relax_Timing_AverageHoldTimeError"))
			Relax::Timing::AverageHoldTimeError = std::stoi(value);
		if (variable == xor ("Relax_Timing_AverageSliderHoldTime"))
			Relax::Timing::AverageSliderHoldTime = std::stoi(value);
		if (variable == xor ("Relax_Timing_AverageSliderHoldTimeError"))
			Relax::Timing::AverageSliderHoldTimeError = std::stoi(value);

		if (variable == xor ("Relax_HitScan_WaitLate_Enabled"))
			Relax::HitScan::WaitLateEnabled = value == xor ("1");
		if (variable == xor ("Relax_HitScan_DirectionPrediction_Enabled"))
			Relax::HitScan::DirectionPredictionEnabled = value == xor ("1");
		if (variable == xor ("Relax_HitScan_DirectionPrediction_Angle"))
			Relax::HitScan::DirectionPredictionAngle = std::stoi(value);
		if (variable == xor ("Relax_HitScan_DirectionPrediction_Scale"))
			Relax::HitScan::DirectionPredictionScale = std::stof(value);

		if (variable == xor ("Relax_Blatant_UseLowestPossibleHoldTimes"))
			Relax::Blatant::UseLowestPossibleHoldTimes = value == xor ("1");

		if (configVersion >= 1.f)
		{
			if (variable == xor ("AimAssist_Enabled"))
				AimAssist::Enabled = value == xor ("1");
			if (variable == xor ("AimAssist_Algorithm"))
				AimAssist::Algorithm = std::stoi(value);
			if (variable == xor ("AimAssist_Algorithmv1_Strength"))
				AimAssist::Algorithmv1::Strength = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv1_AssistOnSliders"))
				AimAssist::Algorithmv1::AssistOnSliders = value == xor ("1");
			if (variable == xor ("AimAssist_Algorithmv1_BaseFOV"))
				AimAssist::Algorithmv1::BaseFOV = std::stoi(value);
			if (variable == xor ("AimAssist_Algorithmv1_MaximumFOVScale"))
				AimAssist::Algorithmv1::MaximumFOVScale = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv1_MinimumFOVTotal"))
				AimAssist::Algorithmv1::MinimumFOVTotal = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv1_MaximumFOVTotal"))
				AimAssist::Algorithmv1::MaximumFOVTotal = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv1_AccelerationFactor"))
				AimAssist::Algorithmv1::AccelerationFactor = std::stof(value);
			if (variable == xor ("AimAssist_DrawDebugOverlay"))
				AimAssist::DrawDebugOverlay = value == xor ("1");
			if (variable == xor ("AimAssist_Algorithmv2_Power"))
				AimAssist::Algorithmv2::Power = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv2_AssistOnSliders"))
				AimAssist::Algorithmv2::AssistOnSliders = value == xor ("1");
			if (variable == xor ("AimAssist_Algorithmv3_Power"))
				AimAssist::Algorithmv3::Power = std::stof(value);
			if (variable == xor ("AimAssist_Algorithmv3_SliderAssistPower"))
				AimAssist::Algorithmv3::SliderAssistPower = std::stof(value);
		}

		if (variable == xor ("Timewarp_Enabled"))
			Timewarp::Enabled = value == xor ("1");
		if (variable == xor ("Timewarp_Type"))
			Timewarp::Type = std::stoi(value);
		if (variable == xor ("Timewarp_Rate"))
			Timewarp::Rate = std::stoi(value);
		if (variable == xor ("Timewarp_Multiplier"))
			Timewarp::Multiplier = std::stof(value);

		if (variable == xor ("Visuals_ARChanger_Enabled"))
			Visuals::ARChanger::Enabled = value == xor ("1");
		if (variable == xor ("Visuals_ARChanger_AR"))
			Visuals::ARChanger::AR = std::stof(value);
		if (variable == xor ("Visuals_ARChanger_AdjustToMods"))
			Visuals::ARChanger::AdjustToMods = value == xor ("1");
		if (variable == xor ("Visuals_ARChanger_AdjustToRate"))
			Visuals::ARChanger::AdjustToRate = value == xor ("1");
		if (variable == xor ("Visuals_ARChanger_DrawPreemptiveDot"))
			Visuals::ARChanger::DrawPreemptiveDot = value == xor ("1");
		if (variable == xor ("Visuals_ARChanger_PreemptiveDotColour"))
			Visuals::ARChanger::PreemptiveDotColour = parseImVec4(value);
		if (variable == xor ("Visuals_CSChanger_Enabled"))
			Visuals::CSChanger::Enabled = value == xor ("1");
		if (variable == xor ("Visuals_CSChanger_CS"))
			Visuals::CSChanger::CS = std::stof(value);
		if (variable == xor ("Visuals_Removers_HiddenRemoverEnabled"))
			Visuals::Removers::HiddenRemoverEnabled = value == xor ("1");
		if (variable == xor ("Visuals_Removers_FlashlightRemoverEnabled"))
			Visuals::Removers::FlashlightRemoverEnabled = value == xor ("1");
		if (variable == xor ("Visuals_UI_MenuScale"))
			Visuals::UI::MenuScale = std::stoi(value);
		if (variable == xor ("Visuals_UI_MenuBackground"))
			strcpy_s(Visuals::UI::MenuBackground, value.c_str());
		if (variable == xor ("Visuals_UI_Snow"))
			Visuals::UI::Snow = value == xor ("1");
		if (variable == xor ("Visuals_UI_AccentColour"))
			Visuals::UI::AccentColour = parseImVec4(value);
		if (variable == xor ("Visuals_UI_MenuColour"))
			Visuals::UI::MenuColour = parseImVec4(value);
		if (variable == xor ("Visuals_UI_ControlColour"))
			Visuals::UI::ControlColour = parseImVec4(value);
		if (variable == xor ("Visuals_UI_TextColour"))
			Visuals::UI::TextColour = parseImVec4(value);

		if (variable == xor ("Misc_ScoreSubmissionType"))
			Misc::ScoreSubmissionType = std::stoi(value);
		if (variable == xor ("Misc_DisableSpectators"))
			Misc::DisableSpectators = value == xor ("1");
		if (variable == xor ("Misc_Logging_DisableLogging"))
			Misc::Logging::DisableLogging = value == xor ("1");
		if (variable == xor("Misc_DiscordRichPresenceSpoofer_Enabled"))
			Misc::DiscordRichPresenceSpoofer::Enabled = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomLargeImageTextEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomLargeImageText"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, value.c_str());
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomPlayModeEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomPlayMode"))
			Misc::DiscordRichPresenceSpoofer::CustomPlayMode = std::stoi(value);
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomStateEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomStateEnabled = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomState"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomState, value.c_str());
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomDetailsEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_CustomDetails"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomDetails, value.c_str());
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_HideSpectateButton"))
			Misc::DiscordRichPresenceSpoofer::HideSpectateButton = value == xor ("1");
		if (variable == xor ("Misc_DiscordRichPresenceSpoofer_HideSpectateButton"))
			Misc::DiscordRichPresenceSpoofer::HideMatchButton = value == xor ("1");
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

	const std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + Configs[CurrentConfig] + xor (".cfg");

	std::ofstream ofs;
	ofs.open(configFilePath, std::ofstream::out | std::ofstream::trunc);

	ofs << xor ("Version=") << VERSION << std::endl;

	ofs << xor ("Relax_Enabled=") << Relax::Enabled << std::endl;
	ofs << xor ("Relax_ToggleKey=") << Relax::ToggleKey << std::endl;
	ofs << xor ("Relax_PrimaryKey=") << Relax::PrimaryKey << std::endl;
	ofs << xor ("Relax_SecondaryKey=") << Relax::SecondaryKey << std::endl;
	ofs << xor ("Relax_AlternateBPM=") << Relax::AlternateBPM << std::endl;
	ofs << xor ("Relax_SliderAlternationOverride=") << Relax::SliderAlternationOverride << std::endl;
	ofs << xor ("Relax_Timing_Offset=") << Relax::Timing::Offset << std::endl;
	ofs << xor ("Relax_Timing_TargetUnstableRate=") << Relax::Timing::TargetUnstableRate << std::endl;
	ofs << xor ("Relax_Timing_AverageHoldTime=") << Relax::Timing::AverageHoldTime << std::endl;
	ofs << xor ("Relax_Timing_AverageHoldTimeError=") << Relax::Timing::AverageHoldTimeError << std::endl;
	ofs << xor ("Relax_Timing_AverageSliderHoldTime=") << Relax::Timing::AverageSliderHoldTime << std::endl;
	ofs << xor ("Relax_Timing_AverageSliderHoldTimeError=") << Relax::Timing::AverageSliderHoldTimeError << std::endl;
	ofs << xor ("Relax_HitScan_WaitLate_Enabled=") << Relax::HitScan::WaitLateEnabled << std::endl;
	ofs << xor ("Relax_HitScan_DirectionPrediction_Enabled=") << Relax::HitScan::DirectionPredictionEnabled << std::endl;
	ofs << xor ("Relax_HitScan_DirectionPrediction_Angle=") << Relax::HitScan::DirectionPredictionAngle << std::endl;
	ofs << xor ("Relax_HitScan_DirectionPrediction_Scale=") << Relax::HitScan::DirectionPredictionScale << std::endl;
	ofs << xor ("Relax_Blatant_UseLowestPossibleHoldTimes=") << Relax::Blatant::UseLowestPossibleHoldTimes << std::endl;

	ofs << xor ("AimAssist_Enabled=") << AimAssist::Enabled << std::endl;
	ofs << xor ("AimAssist_Algorithm=") << AimAssist::Algorithm << std::endl;
	ofs << xor ("AimAssist_DrawDebugOverlay=") << AimAssist::DrawDebugOverlay << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_Strength=") << AimAssist::Algorithmv1::Strength << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_AssistOnSliders=") << AimAssist::Algorithmv1::AssistOnSliders << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_BaseFOV=") << AimAssist::Algorithmv1::BaseFOV << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_MaximumFOVScale=") << AimAssist::Algorithmv1::MaximumFOVScale << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_MinimumFOVTotal=") << AimAssist::Algorithmv1::MinimumFOVTotal << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_MaximumFOVTotal=") << AimAssist::Algorithmv1::MaximumFOVTotal << std::endl;
	ofs << xor ("AimAssist_Algorithmv1_AccelerationFactor=") << AimAssist::Algorithmv1::AccelerationFactor << std::endl;
	ofs << xor ("AimAssist_Algorithmv2_Power=") << AimAssist::Algorithmv2::Power << std::endl;
	ofs << xor ("AimAssist_Algorithmv2_AssistOnSliders=") << AimAssist::Algorithmv2::AssistOnSliders << std::endl;
	ofs << xor ("AimAssist_Algorithmv3_Power=") << AimAssist::Algorithmv3::Power << std::endl;
	ofs << xor ("AimAssist_Algorithmv3_SliderAssistPower=") << AimAssist::Algorithmv3::SliderAssistPower << std::endl;

	ofs << xor ("Timewarp_Enabled=") << Timewarp::Enabled << std::endl;
	ofs << xor ("Timewarp_Type=") << Timewarp::Type << std::endl;
	ofs << xor ("Timewarp_Rate=") << Timewarp::Rate << std::endl;
	ofs << xor ("Timewarp_Multiplier=") << Timewarp::Multiplier << std::endl;

	ofs << xor ("Visuals_ARChanger_Enabled=") << Visuals::ARChanger::Enabled << std::endl;
	ofs << xor ("Visuals_ARChanger_AR=") << Visuals::ARChanger::AR << std::endl;
	ofs << xor ("Visuals_ARChanger_AdjustToMods=") << Visuals::ARChanger::AdjustToMods << std::endl;
	ofs << xor ("Visuals_ARChanger_AdjustToRate=") << Visuals::ARChanger::AdjustToRate << std::endl;
	ofs << xor ("Visuals_ARChanger_DrawPreemptiveDot=") << Visuals::ARChanger::DrawPreemptiveDot << std::endl;
	ofs << xor ("Visuals_ARChanger_PreemptiveDotColour=") << Visuals::ARChanger::PreemptiveDotColour.x << "," << Visuals::ARChanger::PreemptiveDotColour.y << "," << Visuals::ARChanger::PreemptiveDotColour.z << "," << Visuals::ARChanger::PreemptiveDotColour.w << std::endl;
	ofs << xor ("Visuals_CSChanger_Enabled=") << Visuals::CSChanger::Enabled << std::endl;
	ofs << xor ("Visuals_CSChanger_CS=") << Visuals::CSChanger::CS << std::endl;
	ofs << xor ("Visuals_Removers_HiddenRemoverEnabled=") << Visuals::Removers::HiddenRemoverEnabled << std::endl;
	ofs << xor ("Visuals_Removers_FlashlightRemoverEnabled=") << Visuals::Removers::FlashlightRemoverEnabled << std::endl;
	ofs << xor ("Visuals_UI_MenuScale=") << Visuals::UI::MenuScale << std::endl;
	ofs << xor ("Visuals_UI_MenuBackground=") << Visuals::UI::MenuBackground << std::endl;
	ofs << xor ("Visuals_UI_Snow=") << Visuals::UI::Snow << std::endl;
	ofs << xor ("Visuals_UI_AccentColour=") << Visuals::UI::AccentColour.x << "," << Visuals::UI::AccentColour.y << "," << Visuals::UI::AccentColour.z << "," << Visuals::UI::AccentColour.w << std::endl;
	ofs << xor ("Visuals_UI_MenuColour=") << Visuals::UI::MenuColour.x << "," << Visuals::UI::MenuColour.y << "," << Visuals::UI::MenuColour.z << "," << Visuals::UI::MenuColour.w << std::endl;
	ofs << xor ("Visuals_UI_ControlColour=") << Visuals::UI::ControlColour.x << "," << Visuals::UI::ControlColour.y << "," << Visuals::UI::ControlColour.z << "," << Visuals::UI::ControlColour.w << std::endl;
	ofs << xor ("Visuals_UI_TextColour=") << Visuals::UI::TextColour.x << "," << Visuals::UI::TextColour.y << "," << Visuals::UI::TextColour.z << "," << Visuals::UI::TextColour.w << std::endl;
	
	ofs << xor ("Misc_ScoreSubmissionType=") << Misc::ScoreSubmissionType << std::endl;
	ofs << xor ("Misc_DisableSpectators=") << Misc::DisableSpectators << std::endl;
	ofs << xor ("Misc_Logging_DisableLogging=") << Misc::Logging::DisableLogging << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_Enabled=") << Misc::DiscordRichPresenceSpoofer::Enabled << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomLargeImageTextEnabled=") << Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomLargeImageText=") << Misc::DiscordRichPresenceSpoofer::CustomLargeImageText << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomPlayModeEnabled=") << Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomPlayMode=") << Misc::DiscordRichPresenceSpoofer::CustomPlayMode << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomStateEnabled=") << Misc::DiscordRichPresenceSpoofer::CustomStateEnabled << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomState=") << Misc::DiscordRichPresenceSpoofer::CustomState << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomDetailsEnabled=") << Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_CustomDetails=") << Misc::DiscordRichPresenceSpoofer::CustomDetails << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_HideSpectateButton=") << Misc::DiscordRichPresenceSpoofer::HideSpectateButton << std::endl;
	ofs << xor ("Misc_DiscordRichPresenceSpoofer_HideMatchButton=") << Misc::DiscordRichPresenceSpoofer::HideMatchButton << std::endl;

	ofs.close();

	STR_ENCRYPT_END
}

void Config::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + Configs[CurrentConfig] + xor (".cfg");

	std::filesystem::remove(configFilePath);
	
	refresh();

	CurrentConfig = 0;

	Load();
}

void Config::Import()
{
	STR_ENCRYPT_START

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string encodedConfigData = ClipboardUtilities::Read();

	if (encodedConfigData.empty())
		return;

	const std::string decodedConfigData = CryptoUtilities::MapleXOR(CryptoUtilities::Base64Decode(encodedConfigData), xor ("xbb9tuvQCGJRhN8z"));
	const std::vector<std::string> decodedConfigDataSplit = StringUtilities::Split(decodedConfigData, "|");

	if (decodedConfigDataSplit.size() < 2 || decodedConfigDataSplit.size() > 2)
		return;

	std::string configName = CryptoUtilities::Base64Decode(decodedConfigDataSplit[0]);
	const std::string configData = CryptoUtilities::Base64Decode(decodedConfigDataSplit[1]);

	std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + configName + xor (".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xor ("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xor ("\\") + newConfigName + xor (".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				configName = newConfigName;
				configFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream ofs(configFilePath);
	ofs << configData << std::endl;
	ofs.close();

	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), configName);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);

	Load();

	STR_ENCRYPT_END
}

void Config::Export()
{
	STR_ENCRYPT_START
	
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + Configs[CurrentConfig] + xor (".cfg");

	std::ifstream ifs(configFilePath);
	const std::string configData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	const std::string encodedConfigData = CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(CryptoUtilities::Base64Encode(Configs[CurrentConfig]) + xor ("|") + CryptoUtilities::Base64Encode(configData), xor ("xbb9tuvQCGJRhN8z")));

	ClipboardUtilities::Write(encodedConfigData);

	STR_ENCRYPT_END
}

void Config::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	
	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + Configs[CurrentConfig] + xor (".cfg");

	if (!Storage::IsValidFileName(RenamedConfigName) || Storage::IsSameFileName(RenamedConfigName, Configs[CurrentConfig]))
		return;

	std::string renamedConfigName = RenamedConfigName;
	std::string renamedConfigFilePath = Storage::ConfigsDirectory + xor ("\\") + renamedConfigName + xor (".cfg");

	if (std::filesystem::exists(renamedConfigFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = renamedConfigName + xor ("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xor ("\\") + newConfigName + xor (".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				renamedConfigName = newConfigName;
				renamedConfigFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}
	
	std::rename(configFilePath.c_str(), renamedConfigFilePath.c_str());

	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), renamedConfigName);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);
}

void Config::Create()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	std::string configName = NewConfigName;
	std::string configFilePath = Storage::ConfigsDirectory + xor ("\\") + NewConfigName + xor (".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xor ("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xor ("\\") + newConfigName + xor (".cfg");
			if (!std::filesystem::exists(newConfigFilePath))
			{
				configName = newConfigName;
				configFilePath = newConfigFilePath;

				break;
			}

			i++;
		}
	}

	std::ofstream ofs(configFilePath);
	ofs.close();
	
	refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), configName);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);

	loadDefaults();
}