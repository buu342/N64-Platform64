#include <stdlib.h>
#include <string.h>
#include "audiofile.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"


/*=============================================================
               Audio File Class Implementation
=============================================================*/

/*==============================
    AudioFile (Constructor)
    Initializes the class
==============================*/

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


/*==============================
    AudioFile (Constructor)
    Initializes the class
    @param Path of the audio file to decode
==============================*/

AudioFile::AudioFile(wxFileName path) : AudioFile()
{
    this->DecodeFile(path);
}


/*==============================
    AudioFile (Destructor)
    Cleans up the class before deletion
==============================*/

AudioFile::~AudioFile()
{
    this->Free();
}


/*==============================
    AudioFile::DecodeFile
    Decodes an audio file. If the function fails, the
    audio data in this object is invalidated.
    @param  Path of the audio file to decode
    @return Returns true if an audio file decodes properly
            or false if not
==============================*/

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
    // TODO: Decode MP3
    // Checking an MP3 file is complex so let's just assume it is, and if it fails to load then it isn't an MP3 file
    return false;
}


/*==============================
    AudioFile::IsOk
    Checks if we have a valid audio file
    @return Returns true if an audio file decoded properly
            or false if not
==============================*/

bool AudioFile::IsOk()
{
    return this->m_Format != AUDIOFMT_NONE && this->m_SampleData != NULL && this->m_Channels > 0;
}


/*==============================
    AudioFile::GetSampleAtTime
    Gets the sample value (from -1 to 1) at a given channel's
    time
    @param  The sample index to get the frame from (should be
            between 0 and the total samples per channel)
    @param  The channel to get the sample from (channel 
            index must be larger or equal to 1)
    @return The sample value, between 0 and 1
==============================*/

double AudioFile::GetSampleAtTime(uint64_t samplepos, int channel)
{
    if (samplepos > this->m_TotalSamples || channel <= 0 || channel > this->m_Channels)
        return 0;

    uint64_t sampledatai = (samplepos*this->m_ByteDepth) + ((channel-1)*this->m_ByteDepth);

    switch (this->m_Format)
    {
        case AUDIOFMT_WAV:
        case AUDIOFMT_FLAC:
            return *(float*)(&this->m_SampleData[sampledatai]);
        default:
            return 0;
    }
}


/*==============================
    AudioFile::GetAvgMinMaxSampleAtTime
    Gets the average min and max sample value (from -1 to 1)
    at a given channel's window
    @param  The sample index to get the frame from (should be
            between 0 and the total samples per channel)
    @param  The amount of samples to count in this average
    @param  The channel to get the sample from (channel 
            index must be larger or equal to 1)
    @return The min and max average sample value, between 0
            and 1
==============================*/

std::pair<double, double> AudioFile::GetAvgMinMaxSampleAtTime(uint64_t samplepos, uint64_t samplecount, int channel)
{
    double av_min = 0;
    double av_max = 0;
    uint64_t c_max = 0;
    uint64_t c_min = 0;
    int64_t sc = this->m_TotalSamples - samplepos;

    // Validate the function arguments
    if (samplepos > this->m_TotalSamples || channel <= 0 || channel > this->m_Channels)
        return {0, 0};
    if (sc > ((int64_t)samplecount))
        sc = samplecount;

    // Iterate through the sample window
    for (int i=0; i<sc; i++)
    {
        uint64_t sampledatai = ((samplepos+i)*this->m_ByteDepth) + ((channel-1)*this->m_ByteDepth);
        float sample = 0;
        switch (this->m_Format)
        {
            case AUDIOFMT_WAV:
            case AUDIOFMT_FLAC:
                sample = *(float*)(&this->m_SampleData[sampledatai]);
                break;
            default:
                sample = 0;
                break;
        }

        // Count the max and min samples
        if (sample >= 0)
        {
            av_max += sample;
            c_max++;
        }
        else
        {
            av_min += sample;
            c_min++;
        }
    }

    // Calculate the average
    if (c_min > 0)
        av_min /= c_min;
    if (c_max > 0)
        av_max /= c_max;
    return {av_min, av_max};
}


/*==============================
    AudioFile::Decode_WAV
    Decode a WAV file at a given path
    @return Returns true if an audio file decoded properly
            or false if not
==============================*/

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
    this->m_Length = ((double)totalsamples)/((double)(samplerate));
    return true;
}


/*==============================
    AudioFile::Decode_FLAC
    Decode a FLAC file at a given path
    @return Returns true if an audio file decoded properly
            or false if not
==============================*/

bool AudioFile::Decode_FLAC(wxFileName path)
{
    unsigned int channels;
    unsigned int samplerate;
    drflac_uint64 totalsamples;
    float* samples = drflac_open_file_and_read_pcm_frames_f32((const char*)path.GetFullPath().c_str(), &channels, &samplerate, &totalsamples, NULL);
    if (samples == NULL)
        return false;
    this->m_Format = AUDIOFMT_FLAC;
    this->m_Channels = channels;
    this->m_SampleRate = samplerate;
    this->m_ByteDepth = 4;
    this->m_TotalSamples = totalsamples;
    this->m_SampleData = (uint8_t*)samples;
    this->m_Length = ((double)totalsamples)/((double)(samplerate));
    return true;
}


/*==============================
    AudioFile::Free
    Frees the data used by this audio file and invalidates it.
==============================*/

void AudioFile::Free()
{
    if (this->m_SampleData != NULL)
        free(this->m_SampleData);
    this->m_SampleData = NULL;
    this->m_Format = AUDIOFMT_NONE;
}


/*==============================
    AudioFile::operator=
    Handles the assignment operator between two AudioFiles.
    Performs a deep copy of the sample data.
==============================*/

AudioFile& AudioFile::operator=(const AudioFile & rhs)
{
    this->m_SampleRate = rhs.m_SampleRate;
    this->m_Channels = rhs.m_Channels;
    this->m_ByteDepth = rhs.m_ByteDepth;
    this->m_Length = rhs.m_Length;
    this->m_TotalSamples = rhs.m_TotalSamples;
    if (rhs.m_SampleData != NULL)
    {
        uint8_t* bytes = (uint8_t*)malloc(rhs.m_TotalSamples*rhs.m_ByteDepth*rhs.m_Channels);
        memcpy(bytes, rhs.m_SampleData, rhs.m_TotalSamples*rhs.m_ByteDepth*rhs.m_Channels);
        this->Free();
        this->m_SampleData = bytes;
    }
    else
        this->m_SampleData = NULL;
    this->m_Format = rhs.m_Format;
    return *this;
}