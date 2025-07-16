#include "asset_image.h"
#include "../../serializer.h"
#include <wx/msgdlg.h>
#include <wx/rawbmp.h>

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
    this->m_Quantization = QUANTIZATION_MEDIANCUT;
    this->m_AlphaMode = ALPHA_NONE;
    this->m_AlphaColor = wxColor(0, 0, 0);
    this->m_AlphaPath = "";

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
    serialize_u8(&data, this->m_Quantization);
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
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_Quantization);
    pos = deserialize_u8(bytesptr, pos, (uint8_t*)&asset->m_AlphaMode);
    pos = deserialize_u8(bytesptr, pos, &temp_8[0]);
    pos = deserialize_u8(bytesptr, pos, &temp_8[1]);
    pos = deserialize_u8(bytesptr, pos, &temp_8[2]);
    asset->m_AlphaColor.Set(temp_8[0], temp_8[1], temp_8[2], 255);
    pos = deserialize_wxstring(bytesptr, pos, &temp_str);
    asset->m_AlphaPath = wxFileName(temp_str);

    return asset;
}

void P64Asset_Image::RegenerateFinal()
{
    if (!this->m_Image.IsOk())
        return;
    uint32_t rawwidth = this->m_Image.GetWidth();
    uint32_t rawheight = this->m_Image.GetHeight();
    unsigned char* base_alpha = NULL;
    unsigned char* base_rgb = NULL;

    // Get the raw RGB data
    base_rgb = (unsigned char*)malloc(rawwidth*rawheight*3);
    memcpy(base_rgb, this->m_Image.GetData(), rawwidth*rawheight*3);

    // Handle alpha
    switch (this->m_AlphaMode)
    {
        case ALPHA_NONE: break;
        case ALPHA_MASK:
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            if (this->m_Image.GetAlpha() != NULL)
                memcpy(base_alpha, this->m_Image.GetAlpha(), rawwidth*rawheight);
            else
                memset(base_alpha, 255, rawwidth*rawheight);
            break;
        case ALPHA_CUSTOM:
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            for (int i=0; i<rawwidth*rawheight; i++)
            {
                if (base_rgb[(i*3)+0] == this->m_AlphaColor.GetRed() && base_rgb[(i*3)+1] == this->m_AlphaColor.GetGreen() && base_rgb[(i*3)+2] == this->m_AlphaColor.GetBlue())
                    base_alpha[i] = 0;
                else
                    base_alpha[i] = 255;
            }
            break;
        case ALPHA_EXTERNALMASK:
            base_alpha = (unsigned char*)malloc(rawwidth*rawheight);
            if (this->m_ImageAlpha.IsOk())
            {
                wxImage bw = this->m_ImageAlpha.ConvertToGreyscale();
                memcpy(base_alpha, bw.GetData(), rawwidth*rawheight);
            }
            else
                memset(base_alpha, 255, rawwidth*rawheight);
            break;
    }

    this->m_ImageFinal = wxImage(rawwidth, rawheight, base_rgb, base_alpha, false);
    this->m_BitmapFinal = wxBitmap(this->m_ImageFinal);
}