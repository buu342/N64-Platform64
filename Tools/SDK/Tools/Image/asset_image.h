#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>
#include <wx/bitmap.h>
#include <vector>
#include <stdint.h>

typedef enum {
    RESIZETYPE_NONE = 0,
    RESIZETYPE_POWER2,
    RESIZETYPE_CUSTOM,
} P64Img_Resize;

typedef enum {
    RESIZEALIGN_TOPLEFT = 0,
    RESIZEALIGN_TOPMIDDLE,
    RESIZEALIGN_TOPRIGHT,
    RESIZEALIGN_MIDDLELEFT,
    RESIZEALIGN_CENTER,
    RESIZEALIGN_MIDDLERIGHT,
    RESIZEALIGN_BOTTOMLEFT,
    RESIZEALIGN_BOTTOMMIDDLE,
    RESIZEALIGN_BOTTOMRIGHT,
} P64Img_Alignment;

typedef enum {
    RESIZEFILL_INVISIBLE = 0,
    RESIZEFILL_EDGE,
    RESIZEFILL_REPEAT,
} P64Img_Fill;

typedef enum {
    FMT_RGBA32 = 0,
    FMT_RGBA16,
    //FMT_YUV16,
    FMT_IA16,
    FMT_IA8,
    FMT_IA4,
    FMT_I8,
    FMT_I4,
    FMT_CI8,
    FMT_CI4,
} P64Img_Format;

typedef enum {
    TILING_MIRROR = 0,
    TILING_WRAP,
    TILING_CLAMP,
} P64Img_Tiling;

typedef enum {
    QUANTIZATION_NONE = 0,
    QUANTIZATION_MEDIANCUT,
    QUANTIZATION_MEDIANCUT_FLOYDSTEINBERG,
} P64Img_QuantizationMode;

typedef enum {
    ALPHA_NONE = 0,
    ALPHA_MASK,
    ALPHA_CUSTOM,
    ALPHA_EXTERNALMASK,
} P64Img_AlphaMode;

class P64Asset_Image
{
    private:

    public:
        wxFileName               m_SourcePath;
        wxBitmap                 m_Bitmap;
        P64Img_Resize            m_ResizeMode;
        wxPoint                  m_CustomSize;
        P64Img_Alignment         m_Alignment;
        P64Img_Fill              m_ResizeFill;
        P64Img_Format            m_ImageFormat;
        P64Img_Tiling            m_TilingX;
        P64Img_Tiling            m_TilingY;
        wxPoint                  m_MaskStart;
        bool                     m_UseMipmaps;
        P64Img_QuantizationMode  m_Quantization;
        P64Img_AlphaMode         m_AlphaMode;
        wxColor                  m_AlphaColor;
        wxFileName               m_AlphaPath;
        wxBitmap                 m_BitmapAlpha;
        wxColor*                 m_FinalTexels;

        P64Asset_Image();
        ~P64Asset_Image();
        wxColor* RegenerateFinal();
        std::vector<uint8_t> Serialize();
        static P64Asset_Image Deserialize(std::vector<uint8_t> bytes);
};