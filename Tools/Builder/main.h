#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/frame.h>

#define PROGRAM_NAME "Builder"

class Main : public wxFrame
{
	private:

	protected:
		wxImageList* m_treeIcons;
		wxTreeCtrl* m_TreeCtrl_ProjectDir;
		wxButton* m_Button_Disassemble;
		wxButton* m_Button_Clean;
		wxButton* m_Button_Build;
		void m_Button_Disassemble_OnButtonClick(wxCommandEvent& event);
		void m_Button_Clean_OnButtonClick(wxCommandEvent& event);
		void m_Button_Build_OnButtonClick(wxCommandEvent& event);
		void RefreshProjectTree();

	public:
		Main();
		~Main();
};
