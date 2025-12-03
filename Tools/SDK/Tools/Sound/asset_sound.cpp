/***************************************************************
                        asset_sound.cpp

This class implements an sound asset file which can be
serialized and deserialized
***************************************************************/

#include "asset_sound.h"
#include "../../serializer.h"
#include <wx/msgdlg.h>


/*=============================================================
                            Macros
=============================================================*/

#define P64ASSET_HEADER "P64RAWSND"
#define P64ASSET_VERSION 1


/*=============================================================
               Sound Asset Class Implementation
=============================================================*/

/*==============================
    P64Asset_Sound (Constructor)
    Initializes the class
==============================*/

P64Asset_Sound::P64Asset_Sound()
{
    this->m_SourcePath = "";
    this->m_SampleRate = 44100;
    this->m_ForceMono = 1;
    this->m_Loop = 0;
    this->m_LoopStart = 0;
    this->m_LoopEnd = 0;
    this->m_Codebook_EntryCount = 2;
    this->m_Codebook_SampleSize = 16;
    this->m_ClusterIterations = 2;

    this->SetUpFileFormat({P64ASSET_HEADER, P64ASSET_VERSION}, {
        std::make_pair(DF_FILENAME, &this->m_SourcePath),
        std::make_pair(DF_U32, &this->m_SampleRate),
        std::make_pair(DF_U8, &this->m_ForceMono),
        std::make_pair(DF_U8, &this->m_Loop),
        std::make_pair(DF_U32, &this->m_LoopStart),
        std::make_pair(DF_U32, &this->m_LoopEnd),
        std::make_pair(DF_U8, &this->m_Codebook_EntryCount),
        std::make_pair(DF_U8, &this->m_Codebook_SampleSize),
        std::make_pair(DF_U8, &this->m_ClusterIterations),
    });
}


/*==============================
    P64Asset_Sound (Destructor)
    Cleans up the class before deletion
==============================*/

P64Asset_Sound::~P64Asset_Sound()
{

}


/*==============================
    P64Asset_Sound::operator=
    Assigns this object to another, making a deep copy of the
    data.
==============================*/

void P64Asset_Sound::operator=(const P64Asset_Sound& rhs)
{
    this->m_SourcePath = rhs.m_SourcePath;
    this->m_SampleRate = rhs.m_SampleRate;
    this->m_ForceMono = rhs.m_ForceMono;
    this->m_Loop = rhs.m_Loop;
    this->m_LoopStart = rhs.m_LoopStart;
    this->m_LoopEnd = rhs.m_LoopEnd;
    this->m_Codebook_EntryCount = rhs.m_Codebook_EntryCount;
    this->m_Codebook_SampleSize = rhs.m_Codebook_SampleSize;
    this->m_ClusterIterations = rhs.m_ClusterIterations;
    this->m_SndFile = rhs.m_SndFile;
    this->m_SoundPreview = rhs.m_SoundPreview;
    //this->m_N64SndFile = rhs.m_N64SndFile; // TODODODODODODODODODODODODODODODODODODODODODODODOD
}


/*==============================
    P64Asset_Sound::GenerateFinal
    Generate the VADPCM audio
    @param The path of the asset (so the absolute path for
           the source sound can be calculated)
==============================*/

void P64Asset_Sound::GenerateFinal(wxFileName assetpath)
{
    wxFileName srcsnd = this->m_SourcePath;

    // Get the source sound and decode it
    srcsnd.Normalize(wxPATH_NORM_ABSOLUTE | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, assetpath.GetPathWithSep());
    if (!wxFileExists(srcsnd.GetFullPath()) || !this->m_SndFile.DecodeFile(srcsnd.GetFullPath()))
    {
        this->m_SndFile = AudioFile();
        return;
    }
    
    // TODO
}


/*==============================
    P64Asset_Sound::IsOk
    Checks if the asset is properly initialized
    @return Whether the asset is properly initialized
==============================*/

bool P64Asset_Sound::IsOk()
{
    return this->m_SndFile.IsOk();
}