#include <stdlib.h>
#include <stdint.h>
#include "audio.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include "dr_mp3.h"
#include "dr_wav.h"


static AudioFile* decode_flac(char* path)
{
    AudioFile* af;
    unsigned int channels;
    unsigned int sampleRate;
    drflac_uint64 totalPCMFrameCount;
    drflac_int32* pSampleData = drflac_open_file_and_read_pcm_frames_s32(path, &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL)
        return NULL;
    af = (AudioFile*)malloc(sizeof(AudioFile));
    af->fmt = AUDIOFMT_FLAC;
    af->channels = channels;
    af->samplerate = sampleRate;
    af->samples = (uint8_t*)pSampleData;
    af->length = ((double)totalPCMFrameCount)/((double)(channels*sampleRate));
    af->depth = 4;
    return af;
}

AudioFile* audio_decodefile(char* path)
{
    return NULL;
}

uint32_t audio_getsampleindex(AudioFile* file, double time)
{
    return 0;
}

void audio_destroyfile(AudioFile* file)
{
    free(file->samples);
    free(file);
}