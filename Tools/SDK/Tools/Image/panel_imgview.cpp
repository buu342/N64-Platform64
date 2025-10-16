#include "panel_imgview.h"
#include <math.h>
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include "../resource.h"


/*=============================================================
                            Macros
=============================================================*/

#define ZOOM_SPEED  1.25


/*=============================================================
           Image Preview Panel Class Implementation
=============================================================*/

/*==============================
    Panel_ImgView (Constructor)
    Initializes the class
==============================*/

Panel_ImgView::Panel_ImgView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle) : wxScrolledWindow(parent, id, pos, size, style)
{
    this->m_LoadedAsset = NULL;
    this->SetDefaultSettings();
    this->m_Bitmap = wxBitmap();

    // Connect events
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(Panel_ImgView::OnPaint), NULL, this);
}


/*==============================
    Panel_ImgView (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_ImgView::~Panel_ImgView()
{
    // Nothing to cleanup
}


/*==============================
    Panel_ImgView::SetDefaultSettings
    Set the default preview settings
==============================*/

void Panel_ImgView::SetDefaultSettings()
{
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
    this->m_PreviewSettings.showalpha = true;
    this->m_PreviewSettings.showfilter = false;
    this->m_PreviewSettings.showtiling = false;
    this->m_PreviewSettings.showstats = false;
}


/*==============================
    Panel_ImgView::SetAsset
    Set the asset to preview
    @param The asset to preview
==============================*/

void Panel_ImgView::SetAsset(P64Asset_Image* asset)
{
    this->m_LoadedAsset = asset;
    this->ReloadAsset();
}


/*==============================
    Panel_ImgView::ZoomIn
    Zoom the panel in
==============================*/

void Panel_ImgView::ZoomIn()
{
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x*ZOOM_SPEED, this->m_PreviewSettings.zoom.y*ZOOM_SPEED);
    if (this->m_PreviewSettings.showfilter)
    {
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
        this->ReloadAsset();
    }
    else
        this->RefreshDrawing();
}


/*==============================
    Panel_ImgView::ZoomOut
    Zoom the panel out
==============================*/

void Panel_ImgView::ZoomOut()
{
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x/ZOOM_SPEED, this->m_PreviewSettings.zoom.y/ZOOM_SPEED);
    if (this->m_PreviewSettings.showfilter)
    {
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
        this->ReloadAsset();
    }
    else
        this->RefreshDrawing();
}


/*==============================
    Panel_ImgView::ZoomReset
    Reset the panel zoom
==============================*/

void Panel_ImgView::ZoomReset()
{
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
    if (this->m_PreviewSettings.showfilter)
    {
        this->m_LoadedAsset->RegenerateFinal(this->m_PreviewSettings.showalpha, this->m_PreviewSettings.showfilter, this->m_PreviewSettings.zoom);
        this->ReloadAsset();
    }
    else
        this->RefreshDrawing();
}


/*==============================
    Panel_ImgView::GetZoom
    Get the panel's zoom
    @return The Zoom value
==============================*/

wxRealPoint Panel_ImgView::GetZoom()
{
    return this->m_PreviewSettings.zoom;
}


/*==============================
    Panel_ImgView::ToggleAlphaDisplay
    Toggle the panel's alpha preview mode
==============================*/

void Panel_ImgView::ToggleAlphaDisplay()
{
    this->m_PreviewSettings.showalpha = !this->m_PreviewSettings.showalpha;
}


/*==============================
    Panel_ImgView::GetAlphaDisplay
    Get the panel's alpha display setting
    @return Whether to show alpha or not
==============================*/

bool Panel_ImgView::GetAlphaDisplay()
{
    return this->m_PreviewSettings.showalpha;
}


/*==============================
    Panel_ImgView::ToggleFilterDisplay
    Toggle the panel's filter preview
==============================*/

void Panel_ImgView::ToggleFilterDisplay()
{
    this->m_PreviewSettings.showfilter = !this->m_PreviewSettings.showfilter;
}


/*==============================
    Panel_ImgView::GetFilterDisplay
    Get the panel's filter preview status
    @return Whether to enable image filtering or not
==============================*/

bool Panel_ImgView::GetFilterDisplay()
{
    return this->m_PreviewSettings.showfilter;
}


/*==============================
    Panel_ImgView::ToggleStatisticsDisplay
    Toggle the panel's filter preview
==============================*/

void Panel_ImgView::ToggleStatisticsDisplay()
{
    this->m_PreviewSettings.showstats = !this->m_PreviewSettings.showstats;
}


/*==============================
    Panel_ImgView::ReloadAsset
    Reload the asset's preview bitmap
==============================*/

void Panel_ImgView::ReloadAsset()
{
    if (this->m_LoadedAsset == NULL || !this->m_LoadedAsset->m_ImageFinal.IsOk())
        this->m_Bitmap = Tex_Missing;
    else
        this->m_Bitmap = wxBitmap(this->m_LoadedAsset->m_ImageFinal);
    this->RefreshDrawing();
}


/*==============================
    Panel_ImgView::RefreshDrawing
    Force the preview panel to be redrawn
==============================*/

void Panel_ImgView::RefreshDrawing()
{
    int w, h;
    if (this->m_LoadedAsset == NULL || !this->m_Bitmap.IsOk())
        return;
    w = this->m_Bitmap.GetWidth();
    h = this->m_Bitmap.GetHeight();
    if (!this->m_PreviewSettings.showfilter)
    {
        w *= this->m_PreviewSettings.zoom.x;
        h *= this->m_PreviewSettings.zoom.y;
    }
    this->SetVirtualSize(w, h);
    this->Layout();
    this->Refresh();
}


/*==============================
    Panel_ImgView::OnPaint
    Draw onto the panel a preview of the image
    @param The paint event
==============================*/

void Panel_ImgView::OnPaint(wxPaintEvent& event)
{
    int screen_w, screen_h;
    wxRealPoint zoom = this->m_PreviewSettings.zoom;
    
    // Prepare the drawing context
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME)));
    dc.Clear();
    dc.GetSize(&screen_w, &screen_h);

    // If we have a valid asset
    if (this->m_LoadedAsset != NULL && this->m_Bitmap.IsOk())
    {
        int i;
        int img_x, img_y;
        int canvas_x, canvas_y;
        float img_w, img_h;

        // Calculate its size and position on the DC
        img_w = this->m_Bitmap.GetWidth();
        img_h = this->m_Bitmap.GetHeight();
        if (this->m_PreviewSettings.showfilter)
        {
            img_x = ((screen_w - img_w)/2)/zoom.x;
            img_y = ((screen_h - img_h)/2)/zoom.y;
            canvas_x = img_x;
            canvas_y = img_y;
            img_x = round(img_x*zoom.x);
            img_y = round(img_y*zoom.y);
        }
        else
        {
            img_x = ((screen_w - img_w*zoom.x)/2)/zoom.x;
            img_y = ((screen_h - img_h*zoom.y)/2)/zoom.y;
            canvas_x = img_x;
            canvas_y = img_y;
            img_w = floor(img_w*zoom.x);
            img_h = floor(img_h*zoom.y);
        }
        if (img_w > screen_w)
        {
            img_x = 0;
            canvas_x = 0;
        }
        if (img_h > screen_h)
        {
            img_y = 0;
            canvas_y = 0;
        }
        canvas_x = round(canvas_x*zoom.x);
        canvas_y = round(canvas_y*zoom.y);

        // Draw the transparent background
        dc.SetUserScale(1, 1);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxColor(255, 255, 255, 128)));
        dc.DrawRectangle(wxRect(canvas_x, canvas_y, img_w, img_h));
        dc.SetBrush(wxBrush(wxColor(192, 192, 192, 255)));
        i=0;
        while (true)
        {
            const float GRIDSIZE = 16;
            const int xcheck_total = ceilf(((float)img_w)/GRIDSIZE);
            const int ycheck_total = ceilf(((float)img_h)/GRIDSIZE);
            const float xcheck_total_frac = xcheck_total - (img_w/GRIDSIZE);
            const float ycheck_total_frac = ycheck_total - (img_h/GRIDSIZE);
            const int xcheck_curr = (i%xcheck_total);
            const int ycheck_curr = (i/xcheck_total);
            float w = ((xcheck_curr + 1) < xcheck_total) ? GRIDSIZE : GRIDSIZE*(1 - xcheck_total_frac);
            float h = ((ycheck_curr + 1) < ycheck_total) ? GRIDSIZE : GRIDSIZE*(1 - ycheck_total_frac);
            dc.DrawRectangle(wxRect(canvas_x+xcheck_curr*GRIDSIZE, canvas_y+ycheck_curr*GRIDSIZE, w, h));
            i += 2;
            if ((xcheck_curr + 2) >= xcheck_total)
            {
                if ((ycheck_curr + 1) >= ycheck_total)
                    break;
                i = (ycheck_curr + 1)*xcheck_total;
                if (ycheck_curr % 2 == 0)
                    i++;
            }
        }

        // Draw the asset itself
        if (this->m_PreviewSettings.showfilter)
            dc.SetUserScale(1.0, 1.0);
        else
            dc.SetUserScale(zoom.x, zoom.y);
        dc.DrawBitmap(this->m_Bitmap, img_x, img_y, false);
    }

    if (this->m_PreviewSettings.showstats)
    {
        wxString str;
        bool validasset = this->m_LoadedAsset != NULL && this->m_LoadedAsset->m_Image.IsOk();
        uint32_t y = screen_h;
        dc.SetBackgroundMode(wxPENSTYLE_SOLID);
        dc.SetTextBackground(wxColor(0, 0, 0, 128));
        dc.SetUserScale(1, 1);

        // TMEM Load count
        if (validasset)
        {
            if (this->m_LoadedAsset->m_FinalTexelCount != 0)
                dc.SetTextForeground(wxColor(255, 255, 255));
            else
                dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString::Format("TMEM Loads: %d", (int)ceilf(((float)this->m_LoadedAsset->m_FinalTexelCount)/4096)); // TODO: Make this more accurate once you implement it properly on the N64
        }
        else
        {
            dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString("TMEM Loads: 0");
        }
        y -= GetTextExtent(str).y;
        dc.DrawText(str, wxPoint(0, y));

        // Texel count
        if (validasset)
        {
            if (this->m_LoadedAsset->m_FinalTexelCount != 0 && this->m_LoadedAsset->m_FinalTexelCount <= 4096)
                dc.SetTextForeground(wxColor(255, 255, 255));
            else
                dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString::Format("Texel count: %d", this->m_LoadedAsset->m_FinalTexelCount);
        }
        else
        {
            dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString("Texel count: 0");
        }
        y -= GetTextExtent(str).y;
        dc.DrawText(str, wxPoint(0, y));

        // Image size
        if (validasset)
        {
            if (this->m_LoadedAsset->m_FinalSize.x != 0 && this->m_LoadedAsset->m_FinalSize.y != 0)
                dc.SetTextForeground(wxColor(255, 255, 255));
            else
                dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString::Format("Size: %dx%d", this->m_LoadedAsset->m_FinalSize.x, this->m_LoadedAsset->m_FinalSize.y);
        }
        else
        {
            dc.SetTextForeground(wxColor(255, 0, 0));
            str = wxString("Size: 0x0");
        }
        y -= GetTextExtent(str).y;
        dc.DrawText(str, wxPoint(0, y));
    }

    event.Skip();
}