#include "Config.h"

#include <filesystem>
#include <fstream>

#include <ThemidaSDK.h>

#include "../../Utilities/Security/xorstr.hpp"

void Config::ensureDirectoryExists()
{
	if (!std::filesystem::exists(Directory))
		std::filesystem::create_directory(Directory);
}

bool Config::isSameName(const std::string& a, const std::string& b)
{
	std::string aLowerCase = a;
	std::string bLowerCase = b;
	std::transform(aLowerCase.begin(), aLowerCase.end(), aLowerCase.begin(), tolower);
	std::transform(bLowerCase.begin(), bLowerCase.end(), bLowerCase.begin(), tolower);

	return aLowerCase == bLowerCase;
}

bool Config::isValidName(std::string name)
{
	return !name.empty() && !isSameName(name, xor ("default"));
}

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
	Relax::PredictionEnabled = false;
	Relax::SliderPredictionEnabled = false;
	Relax::PredictionAngle = 45;
	Relax::PredictionScale = 0.8f;

	AimAssist::Enabled = false;
	AimAssist::EasyMode = true;
	AimAssist::EasyModeStrength = 0.5f;
	AimAssist::Strength = 0.6f;
	AimAssist::BaseFOV = 45;
	AimAssist::MaximumFOVScale = 2.5f;
	AimAssist::MinimumFOVTotal = 0.f;
	AimAssist::MaximumFOVTotal = 225.f;
	AimAssist::SliderballDeadzone = 12.5f;
	AimAssist::FlipSliderballDeadzone = true;
	AimAssist::AssistOnSliders = true;
	AimAssist::StrengthMultiplier = 1.f;
	AimAssist::AssistDeadzone = 3.f;
	AimAssist::ResyncLeniency = 3.5f;
	AimAssist::ResyncLeniencyFactor = 0.698f;
	AimAssist::DrawDebugOverlay = false;

	Timewarp::Enabled = false;
	Timewarp::Rate = 100;

	Visuals::ARChangerEnabled = false;
	Visuals::AR = 9.2f;
	Visuals::ARChangerAdjustToMods = false;
	Visuals::HiddenDisabled = false;
	Visuals::FlashlightDisabled = false;
	Visuals::MenuScale = 2;
	Visuals::MenuBackground[0] = '\0';
	Visuals::AccentColour = ImColor(232, 93, 155, 255).Value;
	Visuals::MenuColour = ImColor(65, 65, 65, 255).Value;
	Visuals::ControlColour = ImColor(76, 76, 76, 255).Value;
	Visuals::TextColour = ImVec4(ImColor(255, 255, 255, 255));

	Misc::DisableSpectators = false;
	Misc::PromptOnScoreSubmissionEnabled = false;
	Misc::DisableLogging = false;
	Misc::RichPresenceSpooferEnabled = false;
	strcpy_s(Misc::SpoofedName, xor ("maple.software"));
	strcpy_s(Misc::SpoofedRank, xor("rocking osu! since 2021"));

	STR_ENCRYPT_END
}

void Config::Initialize(const std::string& directory)
{
	Directory = directory;

	Refresh();

	Load();
}

void Config::Load()
{
	STR_ENCRYPT_START
	
	ensureDirectoryExists();
	loadDefaults(); //load default config first to ensure that old configs are fully initialized

	if (CurrentConfig == 0)
		return;

	std::ifstream file(Directory + "\\" + Configs[CurrentConfig] + ".cfg");
	std::string line;

	while (std::getline(file, line))
	{
		const int delimiterIndex = line.find('=');
		std::string variable = line.substr(0, delimiterIndex);
		std::string value = line.substr(delimiterIndex + 1, std::string::npos);

		if (variable == "RelaxEnabled")
			Relax::Enabled = value == "1";
		if (variable == "RelaxDistribution")
			Relax::Distribution = std::stoi(value);
		if (variable == "RelaxPlaystyle")
			Relax::Playstyle = std::stoi(value);
		if (variable == "RelaxPrimaryKey")
			Relax::PrimaryKey = std::stoi(value);
		if (variable == "RelaxSecondaryKey")
			Relax::SecondaryKey = std::stoi(value);
		if (variable == "RelaxMaxSingletapBPM")
			Relax::MaxSingletapBPM = std::stoi(value);
		if (variable == "RelaxHitSpread")
			Relax::HitSpread = std::stoi(value);
		if (variable == "RelaxAlternationHitSpread")
			Relax::AlternationHitSpread = std::stoi(value);
		if (variable == "RelaxHoldConsecutiveSpinners")
			Relax::HoldConsecutiveSpinners = value == "1";
		if (variable == "RelaxSliderAlternationOverride")
			Relax::SliderAlternationOverride = value == "1";
		if (variable == "RelaxPredictionEnabled")
			Relax::PredictionEnabled = value == "1";
		if (variable == "RelaxSliderPredictionEnabled")
			Relax::SliderPredictionEnabled = value == "1";
		if (variable == "RelaxPredictionAngle")
			Relax::PredictionAngle = std::stoi(value);
		if (variable == "RelaxPredictionScale")
			Relax::PredictionScale = std::stof(value);

		if (variable == "AimAssistEnabled")
			AimAssist::Enabled = value == "1";
		if (variable == "AimAssistEasyMode")
			AimAssist::EasyMode = value == "1";
		if (variable == "AimAssistEasyModeStrength")
			AimAssist::EasyModeStrength = std::stof(value);
		if (variable == "AimAssistStrength")
			AimAssist::Strength = std::stof(value);
		if (variable == "AimAssistBaseFOV")
			AimAssist::BaseFOV = std::stoi(value);
		if (variable == "AimAssistMaximumFOVScale")
			AimAssist::MaximumFOVScale = std::stof(value);
		if (variable == "AimAssistMinimumFOVTotal")
			AimAssist::MinimumFOVTotal = std::stof(value);
		if (variable == "AimAssistMaximumFOVTotal")
			AimAssist::MaximumFOVTotal = std::stof(value);
		if (variable == "AimAssistAssistOnSliders")
			AimAssist::AssistOnSliders = value == "1";
		if (variable == "AimAssistStrengthMultiplier")
			AimAssist::StrengthMultiplier = std::stof(value);
		if (variable == "AimAssistAssistDeadzone")
			AimAssist::AssistDeadzone = std::stof(value);
		if (variable == "AimAssistResyncLeniency")
			AimAssist::ResyncLeniency = std::stof(value);
		if (variable == "AimAssistResyncLeniencyFactor")
			AimAssist::ResyncLeniencyFactor = std::stof(value);
		if (variable == "AimAssistDrawDebugOverlay")
			AimAssist::DrawDebugOverlay = value == "1";
		if (variable == "AimAssistFlipSliderballDeadzone")
			AimAssist::FlipSliderballDeadzone = value == "1";
		if (variable == "AimAssistSliderballDeadzone")
			AimAssist::SliderballDeadzone = std::stof(value);

		if (variable == "TimewarpEnabled")
			Timewarp::Enabled = value == "1";
		if (variable == "TimewarpRate")
			Timewarp::Rate = std::stoi(value);

		if (variable == "VisualsARChangerEnabled")
			Visuals::ARChangerEnabled = value == "1";
		if (variable == "VisualsARChangerAR")
			Visuals::AR = std::stof(value);
		if (variable == "VisualsAdjustToMods")
			Visuals::ARChangerAdjustToMods = value == "1";
		if (variable == "VisualsHiddenDisabled")
			Visuals::HiddenDisabled = value == "1";
		if (variable == "VisualsFlashlightDisabled")
			Visuals::FlashlightDisabled = value == "1";
		if (variable == "VisualsMenuScale")
			Visuals::MenuScale = std::stoi(value);
		if (variable == "VisualsMenuBackground")
			strcpy_s(Visuals::MenuBackground, value.c_str());
		if (variable == "VisualsAccentColour")
			Visuals::AccentColour = parseImVec4(value);
		if (variable == "VisualsMenuColour")
			Visuals::MenuColour = parseImVec4(value);
		if (variable == "VisualsControlColour")
			Visuals::ControlColour = parseImVec4(value);
		if (variable == "VisualsTextColour")
			Visuals::TextColour = parseImVec4(value);

		if (variable == "MiscDisableSpectators")
			Misc::DisableSpectators = value == "1";
		if (variable == "MiscPromptOnScoreSubmissionEnabled")
			Misc::PromptOnScoreSubmissionEnabled = value == "1";
		if (variable == "MiscDisableLogging")
			Misc::DisableLogging = value == "1";
		if (variable == "MiscRichPresenceSpooferEnabled")
			Misc::RichPresenceSpooferEnabled = value == "1";
		if (variable == "MiscSpoofedName")
			strcpy_s(Misc::SpoofedName, value.c_str());
		if (variable == "MiscSpoofedRank")
			strcpy_s(Misc::SpoofedRank, value.c_str());
	}

	file.close();

	STR_ENCRYPT_END
}

void Config::Save()
{
	STR_ENCRYPT_START
	
	if (CurrentConfig == 0)
		return;

	ensureDirectoryExists();

	const std::string cfgPath = Directory + "\\" + Configs[CurrentConfig] + ".cfg";

	std::ofstream ofs;
	ofs.open(cfgPath, std::ofstream::out | std::ofstream::trunc);

	ofs << "RelaxEnabled=" << Relax::Enabled << std::endl;
	ofs << "RelaxDistribution=" << Relax::Distribution << std::endl;
	ofs << "RelaxPlaystyle=" << Relax::Playstyle << std::endl;
	ofs << "RelaxPrimaryKey=" << Relax::PrimaryKey << std::endl;
	ofs << "RelaxSecondaryKey=" << Relax::SecondaryKey << std::endl;
	ofs << "RelaxMaxSingletapBPM=" << Relax::MaxSingletapBPM << std::endl;
	ofs << "RelaxHitSpread=" << Relax::HitSpread << std::endl;
	ofs << "RelaxAlternationHitSpread=" << Relax::AlternationHitSpread << std::endl;
	ofs << "RelaxHoldConsecutiveSpinners=" << Relax::HoldConsecutiveSpinners << std::endl;
	ofs << "RelaxSliderAlternationOverride=" << Relax::SliderAlternationOverride << std::endl;
	ofs << "RelaxPredictionEnabled=" << Relax::PredictionEnabled << std::endl;
	ofs << "RelaxSliderPredictionEnabled=" << Relax::SliderPredictionEnabled << std::endl;
	ofs << "RelaxPredictionAngle=" << Relax::PredictionAngle << std::endl;
	ofs << "RelaxPredictionScale=" << Relax::PredictionScale << std::endl;

	ofs << "AimAssistEnabled=" << AimAssist::Enabled << std::endl;
	ofs << "AimAssistEasyMode=" << AimAssist::EasyMode << std::endl;
	ofs << "AimAssistEasyModeStrength=" << AimAssist::EasyModeStrength << std::endl;
	ofs << "AimAssistStrength=" << AimAssist::Strength << std::endl;
	ofs << "AimAssistBaseFOV=" << AimAssist::BaseFOV << std::endl;
	ofs << "AimAssistMaximumFOVScale=" << AimAssist::MaximumFOVScale << std::endl;
	ofs << "AimAssistMinimumFOVTotal=" << AimAssist::MinimumFOVTotal << std::endl;
	ofs << "AimAssistMaximumFOVTotal=" << AimAssist::MaximumFOVTotal << std::endl;
	ofs << "AimAssistAssistOnSliders=" << AimAssist::AssistOnSliders << std::endl;
	ofs << "AimAssistStrengthMultiplier=" << AimAssist::StrengthMultiplier << std::endl;
	ofs << "AimAssistAssistDeadzone=" << AimAssist::AssistDeadzone << std::endl;
	ofs << "AimAssistResyncLeniency=" << AimAssist::ResyncLeniency << std::endl;
	ofs << "AimAssistResyncLeniencyFactor=" << AimAssist::ResyncLeniencyFactor << std::endl;
	ofs << "AimAssistDrawDebugOverlay=" << AimAssist::DrawDebugOverlay << std::endl;
	ofs << "AimAssistFlipSliderballDeadzone=" << AimAssist::FlipSliderballDeadzone << std::endl;
	ofs << "AimAssistSliderballDeadzone=" << AimAssist::SliderballDeadzone << std::endl;

	ofs << "TimewarpEnabled=" << Timewarp::Enabled << std::endl;
	ofs << "TimewarpRate=" << Timewarp::Rate << std::endl;

	ofs << "VisualsARChangerEnabled=" << Visuals::ARChangerEnabled << std::endl;
	ofs << "VisualsARChangerAR=" << Visuals::AR << std::endl;
	ofs << "VisualsAdjustToMods=" << Visuals::ARChangerAdjustToMods << std::endl;
	ofs << "VisualsHiddenDisabled=" << Visuals::HiddenDisabled << std::endl;
	ofs << "VisualsFlashlightDisabled=" << Visuals::FlashlightDisabled << std::endl;
	ofs << "VisualsMenuScale=" << Visuals::MenuScale << std::endl;
	ofs << "VisualsMenuBackground=" << Visuals::MenuBackground << std::endl;
	ofs << "VisualsAccentColour=" << Visuals::AccentColour.x << "," << Visuals::AccentColour.y << "," << Visuals::AccentColour.z << "," << Visuals::AccentColour.w << std::endl;
	ofs << "VisualsMenuColour=" << Visuals::MenuColour.x << "," << Visuals::MenuColour.y << "," << Visuals::MenuColour.z << "," << Visuals::MenuColour.w << std::endl;
	ofs << "VisualsControlColour=" << Visuals::ControlColour.x << "," << Visuals::ControlColour.y << "," << Visuals::ControlColour.z << "," << Visuals::ControlColour.w << std::endl;
	ofs << "VisualsTextColour=" << Visuals::TextColour.x << "," << Visuals::TextColour.y << "," << Visuals::TextColour.z << "," << Visuals::TextColour.w << std::endl;

	ofs << "MiscDisableSpectators=" << Misc::DisableSpectators << std::endl;
	ofs << "MiscPromptOnScoreSubmissionEnabled=" << Misc::PromptOnScoreSubmissionEnabled << std::endl;
	ofs << "MiscDisableLogging=" << Misc::DisableLogging << std::endl;
	ofs << "MiscRichPresenceSpooferEnabled=" << Misc::RichPresenceSpooferEnabled << std::endl;
	ofs << "MiscSpoofedName=" << Misc::SpoofedName << std::endl;
	ofs << "MiscSpoofedRank=" << Misc::SpoofedRank << std::endl;

	ofs.close();

	STR_ENCRYPT_END
}

void Config::Create()
{
	ensureDirectoryExists();

	const std::string cfgPath = Directory + "\\" + NewConfigName + ".cfg";

	if (!isValidName(NewConfigName) || std::filesystem::exists(cfgPath))
		return;

	std::ofstream ofs(cfgPath);
	ofs.close();

	Refresh();

	const auto it = std::find(Configs.begin(), Configs.end(), NewConfigName);

	if (it != Configs.end())
		CurrentConfig = std::distance(Configs.begin(), it);

	loadDefaults();
}

void Config::Refresh()
{
	ensureDirectoryExists();

	Configs.clear();
	Configs.emplace_back("default");

	for (const auto& file : std::filesystem::directory_iterator(Directory))
		if (file.path().extension() == ".cfg" && isValidName(file.path().filename().stem().string()))
			Configs.push_back(file.path().filename().stem().string());
}