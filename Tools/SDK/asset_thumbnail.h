#pragma once

#include <stdint.h>
#include <wx/bitmap.h>
#include <wx/icon.h>

class P64Asset_Thumbnail
{
    private:
        uint32_t              m_IconLargeDataCount;
        uint8_t*              m_IconLargeData;
        uint32_t              m_IconSmallDataCount;
        uint8_t*              m_IconSmallData;

    public:
        wxIcon                m_IconLarge;
        wxIcon                m_IconSmall;
        std::vector<uint8_t> Serialize();
        static P64Asset_Thumbnail* Deserialize(uint8_t* bytes);
        void    GenerateThumbnails(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in);
        void    GenerateThumbnails(wxImage img);
        bool    IsValidLarge();
        bool    IsValidSmall();

        P64Asset_Thumbnail& operator=(const P64Asset_Thumbnail& rhs);

        P64Asset_Thumbnail();
        ~P64Asset_Thumbnail();
};