#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
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
#include <wx/choice.h>
#include <wx/stdpaths.h>
#include <wx/log.h>
#include <map>

// Program defs
#define PROGRAM_NAME  "Builder"
#define PROJECTPATH   (wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath())

// Build system defs
#define BUILDFOLDER   "build"
#define OUTPUTPATH    PROJECTPATH + wxString("/") + wxString(BUILDFOLDER)
#define DISASSNAME    "disassembly.txt"
#define DISASSPATH    PROJECTPATH + "/" + DISASSNAME

// Project defs
#define TARGET        "platform.n64"
#define TARGETDEBUG   "platform_d.n64"
#define MOVEROMFOLDER "Z:"
#define REGISTERINFO  "\"PLATFORM64\" B PF I"

// Libultra paths
#define LIBULTRAPATH  "C:/ultra"
#define MIPSEPATH     "GCC/MIPSE/BIN"
#define MIPSEFULLPATH (LIBULTRAPATH + wxString("/") + MIPSEPATH)
#define CODESEGMENT   OUTPUTPATH + "/codesegment.o"
#define FINALROM      OUTPUTPATH + "/" + TARGET
#define FINALROM_D    OUTPUTPATH + "/" + TARGETDEBUG

// Icons
extern wxIcon   iconbm_prog;
extern wxBitmap iconbm_c;
extern wxBitmap iconbm_h;

class Main : public wxFrame
{
	private:

	protected:
		wxChoice*	 m_Choice_BuildMode;
		wxImageList* m_treeIcons;
		wxTreeCtrl*  m_TreeCtrl_ProjectDir;
		wxButton*    m_Button_Disassemble;
		wxButton*    m_Button_Clean;
		wxButton*    m_Button_Build;
		wxButton*    m_Button_Upload;
		wxMenuBar*   m_MenuBar;
		wxMenu   *   m_Menu_File;
		wxMenu*      m_Menu_Build;
		wxMenu*      m_Menu_Settings; 
		wxLogWindow* m_LogWin;
		std::map<wxTreeItemId, CompUnit*>* m_CompUnits;

		void m_Choice_BuildMode_OnChoice(wxCommandEvent& event);
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
		void RefreshProjectTree();
		bool CheckDebugEnabled();

	public:
		Main();
		~Main();
};