#pragma once

#include <stdint.h>
#include <wx/bitmap.h>
#include <wx/icon.h>


/*=============================================================
                            Classes
=============================================================*/

// A class representing small/large thumbnails for the search panel
class P64AssetThumbnail
{
    private:
        wxSize    m_IconLargeSize;
        uint8_t*  m_IconLargeData;
        wxIcon    m_IconLarge;
        wxSize    m_IconSmallSize;
        uint8_t*  m_IconSmallData;
        wxIcon    m_IconSmall;

    protected:

    public:
        std::vector<uint8_t>        Serialize();
        static P64AssetThumbnail*   Deserialize(uint8_t** bytes);

        void  GenerateThumbnails(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in);
        void  GenerateThumbnails(wxImage img);
        bool  IsValidLarge();
        bool  IsValidSmall();

        wxIcon GetSmallIcon();
        wxIcon GetLargeIcon();

        P64AssetThumbnail& operator=(const P64AssetThumbnail& rhs);

        P64AssetThumbnail();
        ~P64AssetThumbnail();
};