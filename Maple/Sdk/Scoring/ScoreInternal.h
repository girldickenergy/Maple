#pragma once

#include "CLR/CLRList.h"
#include "CLR/CLRString.h"
#include "PlayModes.h"

struct [[gnu::packed]] ScoreInternal : CLRObject
{
    long long OnlineId;
    double TotalScoreDouble;
    double CurrentHp;
    CLRObject* EnabledMods;
    CLRString* FileChecksum;
    CLRObject* HpGraph;
    CLRString* PlayerName;
    CLRString* HpGraphString;
    CLRArray<uint8_t>* ReplayCompressed;
    CLRList<CLRObject*>* Replay;
    CLRList<int>* HitErrors;
    CLRList<int>* SpinningRates;
    CLRList<bool>* SectionResults;
    CLRList<CLRObject*>* Frames;
    CLRObject* User;
    CLRString* SubmissionResponseString;
    CLRObject* ExtraData;
    CLRObject* Processor;
    CLRObject* Beatmap;
    CLRObject* GotReplayData;
    CLRObject* SubmissionComplete;
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
    CLRObject* Date;
} __attribute__((packed));
