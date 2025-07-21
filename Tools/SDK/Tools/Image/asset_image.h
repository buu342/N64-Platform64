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
    FMT_IA16,
    FMT_IA8,
    FMT_IA4,
    FMT_I8,
    FMT_I4,
    FMT_CI8,
    FMT_CI4,
    //FMT_YUV16,
} P64Img_Format;

typedef enum {
    TILING_MIRROR = 0,
    TILING_WRAP,
    TILING_CLAMP,
} P64Img_Tiling;

typedef enum {
    DITHERING_NONE = 0,
    DITHERING_ORDERED,
    DITHERING_FLOYDSTEINBERG,
} P64Img_DitheringMode;

typedef enum {
    ALPHA_NONE = 0,
    ALPHA_MASK,
    ALPHA_CUSTOM,
    ALPHA_EXTERNALMASK,
} P64Img_AlphaMode;

class P64Asset_Image
{
    private:
        void ReduceTexel(uint8_t* rgb, uint8_t* a);
        void Dither_Ordered(uint8_t* rgb, uint8_t* a, uint32_t i, uint32_t w, uint32_t h);
        void Dither_FloydSteinberg(uint8_t* rgb, uint8_t* a, uint32_t i, uint32_t w, uint32_t h);

    public:
        wxFileName            m_SourcePath;
        P64Img_Resize         m_ResizeMode;
        wxPoint               m_CustomSize;
        P64Img_Alignment      m_Alignment;
        P64Img_Fill           m_ResizeFill;
        P64Img_Format         m_ImageFormat;
        P64Img_Tiling         m_TilingX;
        P64Img_Tiling         m_TilingY;
        wxPoint               m_MaskStart;
        bool                  m_UseMipmaps;
        P64Img_DitheringMode  m_Dithering;
        P64Img_AlphaMode      m_AlphaMode;
        wxColor               m_AlphaColor;
        wxFileName            m_AlphaPath;
        wxPoint               m_FinalSize;
        uint8_t*              m_FinalTexels;

        wxImage               m_Image;
        wxImage               m_ImageAlpha;
        wxImage               m_ImageFinal;
        wxBitmap              m_BitmapFinal;

        P64Asset_Image();
        ~P64Asset_Image();
        void RegenerateFinal();
        std::vector<uint8_t> Serialize();
        static P64Asset_Image* Deserialize(std::vector<uint8_t> bytes);
};
