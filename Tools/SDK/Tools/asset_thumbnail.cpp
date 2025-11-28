/***************************************************************
                      asset_thumbnail.cpp

This class provides a way to read/write thumbnail data into
binary files.
***************************************************************/

#include "asset_thumbnail.h"
#include "../serializer.h"


/*=============================================================
                            Macros
=============================================================*/

#define THUMBSIZE_LARGE 64
#define THUMBSIZE_SMALL 16


/*==============================
    P64AssetThumbnail (Constructor)
    Initializes the class
==============================*/

P64AssetThumbnail::P64AssetThumbnail()
{
    this->m_IconLargeSize = wxSize(0, 0);
    this->m_IconSmallSize = wxSize(0, 0);
    this->m_IconLargeData = NULL;
    this->m_IconSmallData = NULL;
    this->m_IconLarge = wxIcon();
    this->m_IconSmall = wxIcon();
}


/*==============================
    P64AssetThumbnail (Destructor)
    Cleans up the class before deletion
==============================*/

P64AssetThumbnail::~P64AssetThumbnail()
{
    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
}


/*==============================
    P64AssetThumbnail::operator=
    Assigns this object to another, making a deep copy of its
    data.
==============================*/

P64AssetThumbnail& P64AssetThumbnail::operator=(const P64AssetThumbnail& rhs)
{
    uint32_t datasize;
    if (this == &rhs)
        return *this;

    // Free old large icon data
    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    this->m_IconLargeData = NULL;
    this->m_IconLargeSize = wxSize(0, 0);

    // Make a copy of the large icon
    datasize = rhs.m_IconLargeSize.x*rhs.m_IconLargeSize.y*4;
    if (datasize != 0)
    {
        this->m_IconLargeData = (uint8_t*)malloc(datasize);
        if (this->m_IconLargeData != NULL)
        {
            memcpy(this->m_IconLargeData, rhs.m_IconLargeData, datasize);
            this->m_IconLargeSize = rhs.m_IconLargeSize;
        }
    }

    // Free old small icon data
    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
    this->m_IconSmallData = NULL;
    this->m_IconSmallSize = wxSize(0, 0);

    // Make a copy of the small icon
    datasize = rhs.m_IconSmallSize.x*rhs.m_IconSmallSize.y*4;
    if (datasize != 0)
    {
        this->m_IconSmallData = (uint8_t*)malloc(datasize);
        if (this->m_IconSmallData != NULL)
        {
            memcpy(this->m_IconSmallData, rhs.m_IconSmallData, datasize);
            this->m_IconSmallSize = rhs.m_IconSmallSize;
        }
    }

    // Assign the rest of the attributes
    this->m_IconLarge = rhs.m_IconLarge;
    this->m_IconSmall = rhs.m_IconSmall;
    return *this;
}


/*==============================
    P64AssetThumbnail::Serialize
    Serializes the object
    @return The seralized data
==============================*/

std::vector<uint8_t> P64AssetThumbnail::Serialize()
{
    std::vector<uint8_t> data;
    uint32_t datasize;

    serialize_u32(&data, this->m_IconLargeSize.x);
    serialize_u32(&data, this->m_IconLargeSize.y);
    datasize = this->m_IconLargeSize.x* this->m_IconLargeSize.y*4;
    if (datasize > 0)
        serialize_buffer(&data, this->m_IconLargeData, datasize);
    
    serialize_u32(&data, this->m_IconSmallSize.x);
    serialize_u32(&data, this->m_IconSmallSize.y);
    datasize = this->m_IconSmallSize.x* this->m_IconSmallSize.y*4;
    if (datasize > 0)
        serialize_buffer(&data, this->m_IconSmallData, datasize);
    return data;
}


/*==============================
    P64AssetThumbnail::Deserialize
    Deserializes the object from a byte array
    @param  The byte array containing the object data
    @return The seralized data
==============================*/

P64AssetThumbnail* P64AssetThumbnail::Deserialize(uint8_t** bytes)
{
    uint32_t datasize;
    P64AssetThumbnail* asset = new P64AssetThumbnail();
    uint8_t* bytesptr = *bytes;

    // Large thumbnail
    deserialize_u32(&bytesptr, (uint32_t*)&asset->m_IconLargeSize.x);
    deserialize_u32(&bytesptr, (uint32_t*)&asset->m_IconLargeSize.y);
    datasize = asset->m_IconLargeSize.x*asset->m_IconLargeSize.y*4;
    if (datasize != 0)
    {
        uint8_t* rgb = NULL;
        uint8_t* a = NULL;
        wxImage img;

        // Deserialize
        asset->m_IconLargeData = (uint8_t*)malloc(datasize);
        if (asset->m_IconLargeData == NULL)
        {
            delete asset;
            return NULL;
        }
        deserialize_buffer(&bytesptr, asset->m_IconLargeData, datasize);

        // Copy the data into two buffers
        rgb = (uint8_t*)malloc(asset->m_IconLargeSize.x*asset->m_IconLargeSize.y*3);
        if (rgb == NULL)
        {
            delete asset;
            return NULL;
        }
        a = (uint8_t*)malloc(asset->m_IconLargeSize.x*asset->m_IconLargeSize.y*1);
        if (a == NULL)
        {
            free(rgb);
            delete asset;
            return NULL;
        }
        for (int i=0; i<asset->m_IconLargeSize.x*asset->m_IconLargeSize.y; i++)
        {
            #pragma warning(push)
            #pragma warning(disable:6385) // False positive
            rgb[(i*3) + 0] = asset->m_IconLargeData[(i*4) + 0];
            rgb[(i*3) + 1] = asset->m_IconLargeData[(i*4) + 1];
            rgb[(i*3) + 2] = asset->m_IconLargeData[(i*4) + 2];
            a[i]           = asset->m_IconLargeData[(i*4) + 3];
            #pragma warning(pop)
        }
        img = wxImage(asset->m_IconLargeSize.x, asset->m_IconLargeSize.y, rgb, a, false);
        asset->m_IconLarge.CopyFromBitmap(img);
    }
    else
    {
        asset->m_IconLargeData = NULL;
        asset->m_IconLarge = wxIcon();
    }

    // Small thumbnail
    deserialize_u32(&bytesptr, (uint32_t*)&asset->m_IconSmallSize.x);
    deserialize_u32(&bytesptr, (uint32_t*)&asset->m_IconSmallSize.y);
    datasize = asset->m_IconSmallSize.x*asset->m_IconSmallSize.y*4;
    if (datasize)
    {
        uint8_t* rgb = NULL;
        uint8_t* a = NULL;
        wxImage img;

        asset->m_IconSmallData = (uint8_t*)malloc(datasize);
        if (asset->m_IconSmallData == NULL)
        {
            delete asset;
            return NULL;
        }
        deserialize_buffer(&bytesptr, asset->m_IconSmallData, datasize);

        // Copy the data into two buffers
        rgb = (uint8_t*)malloc(asset->m_IconSmallSize.x*asset->m_IconSmallSize.y*3);
        if (rgb == NULL)
        {
            delete asset;
            return NULL;
        }
        a = (uint8_t*)malloc(asset->m_IconSmallSize.x*asset->m_IconSmallSize.y*1);
        if (a == NULL)
        {
            free(rgb);
            delete asset;
            return NULL;
        }
        for (int i=0; i<asset->m_IconSmallSize.x*asset->m_IconSmallSize.y; i++)
        {
            #pragma warning(push)
            #pragma warning(disable:6385) // False positive
            rgb[(i*3) + 0] = asset->m_IconSmallData[(i*4) + 0];
            rgb[(i*3) + 1] = asset->m_IconSmallData[(i*4) + 1];
            rgb[(i*3) + 2] = asset->m_IconSmallData[(i*4) + 2];
            a[i]           = asset->m_IconSmallData[(i*4) + 3];
            #pragma warning(pop)
        }
        img = wxImage(asset->m_IconSmallSize.x, asset->m_IconSmallSize.y, rgb, a, false);
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


/*==============================
    P64AssetThumbnail::GenerateThumbnails
    Takes an image and generates correctly sized thumbnails for
    them.
    @param An array with RGB data for the image
    @param An array with the alpha data for the image
    @param The width of the source image
    @param The height of the source image
==============================*/

void P64AssetThumbnail::GenerateThumbnails(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in)
{
    wxImage temp;
    uint32_t datasize;
    float wratio, hratio;

    // Cleanup old stuff
    if (this->m_IconLargeData != NULL)
        free(this->m_IconLargeData);
    if (this->m_IconSmallData != NULL)
        free(this->m_IconSmallData);
    this->m_IconLargeSize = wxSize(0, 0);
    this->m_IconSmallSize = wxSize(0, 0);
    this->m_IconLarge = wxIcon();
    this->m_IconSmall = wxIcon();

    // Calculate image ratio
    if (w_in > h_in)
    {
        wratio = 1.0;
        hratio = ((float)h_in)/((float)w_in);
    }
    else
    {
        wratio = ((float)w_in)/((float)h_in);
        hratio = 1.0;
    }

    // Make the large icon
    temp = wxImage(w_in, h_in, src, alphasrc, true);
    temp.Rescale(THUMBSIZE_LARGE*wratio, THUMBSIZE_LARGE*hratio);

    // Alocate memory for the large icon
    datasize = temp.GetWidth()*temp.GetHeight()*4;
    this->m_IconLargeSize = wxSize(temp.GetWidth(), temp.GetHeight());
    this->m_IconLargeData = (uint8_t*)malloc(datasize);
    if (this->m_IconLargeData == NULL)
    {
        this->m_IconLargeSize = wxSize(0, 0);
        return;
    }

    //  Store the data from the icon
    this->m_IconLarge.CopyFromBitmap(temp);
    for (uint32_t i=0; i<datasize/4; i++)
    {
        this->m_IconLargeData[(i*4) + 0] = temp.GetData()[(i*3) + 0];
        this->m_IconLargeData[(i*4) + 1] = temp.GetData()[(i*3) + 1];
        this->m_IconLargeData[(i*4) + 2] = temp.GetData()[(i*3) + 2];
        if (temp.GetAlpha() != NULL)
            this->m_IconLargeData[(i*4) + 3] = temp.GetAlpha()[i];
        else
            this->m_IconLargeData[(i*4) + 3] = 255;
    }


    // Make the small icon
    temp = wxImage(w_in, h_in, src, alphasrc, true);
    temp.Rescale(THUMBSIZE_SMALL*wratio, THUMBSIZE_SMALL*hratio);

    // Alocate memory for the small icon
    datasize = temp.GetWidth()*temp.GetHeight()*4;
    this->m_IconSmallSize = wxSize(temp.GetWidth(), temp.GetHeight());
    this->m_IconSmallData = (uint8_t*)malloc(datasize);
    if (this->m_IconSmallData == NULL)
    {
        free(this->m_IconLargeData);
        this->m_IconLargeData = NULL;
        this->m_IconLargeSize = wxSize(0, 0);
        this->m_IconSmallSize = wxSize(0, 0);
        return;
    }

    //  Store the data from the icon
    this->m_IconSmall.CopyFromBitmap(temp);
    for (uint32_t i=0; i<datasize/4; i++)
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


/*==============================
    P64AssetThumbnail::GenerateThumbnails
    Takes an image and generates correctly sized thumbnails for
    them.
    @param A wxImage to generate the thumbnails from
==============================*/

void P64AssetThumbnail::GenerateThumbnails(wxImage img)
{
    if (img.IsOk())
        this->GenerateThumbnails(img.GetData(), img.GetAlpha(), img.GetWidth(), img.GetHeight());
}


/*==============================
    P64AssetThumbnail::IsValidLarge
    Checks if the large thumbnail is valid or not
    @return Whether the large thumbnail is valid or not
==============================*/

bool P64AssetThumbnail::IsValidLarge()
{
    return this->m_IconLargeData != NULL;
}


/*==============================
    P64AssetThumbnail::IsValidSmall
    Checks if the small thumbnail is valid or not
    @return Whether the small thumbnail is valid or not
==============================*/

bool P64AssetThumbnail::IsValidSmall()
{
    return this->m_IconSmallData != NULL;
}


/*==============================
    P64AssetThumbnail::GetLargeIcon
    Gets a wxIcon of the large icon
    @return The large icon
==============================*/

wxIcon P64AssetThumbnail::GetLargeIcon()
{
    return this->m_IconLarge;
}


/*==============================
    P64AssetThumbnail::GetSmallIcon
    Gets a wxIcon of the small icon
    @return The small icon
==============================*/

wxIcon P64AssetThumbnail::GetSmallIcon()
{
    return this->m_IconSmall;
}