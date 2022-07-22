/***************************************************************
                          audio.c
                               
Handles audio
***************************************************************/

#include <ultra64.h>
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

static OSThread	s_threadstruct_audio;


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
    debug_printf("Created audio thread\n");
    
    // Spin this thread forever
    while (1)
    {
        debug_printf("Audio thread loop start\n");
        
        debug_printf("Stopping audio thread\n");
        osStopThread(&s_threadstruct_audio);
    }
}