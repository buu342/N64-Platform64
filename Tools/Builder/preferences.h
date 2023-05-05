#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/frame.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

// Default project config
#define DEFAULT_PROJECTPATH        (wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath())
#define DEFAULT_ROMNAME            "homebrew.n64"
#define DEFAULT_BUILDPATH          (global_projectconfig.ProjectPath+"/build")
#define DEFAULT_ROMHEADER_NAME     "MY HOMEBREW GAME"
#define DEFAULT_ROMHEADER_MANUF    "N"
#define DEFAULT_ROMHEADER_ID       "HB"
#define DEFAULT_ROMHEADER_COUNTRY  "E"
#define DEFAULT_GCCFLAGS           (wxString("-Wall ") \
									+"-I. " \
									+"-I"+global_programconfig.Path_Libultra+"/usr/include/PR " \
									+"-I"+global_programconfig.Path_Libultra+"/usr/include " \
									+"-G 0 -c -I"+global_programconfig.Path_Libultra+"/usr/include " \
								    +"-DF3DEX_GBI_2 -DNOT_SPEC")
#define DEFAULT_LDFLAGS            (wxString("-L. ") \
									+"-L"+global_programconfig.Path_Libultra+"/usr/lib " \
									+"-L"+global_programconfig.Path_Libultra+"/usr/lib/PR " \
									+"-L"+global_programconfig.Path_Libultra+"/gcc/mipse/lib -lkmc")
#define DEFAULT_MILDFLAGS          ""
 
// Default program config
#define DEFAULT_LIBULTRAPATH  "C:/ultra"
#define DEFAULT_TOOLKITPATH   "C:/ultra/GCC/MIPSE/BIN"
#define DEFAULT_MOVEPATH      "Z:"
#define DEFAULT_DISASSEMBLY   "disassembly.txt"
#define DEFAULT_USEEXEW32     true
#define DEFAULT_SEPARATEDEBUG true
#define DEFAULT_USEBUILD      true
#define DEFAULT_USENRDC       true
#define DEFAULT_USEMAKEMASK   true
#define DEFAULT_USEMOVE       false
#define DEFAULT_USEUPLOAD     false
#define DEFAULT_PROMPTMOVE    true
#define DEFAULT_PROMPTUPLOAD  true
#define DEFAULT_PROMPTDISASS  false
#define DEFAULT_PROMPTCLEAN   true

typedef struct
{
	wxString ProjectPath;
	wxString TargetName;
	wxString BuildFolder;
	wxString ROMHeader_Name;
	wxString ROMHeader_Manufacturer;
	wxString ROMHeader_ID;
	wxString ROMHeader_Country;
	wxString Flags_GCC;
	wxString Flags_LD;
	wxString Flags_MILD;
} ConfigProject;

typedef struct
{
	wxString Path_Libultra;
	wxString Path_Toolkit;
	wxString Path_Move;
	wxString DissamblyName;
	bool     Use_EXEW32;
	bool     SeparateDebug;
	bool     Use_Build;
	bool     Use_NRDC;
	bool     Use_MakeMask;
	bool     Use_Move;
	bool     Use_Upload;
	bool     Prompt_Move;
	bool     Prompt_Upload;
	bool     Prompt_Disassembly;
	bool     Prompt_Clean;
} ConfigProgram;

extern wxFileConfig* global_configfile;
extern ConfigProject global_projectconfig;
extern ConfigProgram global_programconfig;

void Config_SaveProgramConfig();
void Config_LoadProgramConfig();
void Config_DefaultProgramConfig();

void Config_SaveProjectConfig();
void Config_LoadProjectConfig();
void Config_DefaultProjectConfig();

class Preferences : public wxFrame
{
	private:

	protected:
		wxWindowDisabler* m_Disabler;
		wxStaticText*     m_Text_TargetName;
		wxTextCtrl*       m_TextCtrl_TargetName;
		wxStaticText*     m_Text_BuildFolder;
		wxTextCtrl*       m_TextCtrl_BuildFolder;
		wxStaticText*     m_Text_ROMHeader;
		wxTextCtrl*       m_TextCtrl_ROMHeader_Title;
		wxTextCtrl*       m_TextCtrl_ROMHeader_Manufacturer;
		wxTextCtrl*       m_TextCtrl_ROMHeader_ID;
		wxTextCtrl*       m_TextCtrl_ROMHeader_Country;
		wxStaticText*     m_Text_GCCFlags;
		wxTextCtrl*       m_TextCtrl_GCCFlags;
		wxStaticText*     m_Text_LDFlags;
		wxTextCtrl*       m_TextCtrl_LDFlags;
		wxStaticText*     m_Text_MILDFlags;
		wxTextCtrl*       m_TextCtrl_MILDFlags;
		wxStaticLine*     m_StaticLine;
		wxStaticText*     m_Text_LibultraPath;
		wxTextCtrl*       m_TextCtrl_LibultraPath;
		wxStaticText*     m_Text_ToolkitPath;
		wxTextCtrl*       m_TextCtrl_ToolkitPath;
		wxStaticText*     m_Text_MoveFolder;
		wxTextCtrl*       m_TextCtrl_MoveFolder;
		wxStaticText*     m_Text_DisassemblyName;
		wxTextCtrl*       m_TextCtrl_DisassemblyName;
		wxCheckBox*       m_CheckBox_UseEXEW32;
		wxCheckBox*       m_CheckBox_SeparateDebug;
		wxCheckBox*       m_CheckBox_UseBuild;
		wxCheckBox*       m_CheckBox_UseNRDC;
		wxCheckBox*       m_CheckBox_UseMakemask;
		wxCheckBox*       m_CheckBox_MoveROM;
		wxCheckBox*       m_CheckBox_PromptMoving;
		wxCheckBox*       m_CheckBox_UploadEnabled;
		wxCheckBox*       m_CheckBox_PromptUpload;
		wxCheckBox*       m_CheckBox_PromptDisassembly;
		wxCheckBox*       m_CheckBox_PromptClean;
		wxButton*         m_Button_Close;

		void m_TextCtrl_TargetName_OnText(wxCommandEvent& event);
		void m_TextCtrl_BuildFolder_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Title_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Manufacturer_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_ID_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Country_OnText(wxCommandEvent& event);
		void m_TextCtrl_GCCFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_LDFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_MILDFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_LibultraPath_OnText(wxCommandEvent& event);
		void m_TextCtrl_Toolkit_OnText(wxCommandEvent& event);
		void m_TextCtrl_MoveFolder_OnText(wxCommandEvent& event);
		void m_TextCtrl_DisassemblyName_OnText(wxCommandEvent& event);
		void m_CheckBox_UseEXEW32_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_SeparateDebug_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_UseBuild_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_UseNRDC_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_UseMakemask_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_MoveROM_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_PromptMoving_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_UploadEnabled_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_PromptUpload_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_PromptDisassembly_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_PromptClean_OnCheckBox(wxCommandEvent& event);
		void m_Button_Close_OnButtonClick(wxCommandEvent& event);

	public:
		Preferences(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(550, 550), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxRESIZE_BORDER | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL);
		~Preferences();
};