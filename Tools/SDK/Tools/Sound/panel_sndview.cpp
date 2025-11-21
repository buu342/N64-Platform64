#include "panel_sndview.h"
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
    Panel_SndView (Constructor)
    Initializes the class
    @param The parent window
    @param The panel's ID (default wxID_ANY)
    @param The panel's position (default wxDefaultPosition)
    @param The panel's size (default wxDefaultSize)
    @param The panel's style (default wxScrolledWindowStyle)
==============================*/

Panel_SndView::Panel_SndView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle) : wxScrolledWindow(parent, id, pos, size, style)
{
    this->m_LoadedAsset = NULL;
    this->SetDefaultSettings();
    this->m_AudioFile = AudioFile();

    // Connect events
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(Panel_SndView::OnPaint), NULL, this);
}


/*==============================
    Panel_SndView (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_SndView::~Panel_SndView()
{
    // Nothing to cleanup
}


/*==============================
    Panel_SndView::SetDefaultSettings
    Set the default preview settings
==============================*/

void Panel_SndView::SetDefaultSettings()
{
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
}


/*==============================
    Panel_SndView::SetAsset
    Set the asset to preview
    @param The asset to preview
==============================*/

void Panel_SndView::SetAsset(P64Asset_Sound* asset)
{
    this->m_LoadedAsset = asset;
    this->ReloadAsset();
}


/*==============================
    Panel_SndView::ZoomIn
    Zoom the panel in
==============================*/

void Panel_SndView::ZoomIn()
{
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x*ZOOM_SPEED, this->m_PreviewSettings.zoom.y*ZOOM_SPEED);
    this->RefreshDrawing();
}


/*==============================
    Panel_SndView::ZoomOut
    Zoom the panel out
==============================*/

void Panel_SndView::ZoomOut()
{
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x/ZOOM_SPEED, this->m_PreviewSettings.zoom.y/ZOOM_SPEED);
    this->RefreshDrawing();
}


/*==============================
    Panel_SndView::ZoomReset
    Reset the panel zoom
==============================*/

void Panel_SndView::ZoomReset()
{
    this->m_PreviewSettings.zoom = wxRealPoint(1.0, 1.0);
    this->RefreshDrawing();
}


/*==============================
    Panel_SndView::GetZoom
    Get the panel's zoom
    @return The Zoom value
==============================*/

wxRealPoint Panel_SndView::GetZoom()
{
    return this->m_PreviewSettings.zoom;
}

/*==============================
    Panel_SndView::ReloadAsset
    Reload the asset's preview bitmap
==============================*/

void Panel_SndView::ReloadAsset()
{
    if (this->m_LoadedAsset != NULL)
        this->m_AudioFile = this->m_LoadedAsset->m_SndFile;
    this->RefreshDrawing();
}


/*==============================
    Panel_SndView::RefreshDrawing
    Force the preview panel to be redrawn
==============================*/

void Panel_SndView::RefreshDrawing()
{
    if (this->m_LoadedAsset == NULL)
        return;
    this->Layout();
    this->Refresh();
}


/*==============================
    Panel_SndView::OnPaint
    Draw onto the panel a preview of the image
    @param The paint event
==============================*/

void Panel_SndView::OnPaint(wxPaintEvent& event)
{
    AudioFile* af = &this->m_AudioFile;
    int screen_w, screen_h;

    // Prepare the drawing context
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
    dc.Clear();
    dc.GetSize(&screen_w, &screen_h);

    // Draw the waveforms
    if (this->m_LoadedAsset != NULL && af->IsOk())
    {
        int channel_h = screen_h/af->m_Channels;
        for (int ch=0; ch<af->m_Channels; ch++)
        {
            int channel_mid = (channel_h*(ch+1)) - (channel_h/2);

            // Render the waveform
            dc.SetPen(wxPen(wxColour(0, 255, 0), 1, wxPENSTYLE_SOLID));
            for (int x=0; x<screen_w; x++)
            {
                uint64_t sampletime = ((((double)x)/((double)screen_w))*af->m_TotalSamples);
                uint64_t samplesppx = af->m_TotalSamples/screen_w;
                std::pair<double, double> sampley = af->GetAvgMinMaxSampleAtTime(sampletime, samplesppx, ch+1);
                dc.DrawLine(x, channel_mid - (sampley.first*(channel_h/2)), x, channel_mid - (sampley.second*(channel_h/2)));
            }

            // Render the center line
            dc.SetPen(wxPen(wxColour(255, 0, 0), 1, wxPENSTYLE_SOLID));
            dc.DrawLine(0, channel_mid, screen_w, channel_mid);
        }
    }

    event.Skip();
}