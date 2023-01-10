#include "ReplayDecoder.h"

#include "Osu/Mods.h"
#include "Osu/Keys/CatchTheBeatKeys.h"
#include "Osu/Keys/OsuKeys.h"
#include "Osu/Keys/OsuManiaKeys.h"
#include "Osu/Keys/TaikoKeys.h"
#include "Osu/PlayModes.h"

#include "../Serialization/SerializationReader.h"
#include "../Utilities/ChiyoStringUtilities.h"
#include "../LZMA/lzma_helper.hpp"

Replay ReplayDecoder::Decode(std::string filepath)
{
	std::ifstream filestream(filepath, std::ios::binary);

	return Decode(&filestream);
}

Replay ReplayDecoder::Decode(std::istream* filestream)
{
    Replay replay = Replay();

	SerializationReader reader = SerializationReader(filestream);

    replay.PlayMode = (PlayModes)(int)reader.ReadByte();
    replay.Version = reader.ReadInt32();
    replay.BeatmapHash = reader.ReadString();
    replay.PlayerName = reader.ReadString();
    replay.ReplayHash = reader.ReadString();
    replay.Count300 = reader.ReadUInt16();
    replay.Count100 = reader.ReadUInt16();
    replay.Count50 = reader.ReadUInt16();
    replay.CountGeki = reader.ReadUInt16();
    replay.CountKatu = reader.ReadUInt16();
    replay.CountMiss = reader.ReadUInt16();
    replay.TotalScore = reader.ReadInt32();
    replay.MaxCombo = reader.ReadUInt16();
    replay.Perfect = reader.ReadBoolean();
    replay.Mods = (Mods)reader.ReadInt32();

    std::string lifeData = reader.ReadString();
    if (!lifeData.empty())
    {
        for (const std::string& lifeBlock : ChiyoStringUtilities::Split(lifeData, ","))
        {
            std::vector<std::string> tokens = ChiyoStringUtilities::Split(lifeBlock, "|");
            if (tokens.size() < 2)
                continue;

            LifeFrame lifeFrame = LifeFrame();
            lifeFrame.Time = std::stoi(tokens[0]);
            lifeFrame.Percentage = std::stof(tokens[1]);

            replay.LifeFrames.push_back(lifeFrame);
        }
    }

    replay.ReplayTimestamp = reader.ReadInt64();
    std::vector<unsigned char> compressedReplayFrames = reader.ReadByteArray();

    if (compressedReplayFrames.size() > 0)
    {
        int lastTime = 0;
        for (const std::string& replayBlock : ChiyoStringUtilities::Split(ChiyoStringUtilities::ByteArrayToString(lh::lzma_decompress(compressedReplayFrames)), ","))
        {
            if (replayBlock.empty())
                continue;

            std::vector<std::string> tokens = ChiyoStringUtilities::Split(replayBlock, "|");

            if (tokens.size() < 4)
                continue;

            if (tokens[0] == "-12345")
            {
                replay.Seed = std::stoi(tokens[3]);

                continue;
            }

            ReplayFrame replayFrame = ReplayFrame();
            replayFrame.TimeDiff = std::stoi(tokens[0]);
            replayFrame.Time = replayFrame.TimeDiff + lastTime;
            replayFrame.X = std::stof(tokens[1]);
            replayFrame.Y = std::stof(tokens[2]);

            switch (replay.PlayMode)
            {
                case PlayModes::Osu:
                    replayFrame.OsuKeys = (OsuKeys)std::stoi(tokens[3]);
                    break;
                case PlayModes::Taiko:
                    replayFrame.TaikoKeys = (TaikoKeys)std::stoi(tokens[3]);
                    break;
                case PlayModes::CatchTheBeat:
                    replayFrame.CatchTheBeatKeys = (CatchTheBeatKeys)std::stoi(tokens[3]);
                    break;
                case PlayModes::OsuMania:
                    replayFrame.OsuManiaKeys = (OsuManiaKeys)std::stoi(tokens[3]);
                    break;
            }

            lastTime = replayFrame.Time;

            replay.ReplayFrames.push_back(replayFrame);
        }
    }

    if (replay.Version >= 20140721)
        replay.OnlineId = reader.ReadInt64();
    else if (replay.Version >= 20121008)
        replay.OnlineId = (long)reader.ReadInt32();

    replay.ReplayLength = replay.ReplayFrames.back().Time;

	return replay;
}
