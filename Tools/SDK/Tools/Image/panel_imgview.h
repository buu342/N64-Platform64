#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include <wx/bitmap.h>

class Panel_ImgView : public wxScrolledWindow
{
    private:

    protected:
        wxBitmap m_Bitmap;
        void OnMouse(wxMouseEvent& event);
        void OnPaint(wxPaintEvent& event);

    public:
        Panel_ImgView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_ImgView();

        void Create(wxWindow* parent, wxWindowID id = -1);
        void LoadImageFromPath(wxBitmap& image);
};