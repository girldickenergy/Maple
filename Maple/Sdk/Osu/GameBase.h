#pragma once

#include <Explorer/TypeExplorer.h>
#include "../DataTypes/Structs/sRectangle.h"

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
    static inline Field clientBoundsField;

    typedef void* (__fastcall* fnGetWindow)(void* instance);
    static inline fnGetWindow getWindow = nullptr;

    typedef HWND (__fastcall* fnGetHandle)(void* instance);
    static inline fnGetHandle getHandle = nullptr;
public:
	static inline TypeExplorer RawGameBase;
	
	static void Initialize();
	static void* Instance();
    static OsuModes Mode();
    static sRectangle* GetClientBounds();
    static HWND GetWindowHandle();
};