#pragma once

#include <Explorer/TypeExplorer.h>

enum class OsuModes
{
    Menu,
    Edit,
    Play,
    Exit,
    SelectEdit,
    SelectPlay,
    SelectDrawings,
    Rank,
    Update,
    Busy,
    Unknown,
    Lobby,
    MatchSetup,
    SelectMulti,
    RankingVs,
    OnlineSelection,
    OptionsOffsetWizard,
    RankingTagCoop,
    RankingTeam,
    BeatmapImport,
    PackageUpdater,
    Benchmark,
    Tourney,
    Charts
};

class GameBase
{
	static inline void* instanceAddress = nullptr;
	static inline void* modeAddress = nullptr;
public:
	static inline TypeExplorer RawGameBase;
	
	static void Initialize();
	static void* Instance();
    static OsuModes Mode();
};