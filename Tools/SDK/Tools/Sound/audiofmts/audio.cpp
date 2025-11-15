#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "audio.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"


static AudioFile decode_flac(const char* path)
{
    AudioFile af;
    unsigned int channels;
    unsigned int samplerate;
    drflac_uint64 totalsamples;
    drflac_int32* samples = drflac_open_file_and_read_pcm_frames_s32(path, &channels, &samplerate, &totalsamples, NULL);
    if (samples == NULL)
    {
        af.fmt = AUDIOFMT_NONE;
        af.samples = NULL;
        return af;
    }
    af.fmt = AUDIOFMT_FLAC;
    af.channels = channels;
    af.samplerate = samplerate;
    af.depth = 4;
    af.totalsamples = totalsamples;
    af.samples = (uint8_t*)samples;
    af.length = ((double)totalsamples)/((double)(channels*samplerate));
    return af;
}

static AudioFile decode_wav(const char* path)
{
    AudioFile af;
    unsigned int channels;
    unsigned int samplerate;
    drwav_uint64 totalsamples;
    float* samples = drwav_open_file_and_read_pcm_frames_f32(path, &channels, &samplerate, &totalsamples, NULL);
    if (samples == NULL)
    {
        af.fmt = AUDIOFMT_NONE;
        af.samples = NULL;
        return af;
    }
    af.fmt = AUDIOFMT_WAV;
    af.channels = channels;
    af.samplerate = samplerate;
    af.depth = 4;
    af.totalsamples = totalsamples;
    af.samples = (uint8_t*)samples;
    af.length = ((double)totalsamples)/((double)(channels*samplerate));
    return af;
}

AudioFile audio_decodefile(const char* path)
{
    AudioFile nullsnd;
    char header[4];
    FILE* fp = fopen(path, "r+");
    nullsnd.fmt = AUDIOFMT_NONE;
    nullsnd.samples = NULL;
    if (fp == NULL)
        return nullsnd;
    fread(header, 4, 1, fp);
    fclose(fp);
    if (!strncmp(header, "RIFF", 4))
        return decode_wav(path);
    else if (!strncmp(header, "fLaC", 4))
        return decode_flac(path);
    // Checking an MP3 file is complex so let's just assume it is, and if it fails to load then it isn't an MP3 file
    return nullsnd;
}

uint32_t audio_getsampleindex(AudioFile* file, double time)
{
    return 0;
}

void audio_freedata(AudioFile* file)
{
    if (file->fmt != AUDIOFMT_NONE)
        free(file->samples);
}