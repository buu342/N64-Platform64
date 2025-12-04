#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>
#include <wx/bitmap.h>
#include <vector>
#include <stdint.h>
#include "../asset_thumbnail.h"
#include "../asset.h"


/*=============================================================
                             Types
=============================================================*/

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
    RESIZEFILL_MIRROR,
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

struct PreviewSettings_Image {
    bool showalpha;
    bool showalphamask;
    bool showfilter;
    bool showtiling;
    bool showstats;
    wxRealPoint zoom;
};


/*=============================================================
                            Classes
=============================================================*/

class P64Asset_Image : public P64Asset
{
    private:
        wxImage  m_ImageFinalRaw;

        wxSize CalculateImageSize();
        void ResizeAndMask(uint8_t** srcptr, uint8_t depth, uint32_t w, uint32_t h);
        void Blur_Average(uint8_t** srcptr, uint8_t depth, uint32_t w_in, uint32_t h_in, wxRealPoint zoom);
        void Blur_Bilinear(uint8_t** srcptr, uint8_t depth, uint32_t w_in, uint32_t h_in, wxRealPoint zoom);
        void ReduceTexel(uint8_t* rgb);
        void ReduceAlpha(uint8_t* a);
        void Dither_Ordered(uint8_t* rgb, uint32_t i, uint32_t w, uint32_t h);
        void Dither_FloydSteinberg(uint8_t* rgb, uint32_t i, uint32_t w, uint32_t h);
        void GenerateTexels(uint8_t* src, uint8_t* alphasrc, uint32_t w_in, uint32_t h_in);

    protected:

    public:
        // File data
        wxFileName            m_SourcePath;
        P64Img_Resize         m_ResizeMode;
        wxSize                m_CustomSize;
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
        wxSize                m_FinalSize;
        uint32_t              m_FinalTexelCount;
        uint8_t*              m_FinalTexels;

        wxImage               m_SourceImage;
        wxImage               m_SourceAlpha;
        wxImage               m_PreviewImage;

        bool IsOk();
        void GenerateFinal(wxFileName assetpath);
        void GeneratePreview(PreviewSettings_Image settings);
        uint32_t CalculateTexelCount();

        void operator=(const P64Asset_Image& rhs);

        P64Asset_Image();
        ~P64Asset_Image();
};
