#include "asset_image.h"
#include "../../serializer.h"
#include <wx/msgdlg.h>
#include <wx/rawbmp.h>
#include <algorithm> 

#define P64ASSET_HEADER "P64RAWIMG"
#define P64ASSET_VERSION 1


P64Asset_Image::P64Asset_Image()
{
    this->m_SourcePath = "";
    this->m_ResizeMode = RESIZETYPE_NONE;
    this->m_CustomSize = wxPoint(0, 0);
    this->m_Alignment = RESIZEALIGN_CENTER;
    this->m_ResizeFill = RESIZEFILL_INVISIBLE;
    this->m_ImageFormat = FMT_RGBA16;
    this->m_TilingX = TILING_MIRROR;
    this->m_TilingY = TILING_MIRROR;
    this->m_MaskStart = wxPoint(0, 0);
    this->m_UseMipmaps = false;
    this->m_Dithering = DITHERING_ORDERED;
    this->m_AlphaMode = ALPHA_NONE;
    this->m_AlphaColor = wxColor(0, 0, 0);
    this->m_AlphaPath = "";

    this->m_FinalSize = wxPoint(0, 0);
    this->m_FinalTexels = NULL;

    this->m_Image = wxImage();
    this->m_ImageAlpha = wxImage();
    this->m_ImageFinal = wxImage();
    this->m_BitmapFinal = wxBitmap();
}

P64Asset_Image::~P64Asset_Image()
{

}

std::vector<uint8_t> P64Asset_Image::Serialize()
{
    std::vector<uint8_t> data;

    serialize_header(&data, P64ASSET_HEADER, P64ASSET_VERSION);
    serialize_wxstring(&data, this->m_SourcePath.GetFullPath());
    serialize_u8(&data, this->m_ResizeMode);
    serialize_u32(&data, this->m_CustomSize.x);
    serialize_u32(&data, this->m_CustomSize.y);
    serialize_u8(&data, this->m_Alignment);
    serialize_u8(&data, this->m_ResizeFill);
    serialize_u8(&data, this->m_ImageFormat);
    serialize_u8(&data, this->m_TilingX);
    serialize_u8(&data, this->m_TilingY);
    serialize_u32(&data, this->m_MaskStart.x);
    serialize_u32(&data, this->m_MaskStart.y);
    serialize_u8(&data, this->m_UseMipmaps);
    serialize_u8(&data, this->m_Dithering);
    serialize_u8(&data, this->m_AlphaMode);
    serialize_u8(&data, this->m_AlphaColor.Red());
    serialize_u8(&data, this->m_AlphaColor.Green());
    serialize_u8(&data, this->m_AlphaColor.Blue());
    serialize_wxstring(&data, this->m_AlphaPath.GetFullPath());

    return data;
}

P64Asset_Image* P64Asset_Image::Deserialize(std::vector<uint8_t> bytes)
{
    P64Asset_Image* asset;
    wxString temp_str;
    uint32_t temp_32;
    uint8_t temp_8[3];
    char header[16];
    uint8_t* bytesptr = &bytes[0];
    uint32_t pos = 0;

    if (bytes.size() < 16)
    {
        wxMessageDialog dialog(NULL, "File is not a P64 image", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }

    pos = deserialize_header(bytesptr, pos, header, &temp_32);
    if (strcmp(header, P64ASSET_HEADER) != 0)
    {
        wxMessageDialog dialog(NULL, "File is not a P64 image", "Error deserializing", wxCENTER | wxOK |  wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }
    if (temp_32 > P64ASSET_VERSION)
    {
        wxMessageDialog dialog(NULL, "File is a more recent, unsupported version", "Error deserializing", wxCENTER | wxOK |  wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }

    asset = new P64Asset_Image();
    pos = deserialize_wxstring(bytesptr, pos, &temp_str);
    asset->m_SourcePath = wxFileName(temp_str);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_ResizeMode);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_CustomSize.x);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_CustomSize.y);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_Alignment);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_ResizeFill);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_ImageFormat);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_TilingX);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_TilingY);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_MaskStart.x);
    pos = deserialize_u32(bytesptr, pos, (uint32_t*)&asset->m_MaskStart.y);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_UseMipmaps);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_Dithering);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_AlphaMode);
    pos = deserialize_u8(bytesptr, pos, &temp_8[0]);
    pos = deserialize_u8(bytesptr, pos, &temp_8[1]);
    pos = deserialize_u8(bytesptr, pos, &temp_8[2]);
    asset->m_AlphaColor.Set(temp_8[0], temp_8[1], temp_8[2], 255);
    pos = deserialize_wxstring(bytesptr, pos, &temp_str);
    asset->m_AlphaPath = wxFileName(temp_str);

    return asset;
}

void P64Asset_Image::ReduceTexel(uint8_t* rgb)
{
    switch (this->m_ImageFormat)
    {
        case FMT_RGBA32:
            break;
        case FMT_RGBA16:
            rgb[0] = rgb[0] >> 3 << 3;
            rgb[1] = rgb[1] >> 3 << 3;
            rgb[2] = rgb[2] >> 3 << 3;
            break;
        default:
            break;
    }
}

void P64Asset_Image::ReduceAlpha(uint8_t* a)
{
    if (a != NULL)
        return;
    switch (this->m_ImageFormat)
    {
        case FMT_IA16:
        case FMT_RGBA32:
            break;
        case FMT_RGBA16:
        case FMT_CI8:
        case FMT_CI4:
        case FMT_IA4:
            (*a) = ((*a) > 128) ? 255 : 0;
            break;
        case FMT_I8:
        case FMT_I4:
            (*a) = 255;
        case FMT_IA8:
            (*a) = (*a) >> 4 << 4;
    }
}

void P64Asset_Image::Dither_Ordered(uint8_t* rgb, uint32_t i, uint32_t w, uint32_t h)
{
    // Taken from https://stackoverflow.com/a/17438757
    const uint8_t dither_treshold_r[64] = {
      1, 7, 3, 5, 0, 8, 2, 6,
      7, 1, 5, 3, 8, 0, 6, 2,
      3, 5, 0, 8, 2, 6, 1, 7,
      5, 3, 8, 0, 6, 2, 7, 1,
      0, 8, 2, 6, 1, 7, 3, 5,
      8, 0, 6, 2, 7, 1, 5, 3,
      2, 6, 1, 7, 3, 5, 0, 8,
      6, 2, 7, 1, 5, 3, 8, 0
    };
    const uint8_t dither_treshold_g[64] = {
      1, 3, 2, 2, 3, 1, 2, 2,
      2, 2, 0, 4, 2, 2, 4, 0,
      3, 1, 2, 2, 1, 3, 2, 2,
      2, 2, 4, 0, 2, 2, 0, 4,
      1, 3, 2, 2, 3, 1, 2, 2,
      2, 2, 0, 4, 2, 2, 4, 0,
      3, 1, 2, 2, 1, 3, 2, 2,
      2, 2, 4, 0, 2, 2, 0, 4
    };
    const uint8_t dither_treshold_b[64] = {
      5, 3, 8, 0, 6, 2, 7, 1,
      3, 5, 0, 8, 2, 6, 1, 7,
      8, 0, 6, 2, 7, 1, 5, 3,
      0, 8, 2, 6, 1, 7, 3, 5,
      6, 2, 7, 1, 5, 3, 8, 0,
      2, 6, 1, 7, 3, 5, 0, 8,
      7, 1, 5, 3, 8, 0, 6, 2,
      1, 7, 3, 5, 0, 8, 2, 6
    };

    // Perform the dither
    uint8_t tresshold_id = (((i/w) & 7) << 3) + ((i%w) & 7);
    rgb[(i*3)+0] = std::min(rgb[(i*3)+0] + dither_treshold_r[tresshold_id], 255);
    rgb[(i*3)+1] = std::min(rgb[(i*3)+1] + dither_treshold_r[tresshold_id], 255);
    rgb[(i*3)+2] = std::min(rgb[(i*3)+2] + dither_treshold_r[tresshold_id], 255);
}

void P64Asset_Image::Dither_FloydSteinberg(uint8_t* rgb, uint32_t i, uint32_t w, uint32_t h)
{
    // Make a copy of the original colors before we bit-crush it
    uint32_t original_r = rgb[(i*3)+0];
    uint32_t original_g = rgb[(i*3)+1];
    uint32_t original_b = rgb[(i*3)+2];

    // Get the reduced color value
    this->ReduceTexel(&rgb[i*3]);

    // Grab the bit-crushed color and then calculate the error
    uint32_t transformed_r = rgb[(i*3)+0];
    uint32_t transformed_g = rgb[(i*3)+1];
    uint32_t transformed_b = rgb[(i*3)+2];
    int32_t error_r = original_r - transformed_r;
    int32_t error_g = original_g - transformed_g;
    int32_t error_b = original_b - transformed_b;
    uint32_t x = i%w;
    uint32_t y = i/w;
    uint32_t offset;

    // Apply the kernel to the offset pixels
    if (x+1 < w) // Right
    {
        offset = i+1;
        rgb[(offset*3)+0] = std::min((rgb[(offset*3)+0] + (((error_r*7)/16))), 255);
        rgb[(offset*3)+1] = std::min((rgb[(offset*3)+1] + (((error_g*7)/16))), 255);
        rgb[(offset*3)+2] = std::min((rgb[(offset*3)+2] + (((error_b*7)/16))), 255);
    }
    if (y+1 < h) // Bottom
    {
        if (x-1 > 0) // Bottom left
        {
            offset = i + w-1;
            rgb[(offset*3)+0] = std::min((rgb[(offset*3)+0] + (((error_r*3)/16))), 255);
            rgb[(offset*3)+1] = std::min((rgb[(offset*3)+1] + (((error_g*3)/16))), 255);
            rgb[(offset*3)+2] = std::min((rgb[(offset*3)+2] + (((error_b*3)/16))), 255);
        }
        // Bottom middle
        offset = i + w;
        rgb[(offset*3)+0] = std::min((rgb[(offset*3)+0] + (((error_r*5)/16))), 255);
        rgb[(offset*3)+1] = std::min((rgb[(offset*3)+1] + (((error_g*5)/16))), 255);
        rgb[(offset*3)+2] = std::min((rgb[(offset*3)+2] + (((error_b*5)/16))), 255);
        if (x+1 < w) // Bottom right
        {
            offset = i+1 + w;
            rgb[(offset*3)+0] = std::min((rgb[(offset*3)+0] + (((error_r*1)/16))), 255);
            rgb[(offset*3)+1] = std::min((rgb[(offset*3)+1] + (((error_g*1)/16))), 255);
            rgb[(offset*3)+2] = std::min((rgb[(offset*3)+2] + (((error_b*1)/16))), 255);
        }
    }
}

void P64Asset_Image::RegenerateFinal()
{
    if (!this->m_Image.IsOk())
        return;
    uint32_t rawwidth = this->m_Image.GetWidth();
    uint32_t rawheight = this->m_Image.GetHeight();
    unsigned char* base_alpha = NULL;
    unsigned char* base_rgb = NULL;

    // Get the raw RGB data from the image
    base_rgb = (unsigned char*)malloc(rawwidth*rawheight*3);
    if (base_rgb == NULL)
        return;
    memcpy(base_rgb, this->m_Image.GetData(), rawwidth*rawheight*3);

    // Handle alpha
    switch (this->m_AlphaMode)
    {
        case ALPHA_NONE: break;
        case ALPHA_MASK:
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            if (base_alpha != NULL)
            {
                if (this->m_Image.GetAlpha() != NULL)
                    memcpy(base_alpha, this->m_Image.GetAlpha(), rawwidth*rawheight);
                else
                    memset(base_alpha, 255, rawwidth*rawheight);
            }
            break;
        case ALPHA_CUSTOM:
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            if (base_alpha != NULL)
            {
                for (uint32_t i=0; i<rawwidth*rawheight; i++)
                {
                    if (base_rgb[(i*3)+0] == this->m_AlphaColor.GetRed() && base_rgb[(i*3)+1] == this->m_AlphaColor.GetGreen() && base_rgb[(i*3)+2] == this->m_AlphaColor.GetBlue())
                        base_alpha[i] = 0;
                    else
                        base_alpha[i] = 255;
                }
            }
            break;
        case ALPHA_EXTERNALMASK:
            // TODO: External mask must have the same size as the source image (before any resizing and masking)
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            if (base_alpha != NULL)
            {
                if (this->m_ImageAlpha.IsOk())
                {
                    wxImage bw = this->m_ImageAlpha.ConvertToGreyscale();
                    memcpy(base_alpha, bw.GetData(), rawwidth*rawheight);
                }
                else
                    memset(base_alpha, 255, rawwidth*rawheight);
            }
            break;
    }

    // Perform scaling on the image

    // Generate Mipmaps

    // Perform bit reduction + dithering on the RGB values
    for (uint32_t i=0; i<rawwidth*rawheight; i++)
    {
        switch (this->m_Dithering)
        {
            case DITHERING_NONE: break;
            case DITHERING_ORDERED: this->Dither_Ordered(base_rgb, i, rawwidth, rawheight); break;
            case DITHERING_FLOYDSTEINBERG: this->Dither_FloydSteinberg(base_rgb, i, rawwidth, rawheight); break;
        }
        this->ReduceTexel(&base_rgb[(i*3)]);
        if (base_alpha != NULL)
            this->ReduceAlpha(&base_alpha[i]);
    }
    
    // Generate the final texels

    // Generate some images for wxWidgets preview
    this->m_ImageFinal = wxImage(rawwidth, rawheight, base_rgb, base_alpha, false);
    this->m_BitmapFinal = wxBitmap(this->m_ImageFinal);
}