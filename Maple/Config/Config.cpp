#include "Config.h"

#include <filesystem>
#include <fstream>

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Storage/Storage.h"
#include "../Storage/StorageConfig.h"
#include "../Utilities/Clipboard/ClipboardUtilities.h"
#include "../Utilities/Crypto/CryptoUtilities.h"
#include "../Utilities/Strings/StringUtilities.h"

std::string Config::encryptEntry(const std::string& key, const std::string& value)
{
	std::stringstream ss;
	ss << key << xorstr_("=") << value;

	return CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(ss.str(), xorstr_("kelxFFMHsiGnONNa")));
}

std::string Config::decryptEntry(const std::string& entry)
{
	return CryptoUtilities::MapleXOR(CryptoUtilities::Base64Decode(entry), xorstr_("kelxFFMHsiGnONNa"));
}

ImVec4 Config::parseImVec4(std::string vec) {
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

std::string Config::imvec4ToString(ImVec4 vec)
{
    std::stringstream ss;
    ss << std::to_string(vec.x) << "," << std::to_string(vec.y) << "," << std::to_string(vec.z) << "," << std::to_string(vec.w);

	return ss.str();
}

void Config::loadDefaults()
{
	
	Relax::Enabled = false;
	Relax::ToggleKey = VK_RCONTROL;
	Relax::PrimaryKey = 1;
	Relax::SecondaryKey = 3;
	Relax::AlternateBPM = 100;
	Relax::SliderAlternationOverride = false;
	Relax::Timing::Offset = 0;
	Relax::Timing::TargetUnstableRate = 100;
	Relax::Timing::AllowableHitRange = 300;
	Relax::Timing::MinimumHoldTime = 60;
	Relax::Timing::MaximumHoldTime = 80;
	Relax::Timing::MinimumSliderHoldTime = 50;
	Relax::Timing::MaximumSliderHoldTime = 65;
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
	Timewarp::RateLimitEnabled = true;

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
	Misc::PromptBehaviorOnRetry = 0;
	Misc::DisableSpectators = false;
	Misc::DiscordRichPresenceSpoofer::Enabled = false;
	Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, xorstr_("peppy (rank #18,267,309)"));
	Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled = false;
	Misc::DiscordRichPresenceSpoofer::CustomPlayMode = 0;
	Misc::DiscordRichPresenceSpoofer::CustomStateEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomState, xorstr_("Trying to detect Maple since 2021"));
	Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled = false;
	strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomDetails, xorstr_("Kenji Ninuma - DISCO PRINCE [Normal]"));
	Misc::DiscordRichPresenceSpoofer::HideSpectateButton = false;
	Misc::DiscordRichPresenceSpoofer::HideMatchButton = false;

}

void Config::refresh()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	Configs.clear();
	Configs.emplace_back(xorstr_("default"));

	for (const auto& file : std::filesystem::directory_iterator(Storage::ConfigsDirectory))
		if (file.path().extension() == xorstr_(".cfg") && Storage::IsValidFileName(file.path().filename().stem().string()))
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
	
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	loadDefaults(); //load default config first to ensure that old configs are fully initialized

	StorageConfig::DefaultConfig = Configs[CurrentConfig];
	Storage::SaveStorageConfig();

	if (CurrentConfig == 0)
		return;

	std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfig] + xorstr_(".cfg");

	if (!std::filesystem::exists(configFilePath))
		return;

	std::ifstream file(configFilePath);
	std::string line;

	float configVersion = 0.f;
	while (std::getline(file, line))
	{
        std::string lineDecrypted = decryptEntry(line);
		const int delimiterIndex = lineDecrypted.find('=');
		std::string variable = lineDecrypted.substr(0, delimiterIndex);
		std::string value = lineDecrypted.substr(delimiterIndex + 1, std::string::npos);

		if (variable == xorstr_("Version"))
			configVersion = std::stof(value);

		if (variable == xorstr_("Relax_Enabled"))
			Relax::Enabled = value == xorstr_("1");
		if (variable == xorstr_("Relax_ToggleKey"))
			Relax::ToggleKey = std::stoi(value);
		if (variable == xorstr_("Relax_PrimaryKey"))
			Relax::PrimaryKey = std::stoi(value);
		if (variable == xorstr_("Relax_SecondaryKey"))
			Relax::SecondaryKey = std::stoi(value);
		if (variable == xorstr_("Relax_AlternateBPM"))
			Relax::AlternateBPM = std::stoi(value);
		if (variable == xorstr_("Relax_SliderAlternationOverride"))
			Relax::SliderAlternationOverride = value == xorstr_("1");
		if (variable == xorstr_("Relax_Timing_Offset"))
			Relax::Timing::Offset = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_TargetUnstableRate"))
			Relax::Timing::TargetUnstableRate = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_AllowableHitRange"))
			Relax::Timing::AllowableHitRange = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_MinimumHoldTime"))
			Relax::Timing::MinimumHoldTime = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_MaximumHoldTime"))
			Relax::Timing::MaximumHoldTime = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_MinimumSliderHoldTime"))
			Relax::Timing::MinimumSliderHoldTime = std::stoi(value);
		if (variable == xorstr_("Relax_Timing_MaximumSliderHoldTime"))
			Relax::Timing::MaximumSliderHoldTime = std::stoi(value);
		
		if (variable == xorstr_("Relax_HitScan_DirectionPrediction_Enabled"))
			Relax::HitScan::DirectionPredictionEnabled = value == xorstr_("1");
		if (variable == xorstr_("Relax_HitScan_DirectionPrediction_Angle"))
			Relax::HitScan::DirectionPredictionAngle = std::stoi(value);
		if (variable == xorstr_("Relax_HitScan_DirectionPrediction_Scale"))
			Relax::HitScan::DirectionPredictionScale = std::stof(value);

		if (variable == xorstr_("Relax_Blatant_UseLowestPossibleHoldTimes"))
			Relax::Blatant::UseLowestPossibleHoldTimes = value == xorstr_("1");

		if (variable == xorstr_("Timewarp_Enabled"))
			Timewarp::Enabled = value == xorstr_("1");
		if (variable == xorstr_("Timewarp_Type"))
			Timewarp::Type = std::stoi(value);
		if (variable == xorstr_("Timewarp_Rate"))
			Timewarp::Rate = std::stoi(value);
		if (variable == xorstr_("Timewarp_Multiplier"))
			Timewarp::Multiplier = std::stof(value);
        if (variable == xorstr_("Timewarp_RateLimitEnabled"))
                Timewarp::RateLimitEnabled = value == xorstr_("1");

		if (variable == xorstr_("Visuals_ARChanger_Enabled"))
			Visuals::ARChanger::Enabled = value == xorstr_("1");
		if (variable == xorstr_("Visuals_ARChanger_AR"))
			Visuals::ARChanger::AR = std::stof(value);
		if (variable == xorstr_("Visuals_ARChanger_AdjustToMods"))
			Visuals::ARChanger::AdjustToMods = value == xorstr_("1");
		if (variable == xorstr_("Visuals_ARChanger_AdjustToRate"))
			Visuals::ARChanger::AdjustToRate = value == xorstr_("1");
		if (variable == xorstr_("Visuals_ARChanger_DrawPreemptiveDot"))
			Visuals::ARChanger::DrawPreemptiveDot = value == xorstr_("1");
		if (variable == xorstr_("Visuals_ARChanger_PreemptiveDotColour"))
			Visuals::ARChanger::PreemptiveDotColour = parseImVec4(value);
		if (variable == xorstr_("Visuals_CSChanger_Enabled"))
			Visuals::CSChanger::Enabled = value == xorstr_("1");
		if (variable == xorstr_("Visuals_CSChanger_CS"))
			Visuals::CSChanger::CS = std::stof(value);
		if (variable == xorstr_("Visuals_Removers_HiddenRemoverEnabled"))
			Visuals::Removers::HiddenRemoverEnabled = value == xorstr_("1");
		if (variable == xorstr_("Visuals_Removers_FlashlightRemoverEnabled"))
			Visuals::Removers::FlashlightRemoverEnabled = value == xorstr_("1");
		if (variable == xorstr_("Visuals_UI_MenuScale"))
			Visuals::UI::MenuScale = std::stoi(value);
		if (variable == xorstr_("Visuals_UI_MenuBackground"))
			strcpy_s(Visuals::UI::MenuBackground, value.c_str());
		if (variable == xorstr_("Visuals_UI_Snow"))
			Visuals::UI::Snow = value == xorstr_("1");
		if (variable == xorstr_("Visuals_UI_AccentColour"))
			Visuals::UI::AccentColour = parseImVec4(value);
		if (variable == xorstr_("Visuals_UI_MenuColour"))
			Visuals::UI::MenuColour = parseImVec4(value);
		if (variable == xorstr_("Visuals_UI_ControlColour"))
			Visuals::UI::ControlColour = parseImVec4(value);
		if (variable == xorstr_("Visuals_UI_TextColour"))
			Visuals::UI::TextColour = parseImVec4(value);

		if (variable == xorstr_("Misc_ScoreSubmissionType"))
			Misc::ScoreSubmissionType = std::stoi(value);
		if (variable == xorstr_("Misc_PromptBehaviorOnRetry"))
			Misc::PromptBehaviorOnRetry = std::stoi(value);
		if (variable == xorstr_("Misc_DisableSpectators"))
			Misc::DisableSpectators = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_Enabled"))
			Misc::DiscordRichPresenceSpoofer::Enabled = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomLargeImageTextEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomLargeImageText"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomLargeImageText, value.c_str());
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomPlayModeEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomPlayMode"))
			Misc::DiscordRichPresenceSpoofer::CustomPlayMode = std::stoi(value);
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomStateEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomStateEnabled = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomState"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomState, value.c_str());
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomDetailsEnabled"))
			Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_CustomDetails"))
			strcpy_s(Misc::DiscordRichPresenceSpoofer::CustomDetails, value.c_str());
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_HideSpectateButton"))
			Misc::DiscordRichPresenceSpoofer::HideSpectateButton = value == xorstr_("1");
		if (variable == xorstr_("Misc_DiscordRichPresenceSpoofer_HideSpectateButton"))
			Misc::DiscordRichPresenceSpoofer::HideMatchButton = value == xorstr_("1");
	}

	file.close();
}

void Config::Save()
{
	if (CurrentConfig == 0)
		return;

	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfig] + xorstr_(".cfg");

	std::ofstream ofs;
	ofs.open(configFilePath, std::ofstream::out | std::ofstream::trunc);

	ofs << encryptEntry(xorstr_("Version"), std::to_string(VERSION)) << std::endl;

	ofs << encryptEntry(xorstr_("Relax_Enabled"), std::to_string(Relax::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_ToggleKey"), std::to_string(Relax::ToggleKey)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_PrimaryKey"), std::to_string(Relax::PrimaryKey)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_SecondaryKey"), std::to_string(Relax::SecondaryKey)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_AlternateBPM"), std::to_string(Relax::AlternateBPM)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_SliderAlternationOverride"), std::to_string(Relax::SliderAlternationOverride)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_Timing_Offset"), std::to_string(Relax::Timing::Offset)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_Timing_TargetUnstableRate"), std::to_string(Relax::Timing::TargetUnstableRate)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_Timing_AllowableHitRange"), std::to_string(Relax::Timing::AllowableHitRange)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_Timing_MinimumHoldTime"), std::to_string(Relax::Timing::MinimumHoldTime)) << std::endl;
	ofs << encryptEntry(xorstr_("Relax_Timing_MaximumHoldTime"), std::to_string(Relax::Timing::MaximumHoldTime)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_Timing_MinimumSliderHoldTime"), std::to_string(Relax::Timing::MinimumSliderHoldTime)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_Timing_MaximumSliderHoldTime"), std::to_string(Relax::Timing::MaximumSliderHoldTime)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_HitScan_DirectionPrediction_Enabled"), std::to_string(Relax::HitScan::DirectionPredictionEnabled)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_HitScan_DirectionPrediction_Angle"), std::to_string(Relax::HitScan::DirectionPredictionAngle)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_HitScan_DirectionPrediction_Scale"), std::to_string(Relax::HitScan::DirectionPredictionScale)) << std::endl;
    ofs << encryptEntry(xorstr_("Relax_Blatant_UseLowestPossibleHoldTimes"), std::to_string(Relax::Blatant::UseLowestPossibleHoldTimes)) << std::endl;

	ofs << encryptEntry(xorstr_("AimAssist_Enabled"), std::to_string(AimAssist::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithm"), std::to_string(AimAssist::Algorithm)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_DrawDebugOverlay"), std::to_string(AimAssist::DrawDebugOverlay)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_Strength"), std::to_string(AimAssist::Algorithmv1::Strength)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_AssistOnSliders"), std::to_string(AimAssist::Algorithmv1::AssistOnSliders)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_BaseFOV"), std::to_string(AimAssist::Algorithmv1::BaseFOV)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_MaximumFOVScale"), std::to_string(AimAssist::Algorithmv1::MaximumFOVScale)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_MinimumFOVTotal"), std::to_string(AimAssist::Algorithmv1::MinimumFOVTotal)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_MaximumFOVTotal"), std::to_string(AimAssist::Algorithmv1::MaximumFOVTotal)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv1_AccelerationFactor"), std::to_string(AimAssist::Algorithmv1::AccelerationFactor)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv2_Power"), std::to_string(AimAssist::Algorithmv2::Power)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv2_AssistOnSliders"), std::to_string(AimAssist::Algorithmv2::AssistOnSliders)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv3_Power"), std::to_string(AimAssist::Algorithmv3::Power)) << std::endl;
	ofs << encryptEntry(xorstr_("AimAssist_Algorithmv3_SliderAssistPower"), std::to_string(AimAssist::Algorithmv3::SliderAssistPower)) << std::endl;

	ofs << encryptEntry(xorstr_("Timewarp_Enabled"), std::to_string(Timewarp::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Timewarp_Type"), std::to_string(Timewarp::Type)) << std::endl;
	ofs << encryptEntry(xorstr_("Timewarp_Rate"), std::to_string(Timewarp::Rate)) << std::endl;
	ofs << encryptEntry(xorstr_("Timewarp_Multiplier"), std::to_string(Timewarp::Multiplier)) << std::endl;
    ofs << encryptEntry(xorstr_("Timewarp_RateLimitEnabled"), std::to_string(Timewarp::RateLimitEnabled)) << std::endl;

	ofs << encryptEntry(xorstr_("Visuals_ARChanger_Enabled"), std::to_string(Visuals::ARChanger::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_ARChanger_AR"), std::to_string(Visuals::ARChanger::AR)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_ARChanger_AdjustToMods"), std::to_string(Visuals::ARChanger::AdjustToMods)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_ARChanger_AdjustToRate"), std::to_string(Visuals::ARChanger::AdjustToRate)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_ARChanger_DrawPreemptiveDot"), std::to_string(Visuals::ARChanger::DrawPreemptiveDot)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_ARChanger_PreemptiveDotColour"), imvec4ToString(Visuals::ARChanger::PreemptiveDotColour)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_CSChanger_Enabled"), std::to_string(Visuals::CSChanger::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_CSChanger_CS"), std::to_string(Visuals::CSChanger::CS)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_Removers_HiddenRemoverEnabled"), std::to_string(Visuals::Removers::HiddenRemoverEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_Removers_FlashlightRemoverEnabled"), std::to_string(Visuals::Removers::FlashlightRemoverEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_MenuScale"), std::to_string(Visuals::UI::MenuScale)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_MenuBackground"), Visuals::UI::MenuBackground) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_Snow"), std::to_string(Visuals::UI::Snow)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_AccentColour"), imvec4ToString(Visuals::UI::AccentColour)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_MenuColour"), imvec4ToString(Visuals::UI::MenuColour)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_ControlColour"), imvec4ToString(Visuals::UI::ControlColour)) << std::endl;
	ofs << encryptEntry(xorstr_("Visuals_UI_TextColour"), imvec4ToString(Visuals::UI::TextColour)) << std::endl;

	ofs << encryptEntry(xorstr_("Misc_ScoreSubmissionType"), std::to_string(Misc::ScoreSubmissionType)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_PromptBehaviorOnRetry"), std::to_string(Misc::PromptBehaviorOnRetry)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DisableSpectators"), std::to_string(Misc::DisableSpectators)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_Enabled"), std::to_string(Misc::DiscordRichPresenceSpoofer::Enabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomLargeImageTextEnabled"), std::to_string(Misc::DiscordRichPresenceSpoofer::CustomLargeImageTextEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomLargeImageText"), Misc::DiscordRichPresenceSpoofer::CustomLargeImageText) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomPlayModeEnabled"), std::to_string(Misc::DiscordRichPresenceSpoofer::CustomPlayModeEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomPlayMode"), std::to_string(Misc::DiscordRichPresenceSpoofer::CustomPlayMode)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomStateEnabled"), std::to_string(Misc::DiscordRichPresenceSpoofer::CustomStateEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomState"), Misc::DiscordRichPresenceSpoofer::CustomState) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomDetailsEnabled"), std::to_string(Misc::DiscordRichPresenceSpoofer::CustomDetailsEnabled)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_CustomDetails"), Misc::DiscordRichPresenceSpoofer::CustomDetails) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_HideSpectateButton"), std::to_string(Misc::DiscordRichPresenceSpoofer::HideSpectateButton)) << std::endl;
	ofs << encryptEntry(xorstr_("Misc_DiscordRichPresenceSpoofer_HideMatchButton"), std::to_string(Misc::DiscordRichPresenceSpoofer::HideMatchButton)) << std::endl;

	ofs.close();
}

void Config::Delete()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfig] + xorstr_(".cfg");

	std::filesystem::remove(configFilePath);
	
	refresh();

	CurrentConfig = 0;

	Load();
}

void Config::Import()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	const std::string encodedConfigData = ClipboardUtilities::Read();

	if (encodedConfigData.empty())
		return;

	const std::string decodedConfigData = CryptoUtilities::MapleXOR(CryptoUtilities::Base64Decode(encodedConfigData), xorstr_("kelxFFMHsiGnONNa"));
	const std::vector<std::string> decodedConfigDataSplit = StringUtilities::Split(decodedConfigData, "|");

	if (decodedConfigDataSplit.size() < 2 || decodedConfigDataSplit.size() > 2)
		return;

	std::string configName = CryptoUtilities::Base64Decode(decodedConfigDataSplit[0]);
	const std::string configData = CryptoUtilities::Base64Decode(decodedConfigDataSplit[1]);

	std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + configName + xorstr_(".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
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

}

void Config::Export()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);

	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfig] + xorstr_(".cfg");

	std::ifstream ifs(configFilePath);
	const std::string configData((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	ifs.close();

	const std::string encodedConfigData = CryptoUtilities::Base64Encode(CryptoUtilities::MapleXOR(CryptoUtilities::Base64Encode(Configs[CurrentConfig]) + xorstr_("|") + CryptoUtilities::Base64Encode(configData), xorstr_("kelxFFMHsiGnONNa")));

	ClipboardUtilities::Write(encodedConfigData);

}

void Config::Rename()
{
	Storage::EnsureDirectoryExists(Storage::ConfigsDirectory);
	
	if (CurrentConfig == 0)
		return;

	const std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + Configs[CurrentConfig] + xorstr_(".cfg");

	if (!Storage::IsValidFileName(RenamedConfigName) || Storage::IsSameFileName(RenamedConfigName, Configs[CurrentConfig]))
		return;

	std::string renamedConfigName = RenamedConfigName;
	std::string renamedConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + renamedConfigName + xorstr_(".cfg");

	if (std::filesystem::exists(renamedConfigFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = renamedConfigName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
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
	std::string configFilePath = Storage::ConfigsDirectory + xorstr_("\\") + NewConfigName + xorstr_(".cfg");

	if (!Storage::IsValidFileName(configName))
		return;

	if (std::filesystem::exists(configFilePath))
	{
		unsigned int i = 2;
		while (true)
		{
			const std::string newConfigName = configName + xorstr_("_") + std::to_string(i);
			const std::string newConfigFilePath = Storage::ConfigsDirectory + xorstr_("\\") + newConfigName + xorstr_(".cfg");
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