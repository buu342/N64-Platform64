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
    this->m_CustomSize = wxSize(0, 0);
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

    this->m_FinalSize = wxSize(0, 0);
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

static uint32_t nearestpow2(uint32_t num)
{
    // Taken from https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;
    return num;
}

wxSize P64Asset_Image::CalculateImageSize()
{
    uint32_t w, h;
    if (!this->m_Image.IsOk())
        return wxSize(1, 1);

    w = this->m_Image.GetWidth();
    h = this->m_Image.GetHeight();
    switch (this->m_ResizeMode)
    {
        case RESIZETYPE_NONE:
            break;
        case RESIZETYPE_POWER2:
            w = nearestpow2(w);
            h = nearestpow2(h);
            break;
        case RESIZETYPE_CUSTOM:
            w = this->m_CustomSize.x;
            h = this->m_CustomSize.y;
            break;
    }
    if (w <= 0)
        w = 1;
    if (h <= 0)
        h = 1;

    // In 4-bit modes, there must be an even number of column pixels
    if ((this->m_ImageFormat == FMT_IA4 || this->m_ImageFormat == FMT_CI4) && (w%2 == 1))
        w++;
    return wxSize(w, h);
}

uint32_t P64Asset_Image::CalculateTexelCount()
{
    wxSize size = this->CalculateImageSize();
    switch (this->m_ImageFormat)
    {
        case FMT_RGBA32: return size.x * size.y * 4;
        case FMT_RGBA16: return size.x * size.y * 2;
        case FMT_IA16:   return size.x * size.y * 2;
        case FMT_IA8:    return size.x * size.y;
        case FMT_IA4:    return ceilf(((float)size.x)/2) * size.y;
        case FMT_I8:     return size.x * size.y;
        case FMT_I4:     return ceilf(((float)size.x)/2) * size.y;
        case FMT_CI8:    return (size.x * size.y * 2) + 2048;
        case FMT_CI4:    return (ceilf(((float)size.x)/2) * size.y * 2) + 2048;
    }
    return 0; // Shouldn't happen
}

static wxPoint SamplePoint_Clamp(wxPoint srcstart, wxPoint srcend, wxPoint samplepoint)
{
    return wxPoint(
        std::min(srcend.x-1, std::max(srcstart.x, samplepoint.x)), 
        std::min(srcend.y-1, std::max(srcstart.y, samplepoint.y))
    );
}

static wxPoint SamplePoint_Repeat(wxPoint srcstart, wxPoint srcend, wxPoint samplepoint)
{
    int32_t rx = samplepoint.x - srcstart.x;
    int32_t ry = samplepoint.y - srcstart.y;
    int32_t w = (srcend.x - srcstart.x);
    int32_t h = (srcend.y - srcstart.y);
    return wxPoint(
        srcstart.x + ((rx % w + w) % w),
        srcstart.y + ((ry % h + h) % h)
    );
}

static wxPoint SamplePoint_Mirror(wxPoint srcstart, wxPoint srcend, wxPoint samplepoint)
{
    int32_t rx = samplepoint.x - srcstart.x;
    int32_t ry = samplepoint.y - srcstart.y;
    int32_t w = (srcend.x - srcstart.x);
    int32_t h = (srcend.y - srcstart.y);
    wxPoint ret = wxPoint(
        ((rx % w + w) % w),
        ((ry % h + h) % h)
    );
    if (((int)abs(floorf((float)rx/w))) % 2 == 1)
        ret.x = w - ret.x - 1;
    if (((int)abs(floorf((float)ry/h))) % 2 == 1)
        ret.y = h - ret.y - 1;
    return srcstart + ret;
}

void P64Asset_Image::ResizeAndMask(uint8_t** srcptr, uint8_t depth, uint32_t w, uint32_t h)
{
    if (*srcptr == NULL)
        return;
    if (this->m_ResizeMode != RESIZETYPE_NONE)
    {
        wxPoint anchor;
        wxSize newsize = this->CalculateImageSize();
        uint8_t* newimg;

        // Figure out the alignment
        switch (this->m_Alignment)
        {
            case RESIZEALIGN_TOPLEFT:      anchor = wxPoint(0,                                   0); break;
            case RESIZEALIGN_TOPMIDDLE:    anchor = wxPoint((((float)newsize.x)/2-((float)w)/2), 0); break;
            case RESIZEALIGN_TOPRIGHT:     anchor = wxPoint(newsize.x-w,                         0); break;
            case RESIZEALIGN_MIDDLELEFT:   anchor = wxPoint(0,                                   (((float)newsize.y)/2-((float)h)/2)); break;
            case RESIZEALIGN_CENTER:       anchor = wxPoint((((float)newsize.x)/2-((float)w)/2), (((float)newsize.y)/2-((float)h)/2)); break;
            case RESIZEALIGN_MIDDLERIGHT:  anchor = wxPoint(newsize.x-w,                         (((float)newsize.y)/2-((float)h)/2)); break;
            case RESIZEALIGN_BOTTOMLEFT:   anchor = wxPoint(0,                                   (newsize.y-h)); break;
            case RESIZEALIGN_BOTTOMMIDDLE: anchor = wxPoint((((float)newsize.x)/2-((float)w)/2), (newsize.y-h)); break;
            case RESIZEALIGN_BOTTOMRIGHT:  anchor = wxPoint(newsize.x-w,                         (newsize.y-h)); break;
        }
        
        // Resize the image
        newimg = (uint8_t*)malloc(newsize.x*newsize.y*depth*sizeof(uint8_t));
        if (newimg== NULL)
            return;

        for (int y=0; y<newsize.y; y++)
        {
            for (int x=0; x<newsize.x; x++)
            {
                wxPoint samplepos;
                switch (this->m_ResizeFill)
                {
                    case RESIZEFILL_INVISIBLE:
                        samplepos = wxPoint(x - anchor.x, y - anchor.y);
                        for (int d=0; d<depth; d++)
                        {
                            if (samplepos.x >= 0 && samplepos.x < ((int)w) && samplepos.y >= 0 && samplepos.y < ((int)h))
                                newimg[y*depth*newsize.x + (x*depth + d)] = (*srcptr)[samplepos.y*depth*w + (samplepos.x*depth + d)];
                            else
                                newimg[y*depth*newsize.x + (x*depth + d)] = 0;
                        }
                        break;
                    case RESIZEFILL_EDGE: 
                        samplepos = SamplePoint_Clamp(anchor, anchor+wxPoint(w, h), wxPoint(x, y));
                        samplepos -= anchor;
                        for (int d=0; d<depth; d++)
                            newimg[y*depth*newsize.x + (x*depth + d)] = (*srcptr)[samplepos.y*depth*w + (samplepos.x*depth + d)]; 
                        break;
                    case RESIZEFILL_REPEAT: 
                        samplepos = SamplePoint_Repeat(anchor, anchor+wxPoint(w, h), wxPoint(x, y));
                        samplepos -= anchor;
                        for (int d=0; d<depth; d++)
                            newimg[y*depth*newsize.x + (x*depth + d)] = (*srcptr)[samplepos.y*depth*w + (samplepos.x*depth + d)];
                        break;
                    case RESIZEFILL_MIRROR:
                        samplepos = SamplePoint_Mirror(anchor, anchor + wxPoint(w, h), wxPoint(x, y));
                        samplepos -= anchor;
                        for (int d = 0; d < depth; d++)
                            newimg[y*depth*newsize.x + (x*depth + d)] = (*srcptr)[samplepos.y*depth*w + (samplepos.x*depth + d)];
                        break;
                }
            }
        }
        
        // Finish
        free(*srcptr);
        *srcptr = newimg;
    }

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
    if (a == NULL)
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
            break;
        case FMT_IA8:
            (*a) = (*a) >> 4 << 4;
            break;
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
    /* // I originally accidentally left used the red threshold only (copy paste mistake), but after fixing that I found the results to be visually worse, so these will remain commented out for the time being
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
    };*/

    // Perform the dither
    uint8_t tresshold_id = (((i/w) & 7) << 3) + ((i%w) & 7);
    rgb[(i*3)+0] = std::min(rgb[(i*3)+0] + dither_treshold_r[tresshold_id], 255);
    rgb[(i*3)+1] = std::min(rgb[(i*3)+1] + dither_treshold_r[tresshold_id], 255);
    rgb[(i*3)+2] = std::min(rgb[(i*3)+2] + dither_treshold_r[tresshold_id], 255);
    (void)h;
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

void P64Asset_Image::Bilinear(uint8_t** srcptr, uint8_t depth, uint32_t w_in, uint32_t h_in, wxRealPoint zoom)
{
    if (*srcptr == NULL)
        return;
    uint32_t w_out = roundf(((float)w_in)*zoom.x);
    uint32_t h_out = roundf(((float)h_in)*zoom.y);
    float x_ratio = (((float)w_in) - 1.0)/(((float)w_out) - 1.0);
    float y_ratio = (((float)h_in) - 1.0)/(((float)h_out) - 1.0);
    uint8_t* out = (uint8_t*)malloc(depth*w_out*h_out*sizeof(uint8_t));
    if (out== NULL)
        return;

    for (float y=0; y<h_out; y++)
    {
        for (float x=0; x<w_out; x++)
        {
            float x_l = floor(x_ratio*x);
            float y_l = floor(y_ratio*y);
            float x_h = ceil(x_ratio*x);
            float y_h = ceil(y_ratio*y);
            float x_weight = (x_ratio*x) - x_l;
            float y_weight = (y_ratio*y) - y_l;
            for (int db=0; db<depth; db++)
            {
                float a = (*srcptr)[(int)y_l*w_in*depth + (int)x_l*depth + db];
                float b = (*srcptr)[(int)y_l*w_in*depth + (int)x_h*depth + db];
                float c = (*srcptr)[(int)y_h*w_in*depth + (int)x_l*depth + db];
                float d = (*srcptr)[(int)y_h*w_in*depth + (int)x_h*depth + db];
                float pixel = a*(1.0 - x_weight)*(1.0 - y_weight) +
                              b*x_weight*(1.0 - y_weight) +
                              c*y_weight*(1.0 - x_weight) +
                              d*x_weight*y_weight;
                out[(int)(y*w_out*depth + x*depth + db)] = pixel;
            }
        }
    }
    free(*srcptr);
    (*srcptr) = out;
}

void P64Asset_Image::RegenerateFinal(bool bitmap_alpha, bool bitmap_filter, wxRealPoint zoom)
{
    if (!this->m_Image.IsOk())
        return;
    wxSize rawsize = this->m_Image.GetSize();
    wxSize newsize;
    uint8_t* base_alpha = NULL;
    uint8_t* base_rgb = NULL;

    // Get the raw RGB data from the image
    base_rgb = (uint8_t*)malloc(rawsize.x*rawsize.y*3);
    if (base_rgb == NULL)
        return;
    memcpy(base_rgb, this->m_Image.GetData(), rawsize.x*rawsize.y*3);

    // Handle alpha
    switch (this->m_AlphaMode)
    {
        case ALPHA_NONE: break;
        case ALPHA_MASK:
            base_alpha = (unsigned char*)malloc(rawsize.x*rawsize.y);
            if (base_alpha != NULL)
            {
                if (this->m_Image.GetAlpha() != NULL)
                    memcpy(base_alpha, this->m_Image.GetAlpha(), rawsize.x*rawsize.y);
                else
                    memset(base_alpha, 255, rawsize.x*rawsize.y);
            }
            break;
        case ALPHA_CUSTOM:
            base_alpha = (unsigned char*)malloc(rawsize.x*rawsize.y);
            if (base_alpha != NULL)
            {
                for (uint32_t i=0; i<(uint32_t)rawsize.x*rawsize.y; i++)
                {
                    if (base_rgb[(i*3)+0] == this->m_AlphaColor.GetRed() && base_rgb[(i*3)+1] == this->m_AlphaColor.GetGreen() && base_rgb[(i*3)+2] == this->m_AlphaColor.GetBlue())
                        base_alpha[i] = 0;
                    else
                        base_alpha[i] = 255;
                }
            }
            break;
        case ALPHA_EXTERNALMASK:
            if (this->m_Image.GetSize() == this->m_ImageAlpha.GetSize())
            {
                base_alpha = (unsigned char*)malloc(rawsize.x*rawsize.y);
                if (base_alpha != NULL)
                {
                    if (this->m_ImageAlpha.IsOk())
                    {
                        wxImage bw = this->m_ImageAlpha.ConvertToGreyscale();
                        for (int i=0; i<rawsize.x*rawsize.y; i++)
                            base_alpha[i] = bw.GetData()[i*3];
                    }
                    else
                        memset(base_alpha, 255, rawsize.x*rawsize.y);
                }
            }
            break;
    }

    // Perform scaling on the image and its alpha
    newsize = this->CalculateImageSize();
    this->ResizeAndMask(&base_rgb, 3, rawsize.x, rawsize.y);
    this->ResizeAndMask(&base_alpha, 1, rawsize.x, rawsize.y);

    // Generate Mipmaps

    // Perform bit reduction + dithering on the RGB values
    for (uint32_t i=0; i<(uint32_t)newsize.x*newsize.y; i++)
    {
        switch (this->m_Dithering)
        {
            case DITHERING_NONE: break;
            case DITHERING_ORDERED: this->Dither_Ordered(base_rgb, i, newsize.x, newsize.y); break;
            case DITHERING_FLOYDSTEINBERG: this->Dither_FloydSteinberg(base_rgb, i, newsize.x, newsize.y); break;
        }
        this->ReduceTexel(&base_rgb[(i*3)]);
        if (base_alpha != NULL)
            this->ReduceAlpha(&base_alpha[i]);
    }
    
    // Generate the final texels

    // Generate some images for wxWidgets preview
    if (bitmap_filter)
    {
        this->Bilinear(&base_rgb, 3, newsize.x, newsize.y, zoom);
        this->Bilinear(&base_alpha, 1, newsize.x, newsize.y, zoom);
        newsize.x = roundf(((float)newsize.x)*zoom.x);
        newsize.y = roundf(((float)newsize.y)*zoom.y);
    }
    if (!bitmap_alpha)
    {
        free(base_alpha);
        base_alpha = NULL;
    }
    this->m_ImageFinal = wxImage(newsize.x, newsize.y, base_rgb, base_alpha, false);
    this->m_BitmapFinal = wxBitmap(this->m_ImageFinal);
}