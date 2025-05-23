/***************************************************************
                          audio.c
                               
Handles audio
***************************************************************/

#include <ultra64.h>
#include <n_libaudio.h>
#include <math.h>
#include "debug.h"
#include "osconfig.h"
#include "audio.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_audio(void *arg);


/*********************************
             Globals
*********************************/

static OSThread s_threadstruct_audio;
static s16*     s_audio_data;


/*==============================
    audio_initialize
    Launches the audio thread
==============================*/

void audio_initialize()
{
    // Start the audio thread
    osCreateThread(&s_threadstruct_audio, THREADID_AUDIO, threadfunc_audio, NULL, STACKREALSTART_AUDIO, THREADPRI_AUDIO);
    osStartThread(&s_threadstruct_audio);
}


/*==============================
    thread_audio
    The audio thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_audio(void *arg)
{
    u32 i;
    u32 l_wave_index = 0;
    const u32 c_duration = 1;
    const u32 c_samplecount = c_duration * AUDIO_FREQUENCY*2;
    const u32 c_buffersize  = c_samplecount*sizeof(u16);
    debug_printf("Audio Thread: Started\n");
    if (c_buffersize > 262144)
        debug_printf("Audio Thread: WAARNING: Audio buffer too big. %d / 262144 bytes\n", c_buffersize);
    
    // Initialize the AI
    osAiSetFrequency(AUDIO_FREQUENCY);

    // Initialize the audio data with a 440hz sine wave
    debug_printf("Audio Thread: Initializing audio data\n");
    s_audio_data = (u16*)HEAP_AUDIO_START;
    for (i=0; i<c_samplecount; i+=2)
    {
        const u32 c_frequency = 440;
        const f32 c_sample = sinf(2*M_PI * c_frequency * (((double)l_wave_index)/((double)AUDIO_FREQUENCY)));
        const s16 c_volume = 0x7FFF;
        s_audio_data[i+0] = (s16)(c_sample*c_volume); // Left channel
        s_audio_data[i+1] = (s16)(c_sample*c_volume); // Right channel
        l_wave_index++;
    }
    osWritebackDCache(s_audio_data, c_buffersize);
    
    // Set the next buffer for the AI to use
    debug_printf("Audio Thread: Sending audio data to DAC\n");
    osAiSetNextBuffer(s_audio_data, c_buffersize);
    
    // Spin this thread forever
    while (1)
    {
        debug_printf("Audio Thread: Loop start.\n");
        
        debug_printf("Audio Thread: Loop end, stopping thread\n");
        osStopThread(&s_threadstruct_audio);
    }
}


/*==============================
    audio_stopthread
    Stops the audio thread
==============================*/

void audio_stopthread()
{
    osStopThread(&s_threadstruct_audio);
}