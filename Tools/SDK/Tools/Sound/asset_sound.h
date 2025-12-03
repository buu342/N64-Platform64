#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>
#include <stdint.h>
#include <vector>
#include <wx/sound.h>
#include "audiofmts/audiofile.h"
#include "../asset.h"


/*=============================================================
                            Classes
=============================================================*/

class P64Asset_Sound : public P64Asset
{
    private:

    protected:

    public:
        wxFileName  m_SourcePath;
        uint32_t    m_SampleRate;
        uint8_t     m_ForceMono;
        uint8_t     m_Loop;
        uint32_t    m_LoopStart;
        uint32_t    m_LoopEnd;
        uint8_t     m_Codebook_EntryCount;
        uint8_t     m_Codebook_SampleSize;
        uint8_t     m_ClusterIterations;

        AudioFile m_SndFile;
        //VADPCM  m_N64SndFile;
        wxSound   m_SoundPreview;

        bool IsOk();
        void GenerateFinal(wxFileName assetpath);

        void operator=(const P64Asset_Sound& rhs);

        P64Asset_Sound();
        ~P64Asset_Sound();
};
