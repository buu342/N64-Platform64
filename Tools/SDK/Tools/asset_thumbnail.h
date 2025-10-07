#pragma once

#include <stdint.h>
#include <wx/bitmap.h>
#include <wx/icon.h>


/*=============================================================
                            Classes
=============================================================*/

// A class representing small/large thumbnails for the search panel
class P64Asset_Thumbnail
{
    private:
        wxSize    m_IconLargeSize;
        uint8_t*  m_IconLargeData;
        wxSize    m_IconSmallSize;
        uint8_t*  m_IconSmallData;

    public:
        wxIcon  m_IconLarge;
        wxIcon  m_IconSmall;

        std::vector<uint8_t>        Serialize();
        static P64Asset_Thumbnail*  Deserialize(uint8_t* bytes);

        void  GenerateThumbnails(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in);
        void  GenerateThumbnails(wxImage img);
        bool  IsValidLarge();
        bool  IsValidSmall();

        P64Asset_Thumbnail& operator=(const P64Asset_Thumbnail& rhs);

        P64Asset_Thumbnail();
        ~P64Asset_Thumbnail();
};