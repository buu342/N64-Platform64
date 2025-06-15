#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include <wx/bitmap.h>

class Panel_ImgView : public wxScrolledWindow
{
    private:

    protected:
        wxRealPoint m_Zoom;
        wxBitmap m_Bitmap;
        void OnMouseWheel(wxMouseEvent& event);
        void OnPaint(wxPaintEvent& event);
        void RefreshDrawing();

    public:
        Panel_ImgView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_ImgView();
        void LoadImageFromPath(wxBitmap& image);
};