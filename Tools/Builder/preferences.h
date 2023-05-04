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

class Preferences : public wxFrame
{
	private:

	protected:
		wxWindowDisabler* m_Disabler;
		wxStaticText* m_Text_TargetName;
		wxTextCtrl*   m_TextCtrl_TargetName;
		wxStaticText* m_Text_OutputFolder;
		wxTextCtrl*   m_TextCtrl_OutputFolder;
		wxStaticText* m_Text_ROMHeader;
		wxTextCtrl*   m_TextCtrl_ROMHeader_Title;
		wxTextCtrl*   m_TextCtrl_ROMHeader_Manufacturer;
		wxTextCtrl*   m_TextCtrl_ROMHeader_ID;
		wxTextCtrl*   m_TextCtrl_ROMHeader_Country;
		wxStaticText* m_Text_GCCFlags;
		wxTextCtrl*   m_TextCtrl_GCCFlags;
		wxStaticText* m_Text_LDFlags;
		wxTextCtrl*   m_TextCtrl_LDFlags;
		wxStaticText* m_Text_MILDFlags;
		wxTextCtrl*   m_TextCtrl_MILDFlags;
		wxStaticLine* m_StaticLine;
		wxStaticText* m_Text_LibultraPath;
		wxTextCtrl*   m_TextCtrl_LibultraPath;
		wxStaticText* m_Text_MoveFolder;
		wxTextCtrl*   m_TextCtrl_MoveFolder;
		wxStaticText* m_Text_DisassemblyName;
		wxTextCtrl*   m_TextCtrl_DisassemblyName;
		wxCheckBox*   m_CheckBox_UseEXEW32;
		wxCheckBox*   m_CheckBox_LeaveObjects;
		wxCheckBox*   m_CheckBox_UseNRDC;
		wxCheckBox*   m_CheckBox_UseMakemask;
		wxCheckBox*   m_CheckBox_MoveROM;
		wxCheckBox*   m_CheckBox_PromptMoving;
		wxCheckBox*   m_CheckBox_UploadEnabled;
		wxCheckBox*   m_CheckBox_PromptUpload;
		wxCheckBox*   m_CheckBox_PromptDisassembly;
		wxCheckBox*   m_CheckBox_PromptClean;
		wxButton*     m_Button_Close;

		void m_TextCtrl_TargetName_OnText(wxCommandEvent& event);
		void m_TextCtrl_OutputFolder_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Title_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Manufacturer_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_ID_OnText(wxCommandEvent& event);
		void m_TextCtrl_ROMHeader_Country_OnText(wxCommandEvent& event);
		void m_TextCtrl_GCCFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_LDFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_MILDFlags_OnText(wxCommandEvent& event);
		void m_TextCtrl_LibultraPath_OnText(wxCommandEvent& event);
		void m_TextCtrl_MoveFolder_OnText(wxCommandEvent& event);
		void m_TextCtrl_DisassemblyName_OnText(wxCommandEvent& event);
		void m_CheckBox_UseEXEW32_OnCheckBox(wxCommandEvent& event);
		void m_CheckBox_LeaveObjects_OnCheckBox(wxCommandEvent& event);
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
		Preferences(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(550, 500), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxRESIZE_BORDER | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL);
		~Preferences();

};