#include "asset_thumbnail.h"
#include "../serializer.h"

#define THUMBSIZE_LARGE 64
#define THUMBSIZE_SMALL 16

P64Asset_Thumbnail::P64Asset_Thumbnail()
{
    this->m_IconLargeDataCount = 0;
    this->m_IconSmallDataCount = 0;
    this->m_IconLargeData = NULL;
    this->m_IconSmallData = NULL;
    this->m_IconLarge = wxIcon();
    this->m_IconSmall = wxIcon();
}

P64Asset_Thumbnail::~P64Asset_Thumbnail()
{
    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
}

P64Asset_Thumbnail& P64Asset_Thumbnail::operator=(const P64Asset_Thumbnail& rhs)
{
    if (this == &rhs)
        return *this;

    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    this->m_IconLargeData = NULL;
    this->m_IconLargeDataCount = 0;
    if (rhs.m_IconLargeDataCount > 0)
    {
        this->m_IconLargeData = (uint8_t*)malloc(rhs.m_IconLargeDataCount);
        if (this->m_IconLargeData != NULL)
        {
            memcpy(this->m_IconLargeData, rhs.m_IconLargeData, rhs.m_IconLargeDataCount);
            this->m_IconLargeDataCount = rhs.m_IconLargeDataCount;
        }
    }

    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
    this->m_IconSmallData = NULL;
    this->m_IconSmallDataCount = 0;
    if (rhs.m_IconSmallDataCount > 0)
    {
        this->m_IconSmallData = (uint8_t*)malloc(rhs.m_IconSmallDataCount);
        if (this->m_IconSmallData != NULL)
        {
            memcpy(this->m_IconSmallData, rhs.m_IconSmallData, rhs.m_IconSmallDataCount);
            this->m_IconSmallDataCount = rhs.m_IconSmallDataCount;
        }
    }

    this->m_IconLarge = rhs.m_IconLarge;
    this->m_IconSmall = rhs.m_IconSmall;
    return *this;
}

std::vector<uint8_t> P64Asset_Thumbnail::Serialize()
{
    std::vector<uint8_t> data;

    serialize_u32(&data, this->m_IconLargeDataCount);
    if (this->m_IconLargeDataCount > 0)
        serialize_buffer(&data, this->m_IconLargeData, this->m_IconLargeDataCount);
    serialize_u32(&data, this->m_IconSmallDataCount);
    if (this->m_IconSmallDataCount > 0)
        serialize_buffer(&data, this->m_IconSmallData, this->m_IconSmallDataCount);
    return data;
}

P64Asset_Thumbnail* P64Asset_Thumbnail::Deserialize(uint8_t* bytes)
{
    P64Asset_Thumbnail* asset = new P64Asset_Thumbnail();
    uint8_t* bytesptr = &bytes[0];
    uint32_t pos = 0;

    // Large thumbnail
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_IconLargeDataCount);
    if (asset->m_IconLargeDataCount != 0)
    {
        uint8_t* rgb = NULL;
        uint8_t* a = NULL;
        wxImage img;

        // Deseralize
        asset->m_IconLargeData = (uint8_t*)malloc(asset->m_IconLargeDataCount);
        if (asset->m_IconLargeData == NULL)
        {
            delete asset;
            return NULL;
        }
        pos = deserialize_buffer(bytesptr, pos, asset->m_IconLargeData, asset->m_IconLargeDataCount);

        // Copy the data into two buffers
        rgb = (uint8_t*)malloc(THUMBSIZE_LARGE*THUMBSIZE_LARGE*3);
        if (rgb == NULL)
        {
            delete asset;
            return NULL;
        }
        a = (uint8_t*)malloc(THUMBSIZE_LARGE*THUMBSIZE_LARGE*1);
        if (a == NULL)
        {
            free(rgb);
            delete asset;
            return NULL;
        }
        for (int i=0; i<THUMBSIZE_LARGE*THUMBSIZE_LARGE; i++)
        {
            rgb[(i*3) + 0] = asset->m_IconLargeData[(i*4) + 0];
            rgb[(i*3) + 1] = asset->m_IconLargeData[(i*4) + 1];
            rgb[(i*3) + 2] = asset->m_IconLargeData[(i*4) + 2];
            a[i]           = asset->m_IconLargeData[(i*4) + 3];
        }
        img = wxImage(THUMBSIZE_LARGE, THUMBSIZE_LARGE, rgb, a, false);
        asset->m_IconLarge.CopyFromBitmap(img);
    }
    else
    {
        asset->m_IconLargeData = NULL;
        asset->m_IconLarge = wxIcon();
    }

    // Small thumbnail
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_IconSmallDataCount);
    if (asset->m_IconSmallDataCount != 0)
    {
        uint8_t* rgb = NULL;
        uint8_t* a = NULL;
        wxImage img;

        asset->m_IconSmallData = (uint8_t*)malloc(asset->m_IconSmallDataCount);
        if (asset->m_IconSmallData == NULL)
        {
            delete asset;
            return NULL;
        }
        pos = deserialize_buffer(bytesptr, pos, asset->m_IconSmallData, asset->m_IconSmallDataCount);

        // Copy the data into two buffers
        rgb = (uint8_t*)malloc(THUMBSIZE_SMALL*THUMBSIZE_SMALL*3);
        if (rgb == NULL)
        {
            delete asset;
            return NULL;
        }
        a = (uint8_t*)malloc(THUMBSIZE_SMALL*THUMBSIZE_SMALL*1);
        if (a == NULL)
        {
            free(rgb);
            delete asset;
            return NULL;
        }
        for (int i=0; i<THUMBSIZE_SMALL*THUMBSIZE_SMALL; i++)
        {
            rgb[(i*3) + 0] = asset->m_IconSmallData[(i*4) + 0];
            rgb[(i*3) + 1] = asset->m_IconSmallData[(i*4) + 1];
            rgb[(i*3) + 2] = asset->m_IconSmallData[(i*4) + 2];
            a[i]           = asset->m_IconSmallData[(i*4) + 3];
        }
        img = wxImage(THUMBSIZE_SMALL, THUMBSIZE_SMALL, rgb, a, false);
        asset->m_IconSmall.CopyFromBitmap(img);
    }
    else
    {
        asset->m_IconSmallData = NULL;
        asset->m_IconSmall = wxIcon();
    }

    // Done
    return asset;
}

void P64Asset_Thumbnail::GenerateThumbnails(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in)
{
    wxImage temp;

    // Cleanup old stuff
    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
    this->m_IconLargeDataCount = 0;
    this->m_IconSmallDataCount = 0;
    this->m_IconLarge = wxIcon();
    this->m_IconSmall = wxIcon();

    // Alocate memory for the large icon
    this->m_IconLargeDataCount = THUMBSIZE_LARGE*THUMBSIZE_LARGE*4;
    this->m_IconLargeData = (uint8_t*)malloc(this->m_IconLargeDataCount);
    if (this->m_IconLargeData == NULL)
    {
        this->m_IconLargeDataCount = 0;
        return;
    }

    // Make the large icon and then store the data
    temp = wxImage(w_in, h_in, src, alphasrc, true);
    temp.Rescale(THUMBSIZE_LARGE, THUMBSIZE_LARGE);
    this->m_IconLarge.CopyFromBitmap(temp);
    for (uint32_t i=0; i<this->m_IconLargeDataCount/4; i++)
    {
        this->m_IconLargeData[(i*4) + 0] = temp.GetData()[(i*3) + 0];
        this->m_IconLargeData[(i*4) + 1] = temp.GetData()[(i*3) + 1];
        this->m_IconLargeData[(i*4) + 2] = temp.GetData()[(i*3) + 2];
        if (temp.GetAlpha() != NULL)
            this->m_IconLargeData[(i*4) + 3] = temp.GetAlpha()[i];
        else
            this->m_IconLargeData[(i*4) + 3] = 255;
    }

    // Alocate memory for the small icon
    this->m_IconSmallDataCount = THUMBSIZE_SMALL*THUMBSIZE_SMALL*4;
    this->m_IconSmallData = (uint8_t*)malloc(this->m_IconSmallDataCount);
    if (this->m_IconSmallData == NULL)
    {
        free(this->m_IconLargeData);
        this->m_IconLargeData = NULL;
        this->m_IconLargeDataCount = 0;
        this->m_IconSmallDataCount = 0;
        return;
    }

    // Make the small icon and then store the data
    temp = wxImage(w_in, h_in, src, alphasrc, true);
    temp.Rescale(THUMBSIZE_SMALL, THUMBSIZE_SMALL);
    this->m_IconSmall.CopyFromBitmap(temp);
    for (uint32_t i=0; i<this->m_IconSmallDataCount/4; i++)
    {
        this->m_IconSmallData[(i*4) + 0] = temp.GetData()[(i*3) + 0];
        this->m_IconSmallData[(i*4) + 1] = temp.GetData()[(i*3) + 1];
        this->m_IconSmallData[(i*4) + 2] = temp.GetData()[(i*3) + 2];
        if (temp.GetAlpha() != NULL)
            this->m_IconSmallData[(i*4) + 3] = temp.GetAlpha()[i];
        else
            this->m_IconSmallData[(i*4) + 3] = 255;
    }
}

void P64Asset_Thumbnail::GenerateThumbnails(wxImage img)
{
    if (img.IsOk())
        this->GenerateThumbnails(img.GetData(), img.GetAlpha(), img.GetWidth(), img.GetHeight());
}

bool P64Asset_Thumbnail::IsValidLarge()
{
    return this->m_IconLargeData != NULL;
}

bool P64Asset_Thumbnail::IsValidSmall()
{
    return this->m_IconSmallData != NULL;
}