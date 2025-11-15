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
}


/*==============================
    P64Asset_Sound (Destructor)
    Cleans up the class before deletion
==============================*/

P64Asset_Sound::~P64Asset_Sound()
{
    audio_freedata(&this->m_SndFile); 
}


/*==============================
    P64Asset_Sound::Serialize
    Serializes the object
    @return The serialized data
==============================*/

std::vector<uint8_t> P64Asset_Sound::Serialize()
{
    std::vector<uint8_t> data;
    serialize_header(&data, P64ASSET_HEADER, P64ASSET_VERSION);
    serialize_wxstring(&data, this->m_SourcePath.GetFullPath());
    serialize_u32(&data, this->m_SampleRate);
    serialize_u8(&data, this->m_ForceMono);
    serialize_u8(&data, this->m_Loop);
    serialize_u32(&data, this->m_LoopStart);
    serialize_u32(&data, this->m_LoopEnd);
    return data;
}


/*==============================
    P64Asset_Sound::Deserialize
    Serializes the object
    @param  The serialized data
    @return The deserialized object or NULL
==============================*/

P64Asset_Sound* P64Asset_Sound::Deserialize(std::vector<uint8_t> bytes)
{
    P64Asset_Sound* asset;
    wxString temp_str;
    uint32_t temp_32;
    char header[16];
    uint8_t* bytesptr = &bytes[0];
    uint32_t pos = 0;

    // Check if the file at least contains a header
    if (bytes.size() < 16)
    {
        wxMessageDialog dialog(NULL, "File is not a P64 sound", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }

    // Validate the header
    pos = deserialize_header(bytesptr, pos, header, &temp_32);
    if (strcmp(header, P64ASSET_HEADER) != 0)
    {
        wxMessageDialog dialog(NULL, "File is not a P64 sound", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }
    if (temp_32 > P64ASSET_VERSION)
    {
        wxMessageDialog dialog(NULL, "File is a more recent, unsupported version", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }

    // Read the sound configuration data
    asset = new P64Asset_Sound();
    pos = deserialize_wxstring(bytesptr, pos, &temp_str);
    asset->m_SourcePath = wxFileName(temp_str);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_SampleRate);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_ForceMono);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_Loop);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_LoopStart);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_LoopEnd);
    return asset;
}