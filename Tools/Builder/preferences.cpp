#include "preferences.h"

Preferences::Preferences(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->m_Disabler = new wxWindowDisabler(this);
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

	// Setup the main sizer
	wxFlexGridSizer* m_Sizer_Main;
	m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
	m_Sizer_Main->AddGrowableCol(0);
	m_Sizer_Main->AddGrowableRow(0);
	m_Sizer_Main->AddGrowableRow(2);
	m_Sizer_Main->AddGrowableRow(3);
	m_Sizer_Main->SetFlexibleDirection(wxBOTH);
	m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	// Set up project setting sizer
	wxFlexGridSizer* m_Sizer_ProjectStuff;
	m_Sizer_ProjectStuff = new wxFlexGridSizer(0, 2, 0, 0);
	m_Sizer_ProjectStuff->AddGrowableCol(1);
	m_Sizer_ProjectStuff->SetFlexibleDirection(wxBOTH);
	m_Sizer_ProjectStuff->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	// Project settings
	this->m_Text_TargetName = new wxStaticText(this, wxID_ANY, wxT("Target name:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_TargetName->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_TargetName, 0, wxALL, 5);
	this->m_TextCtrl_TargetName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_TargetName, 0, wxALL | wxEXPAND, 5);
	this->m_Text_OutputFolder = new wxStaticText(this, wxID_ANY, wxT("Output folder:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_OutputFolder->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_OutputFolder, 0, wxALL, 5);
	this->m_TextCtrl_OutputFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_OutputFolder, 0, wxALL | wxEXPAND, 5);
	this->m_Text_ROMHeader = new wxStaticText(this, wxID_ANY, wxT("ROM header:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_ROMHeader->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_ROMHeader, 0, wxALL, 5);

	// Project settings (NRDC section)
	wxFlexGridSizer* m_Sizer_ROMHeader;
	m_Sizer_ROMHeader = new wxFlexGridSizer(0, 4, 0, 0);
	m_Sizer_ROMHeader->AddGrowableCol(0);
	m_Sizer_ROMHeader->SetFlexibleDirection(wxBOTH);
	m_Sizer_ROMHeader->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	this->m_TextCtrl_ROMHeader_Title = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Title, 0, wxALL | wxEXPAND, 5);
	this->m_TextCtrl_ROMHeader_Manufacturer = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_Manufacturer->SetMaxSize(wxSize(24, -1));
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Manufacturer, 0, wxALL, 5);
	this->m_TextCtrl_ROMHeader_ID = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_ID->SetMinSize(wxSize(36, -1));
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_ID, 0, wxALL, 5);
	this->m_TextCtrl_ROMHeader_Country = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_Country->SetMaxLength(1);
	this->m_TextCtrl_ROMHeader_Country->SetMaxSize(wxSize(24, -1));
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Country, 0, wxALL, 5);
	m_Sizer_ProjectStuff->Add(m_Sizer_ROMHeader, 1, wxEXPAND, 5);

	// Project settings (Program flags)
	this->m_Text_GCCFlags = new wxStaticText(this, wxID_ANY, wxT("GCC flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_GCCFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_GCCFlags, 0, wxALL, 5);
	this->m_TextCtrl_GCCFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_GCCFlags, 0, wxALL | wxEXPAND, 5);
	this->m_Text_LDFlags = new wxStaticText(this, wxID_ANY, wxT("LD flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_LDFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_LDFlags, 0, wxALL, 5);
	this->m_TextCtrl_LDFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_LDFlags, 0, wxALL | wxEXPAND, 5);
	this->m_Text_MILDFlags = new wxStaticText(this, wxID_ANY, wxT("MakeROM flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_MILDFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_MILDFlags, 0, wxALL, 5);
	this->m_TextCtrl_MILDFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_MILDFlags, 0, wxALL | wxEXPAND, 5);
	m_Sizer_Main->Add(m_Sizer_ProjectStuff, 1, wxEXPAND, 5);
	this->m_StaticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_Sizer_Main->Add(this->m_StaticLine, 0, wxEXPAND | wxALL, 5);

	// Program stuff sizer
	wxFlexGridSizer* m_Sizer_ProgramStuff;
	m_Sizer_ProgramStuff = new wxFlexGridSizer(0, 2, 0, 0);
	m_Sizer_ProgramStuff->AddGrowableCol(1);
	m_Sizer_ProgramStuff->SetFlexibleDirection(wxBOTH);
	m_Sizer_ProgramStuff->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	// Program stuff paths
	this->m_Text_LibultraPath = new wxStaticText(this, wxID_ANY, wxT("Libultra path:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_LibultraPath->Wrap(-1);
	m_Sizer_ProgramStuff->Add(this->m_Text_LibultraPath, 0, wxALL, 5);
	this->m_TextCtrl_LibultraPath = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_LibultraPath, 0, wxALL | wxEXPAND, 5);
	this->m_Text_MoveFolder = new wxStaticText(this, wxID_ANY, wxT("Move folder:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_MoveFolder->Wrap(-1);
	m_Sizer_ProgramStuff->Add(this->m_Text_MoveFolder, 0, wxALL, 5);
	this->m_TextCtrl_MoveFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_MoveFolder, 0, wxALL | wxEXPAND, 5);
	this->m_Text_DisassemblyName = new wxStaticText(this, wxID_ANY, wxT("Disassembly name:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_DisassemblyName->Wrap(-1);
	m_Sizer_ProgramStuff->Add(this->m_Text_DisassemblyName, 0, wxALL, 5);
	this->m_TextCtrl_DisassemblyName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_DisassemblyName, 0, wxALL | wxEXPAND, 5);
	m_Sizer_Main->Add(m_Sizer_ProgramStuff, 1, wxEXPAND, 5);

	// Checkboxes section
	wxGridSizer* m_Sizer_Checkboxes;
	m_Sizer_Checkboxes = new wxGridSizer(0, 3, 0, 0);
	this->m_CheckBox_UseEXEW32 = new wxCheckBox(this, wxID_ANY, wxT("Use EXEW32"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseEXEW32, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_LeaveObjects = new wxCheckBox(this, wxID_ANY, wxT("Leave objects next to source"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_LeaveObjects, 0, wxALL, 5);
	this->m_CheckBox_UseNRDC = new wxCheckBox(this, wxID_ANY, wxT("Register ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseNRDC, 0, wxALL, 5);
	this->m_CheckBox_UseMakemask = new wxCheckBox(this, wxID_ANY, wxT("Makemask ROM"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseMakemask, 0, wxALL, 5);
	this->m_CheckBox_MoveROM = new wxCheckBox(this, wxID_ANY, wxT("Move ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_MoveROM, 0, wxALL, 5);
	this->m_CheckBox_PromptMoving = new wxCheckBox(this, wxID_ANY, wxT("Prompt for moving"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptMoving, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_UploadEnabled = new wxCheckBox(this, wxID_ANY, wxT("Upload ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UploadEnabled, 0, wxALL, 5);
	this->m_CheckBox_PromptUpload = new wxCheckBox(this, wxID_ANY, wxT("Prompt for uploading"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptUpload, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_PromptDisassembly = new wxCheckBox(this, wxID_ANY, wxT("Prompt for disassembly"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptDisassembly, 0, wxALL, 5);
	this->m_CheckBox_PromptClean = new wxCheckBox(this, wxID_ANY, wxT("Prompt for cleaning"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptClean, 0, wxALL, 5);
	m_Sizer_Main->Add(m_Sizer_Checkboxes, 1, wxEXPAND, 5);
	this->m_Button_Close = new wxButton(this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Main->Add(this->m_Button_Close, 0, wxALIGN_CENTER | wxALL, 5);

	// Set the page layout
	this->SetSizer(m_Sizer_Main);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect events
	m_TextCtrl_TargetName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_TargetName_OnText), NULL, this);
	m_TextCtrl_OutputFolder->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_OutputFolder_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Title->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Title_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Manufacturer->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText), NULL, this);
	m_TextCtrl_ROMHeader_ID->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_ID_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Country->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Country_OnText), NULL, this);
	m_TextCtrl_GCCFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_GCCFlags_OnText), NULL, this);
	m_TextCtrl_LDFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LDFlags_OnText), NULL, this);
	m_TextCtrl_MILDFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MILDFlags_OnText), NULL, this);
	m_TextCtrl_LibultraPath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LibultraPath_OnText), NULL, this);
	m_TextCtrl_MoveFolder->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MoveFolder_OnText), NULL, this);
	m_TextCtrl_DisassemblyName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_DisassemblyName_OnText), NULL, this);
	m_CheckBox_UseEXEW32->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseEXEW32_OnCheckBox), NULL, this);
	m_CheckBox_LeaveObjects->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_LeaveObjects_OnCheckBox), NULL, this);
	m_CheckBox_UseNRDC->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseNRDC_OnCheckBox), NULL, this);
	m_CheckBox_UseMakemask->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseMakemask_OnCheckBox), NULL, this);
	m_CheckBox_MoveROM->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_MoveROM_OnCheckBox), NULL, this);
	m_CheckBox_PromptMoving->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptMoving_OnCheckBox), NULL, this);
	m_CheckBox_UploadEnabled->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UploadEnabled_OnCheckBox), NULL, this);
	m_CheckBox_PromptUpload->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptUpload_OnCheckBox), NULL, this);
	m_CheckBox_PromptDisassembly->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptDisassembly_OnCheckBox), NULL, this);
	m_CheckBox_PromptClean->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptClean_OnCheckBox), NULL, this);
	m_Button_Close->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Preferences::m_Button_Close_OnButtonClick), NULL, this);
}

Preferences::~Preferences()
{
	delete this->m_Disabler;
	this->GetParent()->SetFocus();

	// Disconnect events
	m_TextCtrl_TargetName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_TargetName_OnText), NULL, this);
	m_TextCtrl_OutputFolder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_OutputFolder_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Title->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Title_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Manufacturer->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText), NULL, this);
	m_TextCtrl_ROMHeader_ID->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_ID_OnText), NULL, this);
	m_TextCtrl_ROMHeader_Country->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Country_OnText), NULL, this);
	m_TextCtrl_GCCFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_GCCFlags_OnText), NULL, this);
	m_TextCtrl_LDFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LDFlags_OnText), NULL, this);
	m_TextCtrl_MILDFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MILDFlags_OnText), NULL, this);
	m_TextCtrl_LibultraPath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LibultraPath_OnText), NULL, this);
	m_TextCtrl_MoveFolder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MoveFolder_OnText), NULL, this);
	m_TextCtrl_DisassemblyName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_DisassemblyName_OnText), NULL, this);
	m_CheckBox_UseEXEW32->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseEXEW32_OnCheckBox), NULL, this);
	m_CheckBox_LeaveObjects->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_LeaveObjects_OnCheckBox), NULL, this);
	m_CheckBox_UseNRDC->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseNRDC_OnCheckBox), NULL, this);
	m_CheckBox_UseMakemask->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseMakemask_OnCheckBox), NULL, this);
	m_CheckBox_MoveROM->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_MoveROM_OnCheckBox), NULL, this);
	m_CheckBox_PromptMoving->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptMoving_OnCheckBox), NULL, this);
	m_CheckBox_UploadEnabled->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UploadEnabled_OnCheckBox), NULL, this);
	m_CheckBox_PromptUpload->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptUpload_OnCheckBox), NULL, this);
	m_CheckBox_PromptDisassembly->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptDisassembly_OnCheckBox), NULL, this);
	m_CheckBox_PromptClean->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptClean_OnCheckBox), NULL, this);
	m_Button_Close->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Preferences::m_Button_Close_OnButtonClick), NULL, this);
}

void Preferences::m_TextCtrl_TargetName_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_OutputFolder_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_ROMHeader_Title_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_ROMHeader_ID_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_ROMHeader_Country_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_GCCFlags_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_LDFlags_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_MILDFlags_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_LibultraPath_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_MoveFolder_OnText(wxCommandEvent& event)
{

}

void Preferences::m_TextCtrl_DisassemblyName_OnText(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_UseEXEW32_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_LeaveObjects_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_UseNRDC_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_UseMakemask_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_MoveROM_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_PromptMoving_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_UploadEnabled_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_PromptUpload_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_PromptDisassembly_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_CheckBox_PromptClean_OnCheckBox(wxCommandEvent& event)
{

}

void Preferences::m_Button_Close_OnButtonClick(wxCommandEvent& event)
{
	this->Close();
}