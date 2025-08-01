#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include "asset_image.h"

struct PreviewSettings {
    bool showalpha;
    bool showfilter;
    bool showtiling;
    bool showstats;
    wxRealPoint zoom;
};

class Panel_ImgView : public wxScrolledWindow
{
    private:

    protected:
        wxBitmap m_Bitmap;
        P64Asset_Image* m_LoadedAsset;
        PreviewSettings m_PreviewSettings;
        void OnPaint(wxPaintEvent& event);

    public:
        Panel_ImgView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_ImgView();
        void ZoomIn();
        void ZoomOut();
        void ZoomReset();
        void ToggleAlphaDisplay();
        bool GetAlphaDisplay();
        void SetDefaultSettings();
        void SetAsset(P64Asset_Image* asset);
        void ReloadAsset();
        void RefreshDrawing();
};