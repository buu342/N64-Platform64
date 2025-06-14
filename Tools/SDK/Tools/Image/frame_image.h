#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include "../panel_search.h"
#include "panel_imgview.h"

class Frame_ImageBrowser : public wxFrame
{
	private:

	protected:
		Panel_ImgView* m_ScrolledWin_Preview;
		wxSplitterWindow* m_Splitter_Vertical;
		Panel_Search* m_Panel_Search;
		wxPanel* m_Panel_Edit;
		wxSplitterWindow* m_Splitter_Horizontal;
		wxPanel* m_Panel_Preview;
		wxPanel* m_Panel_Config;

	public:
		Frame_ImageBrowser( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~Frame_ImageBrowser();

		void m_Splitter_VerticalOnIdle( wxIdleEvent& )
		{
			m_Splitter_Vertical->SetSashPosition( 0 );
			m_Splitter_Vertical->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_VerticalOnIdle ), NULL, this );
		}

		void m_Splitter_HorizontalOnIdle( wxIdleEvent& )
		{
			m_Splitter_Horizontal->SetSashPosition( 0 );
			m_Splitter_Horizontal->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_HorizontalOnIdle ), NULL, this );
		}

};

