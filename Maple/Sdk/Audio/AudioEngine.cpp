#include "AudioEngine.h"

#include <Vanilla.h>

void AudioEngine::Initialize()
{
    RawAudioEngine = Vanilla::Explorer["osu.Audio.AudioEngine"];
    RawAudioTrack = Vanilla::Explorer["osu.Audio.AudioTrack"];
    RawAudioTrackBass = Vanilla::Explorer["osu.Audio.AudioTrackBass"];
    RawAudioTrackVirtual = Vanilla::Explorer["osu.Audio.AudioTrackVirtual"];

    trackBassPlaybackRate = RawAudioTrackBass["playbackRate"].Field;
    trackVirtualPlaybackRate = RawAudioTrackVirtual["playbackRate"].Field;
    audioStreamField = RawAudioTrack["<audioStream>k__BackingField"].Field;
    frequencyLockField = RawAudioTrack["<FrequencyLock>k__BackingField"].Field;

    currentTrackInstanceAddress = RawAudioEngine["AudioTrack"].Field.GetAddress();
    initialFrequencyAddress = RawAudioEngine["InitialFrequency"].Field.GetAddress();
    nightcoreAddress = RawAudioEngine["Nightcore"].Field.GetAddress();
    lastAudioTimeAccurateSetAddress = RawAudioEngine["lastAudioTimeAccurateSet"].Field.GetAddress();
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
    void* trackInstance = CurrentTrackInstance();
    if (!trackInstance)
        return 100.;

    if (TrackHandle() == 0)
        return *static_cast<double*>(trackVirtualPlaybackRate.GetAddress(trackInstance));

	return *static_cast<double*>(trackBassPlaybackRate.GetAddress(trackInstance));
}

void AudioEngine::SetPlaybackRate(double rate)
{
    void* trackInstance = CurrentTrackInstance();
    if (!trackInstance)
        return;

    if (rate < 1.)
        rate = 1.;
	
    *static_cast<int*>(lastAudioTimeAccurateSetAddress) = 0;
    *static_cast<bool*>(frequencyLockField.GetAddress(trackInstance)) = !Nightcore();
    if (round(GetPlaybackRate()) != round(rate))
    {
        if (TrackHandle() == 0)
        {
            *static_cast<double*>(trackVirtualPlaybackRate.GetAddress(trackInstance)) = rate;
        }
        else
        {
            *static_cast<double*>(trackBassPlaybackRate.GetAddress(trackInstance)) = rate;
        	if (Nightcore())
        	{
                channelSetAttribute(TrackHandle(), 1, static_cast<float>(static_cast<double>(InitialFrequency()) * rate / 100.0));
                channelSetAttribute(TrackHandle(), 65536, 0);
        	}
            else
            {
                channelSetAttribute(TrackHandle(), 1, InitialFrequency());
                channelSetAttribute(TrackHandle(), 65536, static_cast<float>(rate - 100.0));
            }
        }
    }
}
