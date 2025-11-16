#include <stdlib.h>
#include <string.h>
#include "audio.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

/*=============================================================
               Audio File Class Implementation
=============================================================*/

AudioFile::AudioFile()
{
    this->m_Format = AUDIOFMT_NONE;
    this->m_SampleRate = 0;
    this->m_Channels = 0;
    this->m_ByteDepth = 0;
    this->m_Length = 0;
    this->m_TotalSamples = 0;
    this->m_SampleData = NULL;
}

AudioFile::AudioFile(wxFileName path) : AudioFile()
{
    this->DecodeFile(path);
}

AudioFile::~AudioFile()
{
    this->Free();
}

bool AudioFile::DecodeFile(wxFileName path)
{
    char header[4];
    FILE* fp = fopen((const char*)path.GetFullPath().c_str(), "r+");
    this->Free();
    if (fp == NULL)
        return false;
    fread(header, 4, 1, fp);
    fclose(fp);
    if (!strncmp(header, "RIFF", 4))
        return this->Decode_WAV(path);
    else if (!strncmp(header, "fLaC", 4))
        return this->Decode_FLAC(path);
    // Checking an MP3 file is complex so let's just assume it is, and if it fails to load then it isn't an MP3 file
    return false;
}

bool AudioFile::IsOk()
{
    return this->m_Format != AUDIOFMT_NONE && this->m_SampleData != NULL;
}

bool AudioFile::Decode_WAV(wxFileName path)
{
    unsigned int channels;
    unsigned int samplerate;
    drwav_uint64 totalsamples;
    float* samples = drwav_open_file_and_read_pcm_frames_f32((const char*)path.GetFullPath().c_str(), &channels, &samplerate, &totalsamples, NULL);
    if (samples == NULL)
        return false;
    this->m_Format = AUDIOFMT_WAV;
    this->m_Channels = channels;
    this->m_SampleRate = samplerate;
    this->m_ByteDepth = 4;
    this->m_TotalSamples = totalsamples;
    this->m_SampleData = (uint8_t*)samples;
    this->m_Length = ((double)totalsamples)/((double)(channels*samplerate));
    return true;
}

bool AudioFile::Decode_FLAC(wxFileName path)
{
    unsigned int channels;
    unsigned int samplerate;
    drflac_uint64 totalsamples;
    drflac_int32* samples = drflac_open_file_and_read_pcm_frames_s32((const char*)path.GetFullPath().c_str(), &channels, &samplerate, &totalsamples, NULL);
    if (samples == NULL)
        return false;
    this->m_Format = AUDIOFMT_FLAC;
    this->m_Channels = channels;
    this->m_SampleRate = samplerate;
    this->m_ByteDepth = 4;
    this->m_TotalSamples = totalsamples;
    this->m_SampleData = (uint8_t*)samples;
    this->m_Length = ((double)totalsamples)/((double)(channels*samplerate));
    return true;
}

void AudioFile::Free()
{
    if (this->m_SampleData != NULL)
        free(this->m_SampleData);
    this->m_SampleData = NULL;
    this->m_Format = AUDIOFMT_NONE;
}

AudioFile AudioFile::operator=(const AudioFile & rhs)
{
    this->m_Format = rhs.m_Format;
    this->m_SampleRate = rhs.m_SampleRate;
    this->m_Channels = rhs.m_Channels;
    this->m_ByteDepth = rhs.m_ByteDepth;
    this->m_Length = rhs.m_Length;
    this->m_TotalSamples = rhs.m_TotalSamples;
    if (rhs.m_SampleData != NULL)
    {
        uint8_t* bytes = (uint8_t*)malloc(rhs.m_TotalSamples);
        memcpy(bytes, rhs.m_SampleData, rhs.m_TotalSamples);
        this->m_SampleData = bytes;
    }
    else
        this->m_SampleData = NULL;
    return *this;
}
