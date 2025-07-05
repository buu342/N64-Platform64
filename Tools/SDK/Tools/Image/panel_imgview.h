#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include "asset_image.h"

class Panel_ImgView : public wxScrolledWindow
{
    private:

    protected:
        wxRealPoint m_Zoom;
        wxBitmap m_Bitmap;
        P64Asset_Image* m_LoadedAsset;
        void OnMouseWheel(wxMouseEvent& event);
        void OnPaint(wxPaintEvent& event);
        void RefreshDrawing();

    public:
        Panel_ImgView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_ImgView();
        void LoadImageFromPath(wxBitmap& image);
        void SetAsset(P64Asset_Image* asset);
        void ReloadAsset();
};