#pragma once

typedef struct IUnknown IUnknown;

#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include "asset_image.h"


/*=============================================================
                             Types
=============================================================*/


/*=============================================================
                            Classes
=============================================================*/

class Panel_ImgView : public wxScrolledWindow
{
    private:
        wxBitmap m_Bitmap;
        P64Asset_Image* m_LoadedAsset;
        PreviewSettings_Image m_PreviewSettings;

        void OnPaint(wxPaintEvent& event);
    
    protected:

    public:
        void ZoomIn();
        void ZoomOut();
        void ZoomReset();
        void ToggleAlphaDisplay();
        void ToggleAlphaMaskDisplay();
        void ToggleFilterDisplay();
        void ToggleStatisticsDisplay();
        void SetDefaultSettings();
        void SetAsset(P64Asset_Image* asset);
        void RefreshDrawing();
        
        ~Panel_ImgView();
        Panel_ImgView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
};