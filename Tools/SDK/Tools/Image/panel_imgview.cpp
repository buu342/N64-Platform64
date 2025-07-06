#include "panel_imgview.h"
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include "../resource.h"

#define ZOOM_SPEED  1.25

Panel_ImgView::Panel_ImgView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle) : wxScrolledWindow(parent, id, pos, size, style)
{
    this->m_LoadedAsset = NULL;
    this->m_Bitmap = wxBitmap();
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(Panel_ImgView::OnPaint), NULL, this);
    this->ZoomReset();
}

Panel_ImgView::~Panel_ImgView()
{

}

void Panel_ImgView::SetAsset(P64Asset_Image* asset)
{
    this->m_LoadedAsset = asset;
    this->ReloadAsset();
}

void Panel_ImgView::ZoomIn()
{
    this->m_Zoom = wxRealPoint(this->m_Zoom.x*ZOOM_SPEED, this->m_Zoom.y*ZOOM_SPEED);
    this->RefreshDrawing();
}

void Panel_ImgView::ZoomOut()
{
    this->m_Zoom = wxRealPoint(this->m_Zoom.x/ZOOM_SPEED, this->m_Zoom.y/ZOOM_SPEED);
    this->RefreshDrawing();
}

void Panel_ImgView::ZoomReset()
{
    this->m_Zoom = wxRealPoint(1.0, 1.0);
    this->RefreshDrawing();
}

void Panel_ImgView::ReloadAsset()
{
    if (this->m_LoadedAsset == NULL || !this->m_LoadedAsset->m_Bitmap.IsOk())
        this->m_Bitmap = Tex_Missing;
    else
        this->m_Bitmap = this->m_LoadedAsset->m_Bitmap;
    this->ZoomReset();
    this->RefreshDrawing();
}

void Panel_ImgView::RefreshDrawing()
{
    if (this->m_LoadedAsset == NULL || !this->m_Bitmap.IsOk())
        return;
    this->SetVirtualSize(this->m_Bitmap.GetWidth()*this->m_Zoom.x, this->m_Bitmap.GetHeight()*this->m_Zoom.y);
    this->Layout();
    this->Refresh();
}

void Panel_ImgView::OnPaint(wxPaintEvent& event)
{
    int x, y;
    int screen_w, screen_h;
    int img_w, img_h;

    // Prepare the drawing context
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetUserScale(this->m_Zoom.x, this->m_Zoom.y);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME)));
    dc.Clear();

    // Draw the texture
    if (this->m_LoadedAsset != NULL && this->m_Bitmap.IsOk())
    {
        dc.GetSize(&screen_w, &screen_h);
        x = (screen_w/this->m_Zoom.x)/2;
        y = (screen_h/this->m_Zoom.y)/2;
        img_w = this->m_Bitmap.GetWidth()*this->m_Zoom.x;
        img_h = this->m_Bitmap.GetHeight()*this->m_Zoom.y;
        if (img_w > screen_w)
            x = ((img_w/this->m_Zoom.x)/2);
        if (img_h > screen_h)
            y = ((img_h/this->m_Zoom.y)/2);
        dc.DrawBitmap(this->m_Bitmap, x - ((img_w/this->m_Zoom.x)/2), y - ((img_h/this->m_Zoom.y)/2), false);
    }
}