#pragma once

typedef struct IUnknown IUnknown;

#include <wx/filename.h>

typedef enum {
    G_IM_FMT_RGBA32,
    G_IM_FMT_RGBA16,
    G_IM_FMT_YUV16,
    G_IM_FMT_CI8,
    G_IM_FMT_CI4,
    G_IM_FMT_IA16,
    G_IM_FMT_IA8,
    G_IM_FMT_IA4,
    G_IM_FMT_I8,
    G_IM_FMT_I4,
} N64ImageType;

class P64Asset_Image
{
    private:
        wxFileName m_SourcePath;
        N64ImageType m_ImageMode;

    public:
        P64Asset_Image();
        ~P64Asset_Image();
};