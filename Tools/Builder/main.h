#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treelist.h>
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
#include <wx/choice.h>
#include <wx/log.h>
#include <map>

extern bool global_modifieddebug;

class Main : public wxFrame
{
	private:

	protected:
		wxChoice*	    m_Choice_BuildMode;
		wxImageList*    m_treeIcons;
		wxTreeListCtrl* m_TreeCtrl_ProjectDir;
		wxButton*       m_Button_Disassemble;
		wxButton*       m_Button_Clean;
		wxButton*       m_Button_Build;
		wxButton*       m_Button_Upload;
		wxMenuBar*      m_MenuBar;
		wxMenu   *      m_Menu_File;
		wxMenu*         m_Menu_Build;
		wxMenu*         m_Menu_Settings; 
		wxLogWindow*    m_LogWin;
		std::map<wxTreeListItem, CompUnit*>* m_CompUnits;

		void m_TreeCtrl_ProjectDir_OnActivated(wxTreeListEvent& event);
		void m_Button_Disassemble_OnButtonClick(wxCommandEvent& event);
		void m_Button_Clean_OnButtonClick(wxCommandEvent& event);
		void m_Button_Build_OnButtonClick(wxCommandEvent& event);
		void m_Button_Upload_OnButtonClick(wxCommandEvent& event);
		void m_MenuItem_Open_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Refresh_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Exit_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Build_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Clean_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Disassemble_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Upload_OnSelection(wxCommandEvent& event);
		void m_MenuItem_ForceRebuild_OnSelection(wxCommandEvent& event);
		void m_MenuItem_Config_OnSelection(wxCommandEvent& event);
		void PopulateCompileChoices();
		bool CheckDebugEnabled();
		void ModifyDebugH();
		void FixDebugH();
		void CleanProject();
		void BuildProject();
		void BuildROM();
		void DisassembleROM();
		void RegisterROM(wxString target);
		void MoveROM(wxString target);
		void UploadROM();

	public:
		void RefreshProjectTree();
		Main();
		~Main();
};