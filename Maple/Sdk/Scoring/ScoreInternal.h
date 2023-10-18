#pragma once

#include "CLR/CLRString.h"
#include "PlayModes.h"

#include <cstdint>

struct ScoreInternal
{
    uintptr_t VTable;
    long OnlineId;
    double TotalScoreDouble;
    double CurrentHp;
    uintptr_t EnabledMods;
    CLRString* FileChecksum;
    uintptr_t HpGraph;
    CLRString* PlayerName;
    CLRString* HpGraphString;
    uintptr_t ReplayCompressed;
    uintptr_t Replay;
    uintptr_t HitErrors;
    uintptr_t SpinningRates;
    uintptr_t SectionResults;
    uintptr_t Frames;
    uintptr_t User;
    CLRString* SubmissionResponseString;
    uintptr_t ExtraData;
    uintptr_t Processor;
    uintptr_t Beatmap;
    uintptr_t GotReplayData;
    uintptr_t SubmissionComplete;
    PlayModes PlayMode;
    int MaxCombo;
    int OnlineRank;
    int FailTime;
    int StartTime;
    int TotalScore;
    int Version;
    int Seed;
    int ScoreSubmissionStatus;
    unsigned short Count100;
    unsigned short Count300;
    unsigned short Count50;
    unsigned short CountGeki;
    unsigned short CountKatu;
    unsigned short CountMiss;
    unsigned short CurrentCombo;
    bool IsOnline;
    bool Pass;
    bool Exit;
    bool Perfect;
    bool AllowSubmission;
    bool HadStoryboard;
    bool HasOnlineReplay;
    uintptr_t Date;
};
