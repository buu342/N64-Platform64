#ifndef _LIB_AUDIO_FMTS_
#define _LIB_AUDIO_FMTS_

    typedef enum {
        AUDIOFMT_NONE,
        AUDIOFMT_WAV,
        AUDIOFMT_FLAC,
        AUDIOFMT_MP3,
        //AUDIOFMT_OGG,
    } AudioFormat;

    typedef struct {
        AudioFormat fmt;
        uint32_t samplerate;
        uint8_t  channels;
        uint8_t  depth;
        double   length;
        uint64_t totalsamples;
        uint8_t* samples;
    } AudioFile;

    AudioFile audio_decodefile(const char* path);
    uint32_t audio_getsampleindex(AudioFile* file, double time);
    void audio_freedata(AudioFile* file);

#endif