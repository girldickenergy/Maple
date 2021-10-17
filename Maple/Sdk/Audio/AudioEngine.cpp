#include "AudioEngine.h"

#include <Vanilla.h>

void AudioEngine::Initialize()
{
    RawAudioEngine = Vanilla::Explorer["osu.Audio.AudioEngine"];

    RawAudioEngine["set_CurrentPlaybackRate"].Method.Compile();
    setCurrentPlaybackRate = static_cast<fnSetCurrentPlaybackRate>(RawAudioEngine["set_CurrentPlaybackRate"].Method.GetNativeStart());

    trackBassPlaybackRate = Vanilla::Explorer["osu.Audio.AudioTrackBass"]["playbackRate"].Field;
    trackVirtualPlaybackRate = Vanilla::Explorer["osu.Audio.AudioTrackVirtual"]["playbackRate"].Field;
    audioStreamField = Vanilla::Explorer["osu.Audio.AudioTrack"]["<audioStream>k__BackingField"].Field;

    RawAudioEngine["set_CurrentPlaybackRate"].Method.Compile();

    currentTrackInstanceAddress = RawAudioEngine["AudioTrack"].Field.GetAddress();
    initialFrequencyAddress = RawAudioEngine["InitialFrequency"].Field.GetAddress();
    nightcoreAddress = RawAudioEngine["Nightcore"].Field.GetAddress();
}

void* AudioEngine::CurrentTrackInstance()
{
    return *static_cast<void**>(currentTrackInstanceAddress);
}

int AudioEngine::TrackHandle()
{
    return *static_cast<int*>(audioStreamField.GetAddress(CurrentTrackInstance()));
}

float AudioEngine::InitialFrequency()
{
    return *static_cast<float*>(initialFrequencyAddress);
}

bool AudioEngine::Nightcore()
{
    return *static_cast<bool*>(nightcoreAddress);
}

double AudioEngine::GetPlaybackRate()
{
    void* instance = CurrentTrackInstance();
    if (!instance)
        return 100.;

    if (TrackHandle() == 0)
        return *static_cast<double*>(trackVirtualPlaybackRate.GetAddress(instance));

	return *static_cast<double*>(trackBassPlaybackRate.GetAddress(instance));
}

void AudioEngine::SetPlaybackRate(double rate)
{
	//PLEASE SEND HELP I CAN'T FIGURE OUT WHY IT CRASHES
    setCurrentPlaybackRate(rate);

    //std::vector<variant_t> args;
    //args.push_back(variant_t(rate));

    //RawAudioEngine["set_CurrentPlaybackRate"].Method.InvokeUnsafe(variant_t(), args);
}
