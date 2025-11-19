#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include "asset_sound.h"


/*=============================================================
                             Types
=============================================================*/

struct PreviewSettings_Audio {
    wxRealPoint zoom;
};


/*=============================================================
                            Classes
=============================================================*/

class Panel_SndView : public wxScrolledWindow
{
    private:
        AudioFile m_AudioFile;
        P64Asset_Sound* m_LoadedAsset;
        PreviewSettings_Audio m_PreviewSettings;

        void OnPaint(wxPaintEvent& event);
    
    protected:

    public:
        void ZoomIn();
        void ZoomOut();
        void ZoomReset();
        void SetAsset(P64Asset_Sound* asset);
        void SetDefaultSettings();
        void ReloadAsset();
        void RefreshDrawing();

        wxRealPoint GetZoom();
        
        Panel_SndView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_SndView();
};