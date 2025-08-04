#include "panel_imgview.h"
#include <math.h>
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include "../resource.h"

#define ZOOM_SPEED  1.25

Panel_ImgView::Panel_ImgView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle) : wxScrolledWindow(parent, id, pos, size, style)
{
    this->m_LoadedAsset = NULL;
    this->SetDefaultSettings();
    this->m_Bitmap = wxBitmap();
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(Panel_ImgView::OnPaint), NULL, this);
}

Panel_ImgView::~Panel_ImgView()
{

}

void Panel_ImgView::SetDefaultSettings()
{
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
    this->m_PreviewSettings.showalpha = true;
    this->m_PreviewSettings.showfilter = false;
    this->m_PreviewSettings.showtiling = false;
    this->m_PreviewSettings.showstats = false;
}

void Panel_ImgView::SetAsset(P64Asset_Image* asset)
{
    this->m_LoadedAsset = asset;
    this->ReloadAsset();
}

void Panel_ImgView::ZoomIn()
{
    if (this->m_PreviewSettings.showfilter)
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x*ZOOM_SPEED, this->m_PreviewSettings.zoom.y*ZOOM_SPEED);
    this->RefreshDrawing();
}

void Panel_ImgView::ZoomOut()
{
    if (this->m_PreviewSettings.showfilter)
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x/ZOOM_SPEED, this->m_PreviewSettings.zoom.y/ZOOM_SPEED);
    this->RefreshDrawing();
}

void Panel_ImgView::ZoomReset()
{
    if (this->m_PreviewSettings.showfilter)
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
    this->RefreshDrawing();
}
wxRealPoint Panel_ImgView::GetZoom()
{
    return this->m_PreviewSettings.zoom;
}

void Panel_ImgView::ToggleAlphaDisplay()
{
    this->m_PreviewSettings.showalpha = !this->m_PreviewSettings.showalpha;
}

bool Panel_ImgView::GetAlphaDisplay()
{
    return this->m_PreviewSettings.showalpha;
}

void Panel_ImgView::ToggleFilterDisplay()
{
    this->m_PreviewSettings.showfilter = !this->m_PreviewSettings.showfilter;
}

bool Panel_ImgView::GetFilterDisplay()
{
    return this->m_PreviewSettings.showfilter;
}

void Panel_ImgView::ReloadAsset()
{
    if (this->m_LoadedAsset == NULL || !this->m_LoadedAsset->m_BitmapFinal.IsOk())
        this->m_Bitmap = Tex_Missing;
    else
        this->m_Bitmap = this->m_LoadedAsset->m_BitmapFinal;
    this->RefreshDrawing();
}

void Panel_ImgView::RefreshDrawing()
{
    if (this->m_LoadedAsset == NULL || !this->m_Bitmap.IsOk())
        return;
    this->SetVirtualSize(this->m_Bitmap.GetWidth()*this->m_PreviewSettings.zoom.x, this->m_Bitmap.GetHeight()*this->m_PreviewSettings.zoom.y);
    this->Layout();
    this->Refresh();
}

void Panel_ImgView::OnPaint(wxPaintEvent& event)
{
    int x, y;
    int screen_w, screen_h;
    int img_w, img_h;
    wxRealPoint zoom = this->m_PreviewSettings.zoom;
    
    // Prepare the drawing context
    wxPaintDC dc(this);
    PrepareDC(dc);

    dc.SetUserScale(zoom.x, zoom.y);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME)));
    dc.Clear();

    // If we have a valid asset
    if (this->m_LoadedAsset != NULL && this->m_Bitmap.IsOk())
    {
        int xstart, ystart, i;

        // Calculate its size and position on the DC
        dc.GetSize(&screen_w, &screen_h);
        x = (screen_w/zoom.x)/2;
        y = (screen_h/zoom.y)/2;
        img_w = this->m_Bitmap.GetWidth()*zoom.x;
        img_h = this->m_Bitmap.GetHeight()*zoom.y;
        if (img_w > screen_w)
            x = ((img_w/zoom.x)/2);
        if (img_h > screen_h)
            y = ((img_h/zoom.y)/2);
        xstart = x - ((img_w/zoom.x)/2);
        ystart = y - ((img_h/zoom.y)/2);

        // Draw the transparent background
        i = 0;
        dc.SetUserScale(1, 1);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxColor(255, 255, 255, 255)));
        dc.DrawRectangle(wxRect(xstart*zoom.x, ystart*zoom.y, img_w, img_h));
        dc.SetBrush(wxBrush(wxColor(192, 192, 192, 255)));
        while (true)
        {
            const float GRIDSIZE = 16; 
            const int xcheck_total = ceilf(((float)img_w)/GRIDSIZE);
            const int ycheck_total = ceilf(((float)img_h)/GRIDSIZE);
            const float xcheck_total_frac = xcheck_total - (img_w/GRIDSIZE);
            const float ycheck_total_frac = ycheck_total - (img_h/GRIDSIZE);
            const int xcheck_curr = (i%xcheck_total);
            const int ycheck_curr = (i/xcheck_total);
            float w = (xcheck_curr+1 < xcheck_total) ? GRIDSIZE : GRIDSIZE*(1-xcheck_total_frac);
            float h = (ycheck_curr+1 < ycheck_total) ? GRIDSIZE : GRIDSIZE*(1-ycheck_total_frac);
            dc.DrawRectangle(wxRect(xstart*zoom.x + xcheck_curr*GRIDSIZE, ystart*zoom.y + ycheck_curr*GRIDSIZE, w, h));
            i += 2;
            if (xcheck_curr+2 >= xcheck_total)
            {
                if (ycheck_curr+1 >= ycheck_total)
                    break;
                i = (ycheck_curr+1)*xcheck_total;
                if (ycheck_curr%2 == 0)
                    i++;
            }
        }

        // Draw the texture
        if (!this->m_PreviewSettings.showfilter)
        {
            dc.SetUserScale(zoom.x, zoom.y);
            dc.DrawBitmap(this->m_Bitmap, x - ((img_w/zoom.x)/2), y - ((img_h/zoom.y)/2), false);
        }
        else
            dc.DrawBitmap(this->m_Bitmap, screen_w/2 - (this->m_Bitmap.GetWidth()/2), screen_h/2 - (this->m_Bitmap.GetWidth()/2), false);

    }
    event.Skip();
}