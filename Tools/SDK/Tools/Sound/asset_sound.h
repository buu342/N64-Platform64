#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>
#include <stdint.h>
#include <vector>


/*=============================================================
                            Classes
=============================================================*/

class P64Asset_Sound
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

        std::vector<uint8_t>   Serialize();
        static P64Asset_Sound* Deserialize(std::vector<uint8_t> bytes);

        P64Asset_Sound();
        ~P64Asset_Sound();
};
