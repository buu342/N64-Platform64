#ifndef _LIB_AUDIO_FMTS_
#define _LIB_AUDIO_FMTS_

    typedef enum {
        AUDIOFMT_WAV,
        AUDIOFMT_FLAC,
        AUDIOFMT_MP3,
        //AUDIOFMT_OGG,
    } AudioFormat;

    typedef struct {
        AudioFormat fmt;
        uint32_t samplerate;
        double   length;
        uint8_t  channels;
        uint8_t  depth;
        double   totalsamples;
        uint8_t* samples;
    } AudioFile;

    AudioFile* audio_decodefile(char* path);
    uint32_t audio_getsampleindex(AudioFile* file, double time);
    void audio_destroyfile(AudioFile* file);

#endif