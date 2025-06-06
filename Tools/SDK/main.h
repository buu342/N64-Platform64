#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/dataview.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/dir.h>


/*********************************
             Classes
*********************************/

class Frame_Main : public wxFrame
{
    private:
        wxDataViewListCtrl* m_DataViewListCtrl_Main;
        wxDir m_ProjectPath;
        void m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated(wxDataViewEvent& event);

    protected:

    public:
        Frame_Main();
        ~Frame_Main();
        void OpenProject();
};