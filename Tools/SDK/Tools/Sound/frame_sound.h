#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/frame.h>
#include "../panel_search.h"


/*=============================================================
                            Classes
=============================================================*/

class Frame_SoundBrowser : public wxFrame
{
    private:
        wxSplitterWindow* m_Splitter_Vertical;
        Panel_Search* m_Panel_Search;
        wxPanel* m_Panel_Edit;

        void m_Splitter_VerticalOnIdle(wxIdleEvent&);
        void m_Splitter_HorizontalOnIdle(wxIdleEvent&);

    protected:
        wxSplitterWindow* m_Splitter_Horizontal;
        wxPanel* m_Panel_Waveform;
        wxPanel* m_Panel_Config;
        wxFilePickerCtrl* m_FilePicker_Source;
        wxChoice* m_Choice_SampleRate;

	public:
		Frame_SoundBrowser(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 480), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
		~Frame_SoundBrowser();
};