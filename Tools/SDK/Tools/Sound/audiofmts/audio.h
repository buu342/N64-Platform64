#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>
#include <stdint.h>


/*=============================================================
                             Types
=============================================================*/

typedef enum {
    AUDIOFMT_NONE,
    AUDIOFMT_WAV,
    AUDIOFMT_FLAC,
    AUDIOFMT_MP3,
    //AUDIOFMT_OGG,
} AudioFormat;


/*=============================================================
                            Classes
=============================================================*/

class AudioFile
{
    private:

        bool Decode_WAV(wxFileName path);
        bool Decode_FLAC(wxFileName path);
        void Free();

    protected:

    public:
        AudioFormat m_Format;
        uint32_t m_SampleRate;
        uint8_t  m_Channels;
        uint8_t  m_ByteDepth;
        double   m_Length;
        uint64_t m_TotalSamples;
        uint8_t* m_SampleData;

        AudioFile();
        AudioFile(wxFileName path);
        ~AudioFile();

        bool DecodeFile(wxFileName path);
        bool IsOk();
        double GetSampleAtTime(uint64_t samplepos, int channel);

        AudioFile& operator=(const AudioFile & rhs);
};