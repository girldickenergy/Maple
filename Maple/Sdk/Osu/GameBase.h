#pragma once

#include <Explorer/TypeExplorer.h>
#include <COM/COMString.h>

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

    typedef void* (__fastcall* fnGetWindow)(void* instance);
    static inline fnGetWindow getWindow = nullptr;

    typedef HWND (__fastcall* fnGetHandle)(void* instance);
    static inline fnGetHandle getHandle = nullptr;

    static inline void* clientHashAddress = nullptr;
    static inline void* uniqueIDAddress = nullptr;
    static inline void* uniqueID2Address = nullptr;
    static inline void* uniqueCheckAddress = nullptr;

    static inline Field obfuscatedStringChangesField;

    typedef COMString*(__fastcall* fnObfuscatedStringGetValue)(void* instance);
    static inline fnObfuscatedStringGetValue obfuscatedStringGetValue;

    typedef void(__fastcall* fnObfuscatedStringSetValue)(void* instance, COMString* value);
    static inline fnObfuscatedStringSetValue obfuscatedStringSetValue;
public:
	static inline TypeExplorer RawGameBase;
	
	static void Initialize();
	static void* Instance();
    static OsuModes Mode();
    static HWND GetWindowHandle();
    static std::wstring GetClientHash();
    static void SetClientHash(const std::wstring& clientHash);
    static std::wstring GetUniqueID();
    static void SetUniqueID(const std::wstring& uniqueID);
    static std::wstring GetUniqueID2();
    static void SetUniqueID2(const std::wstring& uniqueID2);
    static std::wstring GetUniqueCheck();
    static void SetUniqueCheck(const std::wstring& uniqueCheck);
};