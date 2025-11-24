/***************************************************************
                       panel_sndview.cpp

A custom panel that allows you to preview an audio waveform
***************************************************************/

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
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x*ZOOM_SPEED, 1.0f);
    this->RefreshDrawing();
}


/*==============================
    Panel_SndView::ZoomOut
    Zoom the panel out
==============================*/

void Panel_SndView::ZoomOut()
{
    this->m_PreviewSettings.zoom = wxRealPoint(this->m_PreviewSettings.zoom.x/ZOOM_SPEED, 1.0f);
    if (this->m_PreviewSettings.zoom.x < 1.0f)
        this->m_PreviewSettings.zoom.x = 1.0f;
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
    int w;
    if (this->m_LoadedAsset == NULL || !this->m_AudioFile.IsOk())
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
    int vis_start, vis_end;
    int scrollsize;

    // Prepare the drawing context
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
    dc.Clear();
    this->SetVirtualSize(this->GetSize().x*this->m_PreviewSettings.zoom.x, this->GetSize().y);
    this->GetVirtualSize(&screen_w, &screen_h);

    // Get the visible portion of the DC
    this->GetScrollPixelsPerUnit(&scrollsize, NULL);
    vis_start = this->GetViewStart().x*scrollsize;
    vis_end = vis_start + this->GetClientSize().x;

    // Draw the waveforms
    if (this->m_LoadedAsset != NULL && af->IsOk())
    {
        int channel_h = screen_h/af->m_Channels;
        double samplesppx = (af->m_TotalSamples/screen_w)/this->m_PreviewSettings.zoom.x;

        for (int ch=0; ch<af->m_Channels; ch++)
        {
            int channel_mid = (channel_h*(ch+1)) - (channel_h/2);

            // Render the waveform
            dc.SetPen(wxPen(wxColour(0, 255, 0), 1, wxPENSTYLE_SOLID));
            if (samplesppx > 1)
            {
                for (uint64_t x=vis_start; x<vis_end; x++)
                {
                    uint64_t sampletime = ((((double)x)/((double)screen_w))*af->m_TotalSamples);
                    std::pair<double, double> sampley = af->GetAvgMinMaxSampleAtTime(sampletime, samplesppx, ch+1);
                    dc.DrawLine(x, channel_mid - (sampley.first*(channel_h/2)), x, channel_mid - (sampley.second*(channel_h/2)));
                }
            }
            else
            {
                uint64_t lastx = 0;
                double lastvalue = 0;
                uint64_t lastsample = UINT64_MAX;
                for (uint64_t x=vis_start; x<vis_end; x++)
                {
                    uint64_t sampletime = ((((double)x)/((double)screen_w))*af->m_TotalSamples);
                    if (sampletime != lastsample)
                    {
                        double value = af->GetSampleAtTime(sampletime, ch+1);
                        dc.DrawLine(x - (x - lastx), channel_mid - (lastvalue*(channel_h/2)), x, channel_mid - (value*(channel_h/2)));
                        lastx = x;
                        lastsample = sampletime;
                        lastvalue = value;
                    }
                }
            }

            // Render the center line
            dc.SetPen(wxPen(wxColour(255, 0, 0), 1, wxPENSTYLE_SOLID));
            dc.DrawLine(0, channel_mid, screen_w, channel_mid);
        }
    }

    // Skip the event to prevent problems
    event.Skip();
}