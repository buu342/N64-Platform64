#include "main.h"
#include "preferences.h"
#include "app.h"

wxFileConfig* global_configfile = NULL;
ConfigProject global_projectconfig;
ConfigProgram global_programconfig;

Preferences::Preferences(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->m_Disabler = new wxWindowDisabler(this);
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

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
	this->m_TextCtrl_TargetName->SetValue(global_projectconfig.TargetName);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_TargetName, 0, wxALL | wxEXPAND, 5);
	this->m_Text_BuildFolder = new wxStaticText(this, wxID_ANY, wxT("Build folder:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_BuildFolder->Wrap(-1);
	this->m_Text_BuildFolder->Enable(global_programconfig.Use_Build);
	m_Sizer_ProjectStuff->Add(this->m_Text_BuildFolder, 0, wxALL, 5);
	this->m_TextCtrl_BuildFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_BuildFolder->SetValue(global_projectconfig.BuildFolder);
	this->m_TextCtrl_BuildFolder->Enable(global_programconfig.Use_Build);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_BuildFolder, 0, wxALL | wxEXPAND, 5);
	this->m_Text_ROMHeader = new wxStaticText(this, wxID_ANY, wxT("ROM header:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_ROMHeader->Wrap(-1);
	this->m_Text_ROMHeader->Enable(global_programconfig.Use_NRDC);
	m_Sizer_ProjectStuff->Add(this->m_Text_ROMHeader, 0, wxALL, 5);

	// Project settings (NRDC section)
	wxFlexGridSizer* m_Sizer_ROMHeader;
	m_Sizer_ROMHeader = new wxFlexGridSizer(0, 4, 0, 0);
	m_Sizer_ROMHeader->AddGrowableCol(0);
	m_Sizer_ROMHeader->SetFlexibleDirection(wxBOTH);
	m_Sizer_ROMHeader->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	this->m_TextCtrl_ROMHeader_Title = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_Title->SetMaxLength(20);
	this->m_TextCtrl_ROMHeader_Title->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Title->SetValue(global_projectconfig.ROMHeader_Name);
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Title, 0, wxALL | wxEXPAND, 5);
	this->m_TextCtrl_ROMHeader_Manufacturer = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_Manufacturer->SetMaxLength(1);
	this->m_TextCtrl_ROMHeader_Manufacturer->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Manufacturer->SetMaxSize(wxSize(24, -1));
	this->m_TextCtrl_ROMHeader_Manufacturer->SetValue(global_projectconfig.ROMHeader_Manufacturer);
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Manufacturer, 0, wxALL, 5);
	this->m_TextCtrl_ROMHeader_ID = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_ID->SetMaxLength(2);
	this->m_TextCtrl_ROMHeader_ID->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_ID->SetMinSize(wxSize(36, -1));
	this->m_TextCtrl_ROMHeader_ID->SetValue(global_projectconfig.ROMHeader_ID);
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_ID, 0, wxALL, 5);
	this->m_TextCtrl_ROMHeader_Country = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ROMHeader_Country->SetMaxLength(1);
	this->m_TextCtrl_ROMHeader_Country->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Country->SetMaxSize(wxSize(24, -1));
	this->m_TextCtrl_ROMHeader_Country->SetValue(global_projectconfig.ROMHeader_Country);
	m_Sizer_ROMHeader->Add(this->m_TextCtrl_ROMHeader_Country, 0, wxALL, 5);
	m_Sizer_ProjectStuff->Add(m_Sizer_ROMHeader, 1, wxEXPAND, 5);

	// Project settings (Program flags)
	this->m_Text_GCCFlags = new wxStaticText(this, wxID_ANY, wxT("GCC flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_GCCFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_GCCFlags, 0, wxALL, 5);
	this->m_TextCtrl_GCCFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_GCCFlags->SetValue(global_projectconfig.Flags_GCC);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_GCCFlags, 0, wxALL | wxEXPAND, 5);
	this->m_Text_LDFlags = new wxStaticText(this, wxID_ANY, wxT("LD flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_LDFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_LDFlags, 0, wxALL, 5);
	this->m_TextCtrl_LDFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_LDFlags->SetValue(global_projectconfig.Flags_LD);
	m_Sizer_ProjectStuff->Add(this->m_TextCtrl_LDFlags, 0, wxALL | wxEXPAND, 5);
	this->m_Text_MILDFlags = new wxStaticText(this, wxID_ANY, wxT("MakeROM flags:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_MILDFlags->Wrap(-1);
	m_Sizer_ProjectStuff->Add(this->m_Text_MILDFlags, 0, wxALL, 5);
	this->m_TextCtrl_MILDFlags = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_MILDFlags->SetValue(global_projectconfig.Flags_MILD);
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
	this->m_TextCtrl_LibultraPath->SetValue(global_programconfig.Path_Libultra);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_LibultraPath, 0, wxALL | wxEXPAND, 5);
	this->m_Text_ToolkitPath = new wxStaticText(this, wxID_ANY, wxT("Toolkit path:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_ToolkitPath->Wrap(-1);
	m_Sizer_ProgramStuff->Add(this->m_Text_ToolkitPath, 0, wxALL, 5);
	this->m_TextCtrl_ToolkitPath = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_ToolkitPath->SetValue(global_programconfig.Path_Toolkit);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_ToolkitPath, 0, wxALL | wxEXPAND, 5);
	this->m_Text_MoveFolder = new wxStaticText(this, wxID_ANY, wxT("Move folder:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_MoveFolder->Wrap(-1);
	this->m_Text_MoveFolder->Enable(global_programconfig.Use_Move);
	m_Sizer_ProgramStuff->Add(this->m_Text_MoveFolder, 0, wxALL, 5);
	this->m_TextCtrl_MoveFolder = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_MoveFolder->SetValue(global_programconfig.Path_Move);
	this->m_TextCtrl_MoveFolder->Enable(global_programconfig.Use_Move);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_MoveFolder, 0, wxALL | wxEXPAND, 5);
	this->m_Text_DisassemblyName = new wxStaticText(this, wxID_ANY, wxT("Disassembly name:"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Text_DisassemblyName->Wrap(-1);
	m_Sizer_ProgramStuff->Add(this->m_Text_DisassemblyName, 0, wxALL, 5);
	this->m_TextCtrl_DisassemblyName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	this->m_TextCtrl_DisassemblyName->SetValue(global_programconfig.DissamblyName);
	m_Sizer_ProgramStuff->Add(this->m_TextCtrl_DisassemblyName, 0, wxALL | wxEXPAND, 5);
	m_Sizer_Main->Add(m_Sizer_ProgramStuff, 1, wxEXPAND, 5);

	// Checkboxes section
	wxGridSizer* m_Sizer_Checkboxes;
	m_Sizer_Checkboxes = new wxGridSizer(0, 3, 0, 0);
	this->m_CheckBox_UseEXEW32 = new wxCheckBox(this, wxID_ANY, wxT("Use EXEW32"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_UseEXEW32->SetValue(global_programconfig.Use_EXEW32);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseEXEW32, 0, wxALL, 5);
	this->m_CheckBox_UseBuild = new wxCheckBox(this, wxID_ANY, wxT("Leave objects next to source"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_UseBuild->SetValue(!global_programconfig.Use_Build);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseBuild, 0, wxALL, 5);
	this->m_CheckBox_SeparateDebug = new wxCheckBox(this, wxID_ANY, wxT("Keep debug objects separate"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_SeparateDebug->SetValue(global_programconfig.SeparateDebug);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_SeparateDebug, 0, wxALL, 5);
	this->m_CheckBox_UseMakemask = new wxCheckBox(this, wxID_ANY, wxT("Makemask ROM"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_UseMakemask->SetValue(global_programconfig.Use_MakeMask);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseMakemask, 0, wxALL, 5);
	this->m_CheckBox_UseNRDC = new wxCheckBox(this, wxID_ANY, wxT("Register ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_UseNRDC->SetValue(global_programconfig.Use_NRDC);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UseNRDC, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_MoveROM = new wxCheckBox(this, wxID_ANY, wxT("Move ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_MoveROM->SetValue(global_programconfig.Use_Move);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_MoveROM, 0, wxALL, 5);
	this->m_CheckBox_PromptMoving = new wxCheckBox(this, wxID_ANY, wxT("Prompt for moving"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_PromptMoving->SetValue(global_programconfig.Prompt_Move);
	this->m_CheckBox_PromptMoving->Enable(global_programconfig.Use_Move);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptMoving, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_UploadEnabled = new wxCheckBox(this, wxID_ANY, wxT("Upload ROM after building"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_UploadEnabled->SetValue(global_programconfig.Use_Upload);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_UploadEnabled, 0, wxALL, 5);
	this->m_CheckBox_PromptUpload = new wxCheckBox(this, wxID_ANY, wxT("Prompt for uploading"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_PromptUpload->SetValue(global_programconfig.Prompt_Upload);
	this->m_CheckBox_PromptUpload->Enable(global_programconfig.Use_Upload);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptUpload, 0, wxALL, 5);
	m_Sizer_Checkboxes->Add(0, 0, 1, wxEXPAND, 5);
	this->m_CheckBox_PromptDisassembly = new wxCheckBox(this, wxID_ANY, wxT("Prompt for disassembly"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_PromptDisassembly->SetValue(global_programconfig.Prompt_Disassembly);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptDisassembly, 0, wxALL, 5);
	this->m_CheckBox_PromptClean = new wxCheckBox(this, wxID_ANY, wxT("Prompt for cleaning"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_CheckBox_PromptClean->SetValue(global_programconfig.Prompt_Clean);
	m_Sizer_Checkboxes->Add(this->m_CheckBox_PromptClean, 0, wxALL, 5);
	m_Sizer_Main->Add(m_Sizer_Checkboxes, 1, wxEXPAND, 5);
	this->m_Button_Close = new wxButton(this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Main->Add(this->m_Button_Close, 0, wxALIGN_CENTER | wxALL, 5);

	// Set the page layout
	this->SetSizer(m_Sizer_Main);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect events
	this->m_TextCtrl_TargetName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_TargetName_OnText), NULL, this);
	this->m_TextCtrl_BuildFolder->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_BuildFolder_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Title->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Title_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Manufacturer->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_ID->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_ID_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Country->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Country_OnText), NULL, this);
	this->m_TextCtrl_GCCFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_GCCFlags_OnText), NULL, this);
	this->m_TextCtrl_LDFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LDFlags_OnText), NULL, this);
	this->m_TextCtrl_MILDFlags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MILDFlags_OnText), NULL, this);
	this->m_TextCtrl_LibultraPath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LibultraPath_OnText), NULL, this);
	this->m_TextCtrl_ToolkitPath->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_Toolkit_OnText), NULL, this);
	this->m_TextCtrl_MoveFolder->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MoveFolder_OnText), NULL, this);
	this->m_TextCtrl_DisassemblyName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_DisassemblyName_OnText), NULL, this);
	this->m_CheckBox_UseEXEW32->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseEXEW32_OnCheckBox), NULL, this);
	this->m_CheckBox_SeparateDebug->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_SeparateDebug_OnCheckBox), NULL, this);
	this->m_CheckBox_UseBuild->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseBuild_OnCheckBox), NULL, this);
	this->m_CheckBox_UseNRDC->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseNRDC_OnCheckBox), NULL, this);
	this->m_CheckBox_UseMakemask->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseMakemask_OnCheckBox), NULL, this);
	this->m_CheckBox_MoveROM->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_MoveROM_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptMoving->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptMoving_OnCheckBox), NULL, this);
	this->m_CheckBox_UploadEnabled->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UploadEnabled_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptUpload->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptUpload_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptDisassembly->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptDisassembly_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptClean->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptClean_OnCheckBox), NULL, this);
	this->m_Button_Close->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Preferences::m_Button_Close_OnButtonClick), NULL, this);
}

Preferences::~Preferences()
{
	Config_SaveProgramConfig();
	Config_SaveProjectConfig();
	delete this->m_Disabler;
	this->GetParent()->SetFocus();
	((Main*)this->GetParent())->RefreshProjectTree();

	// Disconnect events
	this->m_TextCtrl_TargetName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_TargetName_OnText), NULL, this);
	this->m_TextCtrl_BuildFolder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_BuildFolder_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Title->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Title_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Manufacturer->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_ID->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_ID_OnText), NULL, this);
	this->m_TextCtrl_ROMHeader_Country->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_ROMHeader_Country_OnText), NULL, this);
	this->m_TextCtrl_GCCFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_GCCFlags_OnText), NULL, this);
	this->m_TextCtrl_LDFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LDFlags_OnText), NULL, this);
	this->m_TextCtrl_MILDFlags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MILDFlags_OnText), NULL, this);
	this->m_TextCtrl_LibultraPath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_LibultraPath_OnText), NULL, this);
	this->m_TextCtrl_MoveFolder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_MoveFolder_OnText), NULL, this);
	this->m_TextCtrl_DisassemblyName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Preferences::m_TextCtrl_DisassemblyName_OnText), NULL, this);
	this->m_CheckBox_UseEXEW32->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseEXEW32_OnCheckBox), NULL, this);
	this->m_CheckBox_UseBuild->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseBuild_OnCheckBox), NULL, this);
	this->m_CheckBox_UseNRDC->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseNRDC_OnCheckBox), NULL, this);
	this->m_CheckBox_UseMakemask->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UseMakemask_OnCheckBox), NULL, this);
	this->m_CheckBox_MoveROM->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_MoveROM_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptMoving->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptMoving_OnCheckBox), NULL, this);
	this->m_CheckBox_UploadEnabled->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_UploadEnabled_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptUpload->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptUpload_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptDisassembly->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptDisassembly_OnCheckBox), NULL, this);
	this->m_CheckBox_PromptClean->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Preferences::m_CheckBox_PromptClean_OnCheckBox), NULL, this);
	this->m_Button_Close->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Preferences::m_Button_Close_OnButtonClick), NULL, this);
}

void Preferences::m_TextCtrl_TargetName_OnText(wxCommandEvent& event)
{
	global_projectconfig.TargetName = event.GetString();
	if (global_projectconfig.TargetName == "")
		global_projectconfig.TargetName = DEFAULT_ROMNAME;
}

void Preferences::m_TextCtrl_BuildFolder_OnText(wxCommandEvent& event)
{
	global_projectconfig.BuildFolder = event.GetString();
	if (global_projectconfig.BuildFolder == "")
		global_projectconfig.BuildFolder = DEFAULT_BUILDPATH;
}

void Preferences::m_TextCtrl_ROMHeader_Title_OnText(wxCommandEvent& event)
{
	event.SetString(event.GetString().Left(20));
	global_projectconfig.ROMHeader_Name = event.GetString();
}

void Preferences::m_TextCtrl_ROMHeader_Manufacturer_OnText(wxCommandEvent& event)
{
	event.SetString(event.GetString().Left(1));
	global_projectconfig.ROMHeader_Manufacturer = event.GetString();
}

void Preferences::m_TextCtrl_ROMHeader_ID_OnText(wxCommandEvent& event)
{
	event.SetString(event.GetString().Left(2));
	global_projectconfig.ROMHeader_ID = event.GetString();
}

void Preferences::m_TextCtrl_ROMHeader_Country_OnText(wxCommandEvent& event)
{
	event.SetString(event.GetString().Left(1));
	global_projectconfig.ROMHeader_Country = event.GetString();
}

void Preferences::m_TextCtrl_GCCFlags_OnText(wxCommandEvent& event)
{
	global_projectconfig.Flags_GCC = event.GetString();
}

void Preferences::m_TextCtrl_LDFlags_OnText(wxCommandEvent& event)
{
	global_projectconfig.Flags_LD = event.GetString();
}

void Preferences::m_TextCtrl_MILDFlags_OnText(wxCommandEvent& event)
{
	global_projectconfig.Flags_MILD = event.GetString();
}

void Preferences::m_TextCtrl_LibultraPath_OnText(wxCommandEvent& event)
{
	global_programconfig.Path_Libultra = event.GetString();
	if (event.GetString() == "")
		global_programconfig.DissamblyName = DEFAULT_LIBULTRAPATH;
}

void Preferences::m_TextCtrl_Toolkit_OnText(wxCommandEvent& event)
{
	global_programconfig.Path_Toolkit = event.GetString();
	if (event.GetString() == "")
		global_programconfig.DissamblyName = DEFAULT_TOOLKITPATH;
}

void Preferences::m_TextCtrl_MoveFolder_OnText(wxCommandEvent& event)
{
	global_programconfig.Path_Move = event.GetString();
	if (event.GetString() == "")
		global_programconfig.DissamblyName = DEFAULT_MOVEPATH;
}

void Preferences::m_TextCtrl_DisassemblyName_OnText(wxCommandEvent& event)
{
	global_programconfig.DissamblyName = event.GetString();
	if (event.GetString() == "")
		global_programconfig.DissamblyName = DEFAULT_DISASSEMBLY;
}

void Preferences::m_CheckBox_UseEXEW32_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_EXEW32 = event.IsChecked();
}

void Preferences::m_CheckBox_SeparateDebug_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.SeparateDebug = event.IsChecked();
}

void Preferences::m_CheckBox_UseBuild_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_Build = !event.IsChecked();
	this->m_Text_BuildFolder->Enable(global_programconfig.Use_Build);
	this->m_TextCtrl_BuildFolder->Enable(global_programconfig.Use_Build);
}

void Preferences::m_CheckBox_UseNRDC_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_NRDC = event.IsChecked();
	this->m_Text_ROMHeader->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Title->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Manufacturer->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_ID->Enable(global_programconfig.Use_NRDC);
	this->m_TextCtrl_ROMHeader_Country->Enable(global_programconfig.Use_NRDC);
}

void Preferences::m_CheckBox_UseMakemask_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_MakeMask = event.IsChecked();
}

void Preferences::m_CheckBox_MoveROM_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_Move = event.IsChecked();
	this->m_Text_MoveFolder->Enable(global_programconfig.Use_Move);
	this->m_TextCtrl_MoveFolder->Enable(global_programconfig.Use_Move);
	this->m_CheckBox_PromptMoving->Enable(global_programconfig.Use_Move);
}

void Preferences::m_CheckBox_UploadEnabled_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Use_Upload = event.IsChecked();
	this->m_CheckBox_PromptUpload->Enable(global_programconfig.Use_Upload);
}

void Preferences::m_CheckBox_PromptMoving_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Prompt_Move = event.IsChecked();
}

void Preferences::m_CheckBox_PromptUpload_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Prompt_Upload = event.IsChecked();
}

void Preferences::m_CheckBox_PromptDisassembly_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Prompt_Disassembly = event.IsChecked();
}

void Preferences::m_CheckBox_PromptClean_OnCheckBox(wxCommandEvent& event)
{
	global_programconfig.Prompt_Clean = event.IsChecked();
}

void Preferences::m_Button_Close_OnButtonClick(wxCommandEvent& event)
{
	this->Close();
}

void Config_SaveProgramConfig()
{
	global_configfile->Write("/ProgramConfig/Path_Libultra", global_programconfig.Path_Libultra);
	global_configfile->Write("/ProgramConfig/Path_Toolkit", global_programconfig.Path_Toolkit);
	global_configfile->Write("/ProgramConfig/Path_Move", global_programconfig.Path_Move);
	global_configfile->Write("/ProgramConfig/Path_DisassemblyName", global_programconfig.DissamblyName);
	global_configfile->Write("/ProgramConfig/Use_EXEW32", global_programconfig.Use_EXEW32);
	global_configfile->Write("/ProgramConfig/Use_EXEW32", global_programconfig.SeparateDebug);
	global_configfile->Write("/ProgramConfig/Use_Build", global_programconfig.Use_Build);
	global_configfile->Write("/ProgramConfig/Use_NRDC", global_programconfig.Use_NRDC);
	global_configfile->Write("/ProgramConfig/Use_Makemask", global_programconfig.Use_MakeMask);
	global_configfile->Write("/ProgramConfig/Use_Move", global_programconfig.Use_Move);
	global_configfile->Write("/ProgramConfig/Use_Upload", global_programconfig.Use_Upload);
	global_configfile->Write("/ProgramConfig/Prompt_Move", global_programconfig.Prompt_Move);
	global_configfile->Write("/ProgramConfig/Prompt_Upload", global_programconfig.Prompt_Upload);
	global_configfile->Write("/ProgramConfig/Prompt_Disassembly", global_programconfig.Prompt_Disassembly);
	global_configfile->Write("/ProgramConfig/Prompt_Clean", global_programconfig.Prompt_Clean);
	global_configfile->Flush();
}

void Config_LoadProgramConfig()
{
	global_configfile->Read("/ProgramConfig/Path_Libultra", &global_programconfig.Path_Libultra);
	global_configfile->Read("/ProgramConfig/Path_Toolkit", &global_programconfig.Path_Toolkit);
	global_configfile->Read("/ProgramConfig/Path_Move", &global_programconfig.Path_Move);
	global_configfile->Read("/ProgramConfig/Path_DisassemblyName", &global_programconfig.DissamblyName);
	global_programconfig.Use_EXEW32 = global_configfile->ReadBool("/ProgramConfig/Use_EXEW32", DEFAULT_USEEXEW32);
	global_programconfig.SeparateDebug = global_configfile->ReadBool("/ProgramConfig/SeparateDebug", DEFAULT_SEPARATEDEBUG);
	global_programconfig.Use_Build = global_configfile->ReadBool("/ProgramConfig/Use_Build", DEFAULT_USEBUILD);
	global_programconfig.Use_NRDC = global_configfile->ReadBool("/ProgramConfig/Use_NRDC", DEFAULT_USENRDC);
	global_programconfig.Use_MakeMask = global_configfile->ReadBool("/ProgramConfig/Use_Makemask", DEFAULT_USEMAKEMASK);
	global_programconfig.Use_Move = global_configfile->ReadBool("/ProgramConfig/Use_Move", DEFAULT_USEMOVE);
	global_programconfig.Use_Upload = global_configfile->ReadBool("/ProgramConfig/Use_Upload", DEFAULT_USEUPLOAD);
	global_programconfig.Prompt_Move = global_configfile->ReadBool("/ProgramConfig/Prompt_Move", DEFAULT_PROMPTMOVE);
	global_programconfig.Prompt_Upload = global_configfile->ReadBool("/ProgramConfig/Prompt_Upload", DEFAULT_PROMPTUPLOAD);
	global_programconfig.Prompt_Disassembly = global_configfile->ReadBool("/ProgramConfig/Prompt_Disassembly", DEFAULT_PROMPTDISASS);
	global_programconfig.Prompt_Clean = global_configfile->ReadBool("/ProgramConfig/Prompt_Clean", DEFAULT_PROMPTCLEAN);
}

void Config_DefaultProgramConfig()
{
	global_programconfig.Path_Libultra = DEFAULT_LIBULTRAPATH;
	global_programconfig.Path_Toolkit = DEFAULT_TOOLKITPATH;
	global_programconfig.Path_Move = DEFAULT_MOVEPATH;
	global_programconfig.DissamblyName = DEFAULT_DISASSEMBLY;
	global_programconfig.Use_EXEW32 = DEFAULT_USEEXEW32;
	global_programconfig.SeparateDebug = DEFAULT_SEPARATEDEBUG;
	global_programconfig.Use_Build = DEFAULT_USEBUILD;
	global_programconfig.Use_NRDC = DEFAULT_USENRDC;
	global_programconfig.Use_MakeMask = DEFAULT_USEMAKEMASK;
	global_programconfig.Use_Move = DEFAULT_USEMOVE;
	global_programconfig.Use_Upload = DEFAULT_USEUPLOAD;
	global_programconfig.Prompt_Move = DEFAULT_PROMPTMOVE;
	global_programconfig.Prompt_Upload = DEFAULT_PROMPTUPLOAD;
	global_programconfig.Prompt_Disassembly = DEFAULT_PROMPTDISASS;
	global_programconfig.Prompt_Clean = DEFAULT_PROMPTCLEAN;
	if (!global_configfile->HasEntry("/ProgramConfig/Path_Libultra"))
		global_configfile->Write("/ProgramConfig/Path_Libultra", global_programconfig.Path_Libultra);
	if (!global_configfile->HasEntry("/ProgramConfig/Path_Toolkit"))
		global_configfile->Write("/ProgramConfig/Path_Toolkit", global_programconfig.Path_Toolkit);
	if (!global_configfile->HasEntry("/ProgramConfig/Path_Move"))
		global_configfile->Write("/ProgramConfig/Path_Move", global_programconfig.Path_Move);
	if (!global_configfile->HasEntry("/ProgramConfig/Path_DisassemblyName"))
		global_configfile->Write("/ProgramConfig/Path_DisassemblyName", global_programconfig.DissamblyName);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_EXEW32"))
		global_configfile->Write("/ProgramConfig/Use_EXEW32", global_programconfig.Use_EXEW32);
	if (!global_configfile->HasEntry("/ProgramConfig/SeparateDebug"))
		global_configfile->Write("/ProgramConfig/SeparateDebug", global_programconfig.SeparateDebug);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_Build"))
		global_configfile->Write("/ProgramConfig/Use_Build", global_programconfig.Use_Build);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_NRDC"))
		global_configfile->Write("/ProgramConfig/Use_NRDC", global_programconfig.Use_NRDC);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_Makemask"))
		global_configfile->Write("/ProgramConfig/Use_Makemask", global_programconfig.Use_MakeMask);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_Move"))
		global_configfile->Write("/ProgramConfig/Use_Move", global_programconfig.Use_Move);
	if (!global_configfile->HasEntry("/ProgramConfig/Use_Upload"))
		global_configfile->Write("/ProgramConfig/Use_Upload", global_programconfig.Use_Upload);
	if (!global_configfile->HasEntry("/ProgramConfig/Prompt_Move"))
		global_configfile->Write("/ProgramConfig/Prompt_Move", global_programconfig.Prompt_Move);
	if (!global_configfile->HasEntry("/ProgramConfig/Prompt_Upload"))
		global_configfile->Write("/ProgramConfig/Prompt_Upload", global_programconfig.Prompt_Upload);
	if (!global_configfile->HasEntry("/ProgramConfig/Prompt_Disassembly"))
		global_configfile->Write("/ProgramConfig/Prompt_Disassembly", global_programconfig.Prompt_Disassembly);
	if (!global_configfile->HasEntry("/ProgramConfig/Prompt_Clean"))
		global_configfile->Write("/ProgramConfig/Prompt_Clean", global_programconfig.Prompt_Clean);
	global_configfile->Flush();
}

void Config_SaveProjectConfig()
{
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/TargetName", global_projectconfig.TargetName);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/BuildFolder", RelativeProjectPath(global_projectconfig.BuildFolder));
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Name", global_projectconfig.ROMHeader_Name);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Manufacturer", global_projectconfig.ROMHeader_Manufacturer);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_ID", global_projectconfig.ROMHeader_ID);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Country", global_projectconfig.ROMHeader_Country);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_GCC", global_projectconfig.Flags_GCC);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_LD", global_projectconfig.Flags_LD);
	global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_MILD", global_projectconfig.Flags_MILD);
	global_configfile->Flush();
}

void Config_LoadProjectConfig()
{
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/TargetName", &global_projectconfig.TargetName);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/BuildFolder", &global_projectconfig.BuildFolder);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Name", &global_projectconfig.ROMHeader_Name);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Manufacturer", &global_projectconfig.ROMHeader_Manufacturer);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_ID", &global_projectconfig.ROMHeader_ID);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Country", &global_projectconfig.ROMHeader_Country);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/Flags_GCC", &global_projectconfig.Flags_GCC);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/Flags_LD", &global_projectconfig.Flags_LD);
	global_configfile->Read("/Project_"+global_projectconfig.ProjectPath+"/Flags_MILD", &global_projectconfig.Flags_MILD);
	wxFileName(global_projectconfig.BuildFolder).MakeAbsolute(global_projectconfig.ProjectPath);
}

void Config_SaveExternalProjectConfig(wxFileName path, std::map<wxTreeListItem, CompUnit*>* compunits)
{
	wxFileConfig file(PROGRAM_NAME, "Buu342", path.GetFullPath());
	file.Write("/N64Project/TargetName", global_projectconfig.TargetName);
	file.Write("/N64Project/BuildFolder", RelativeProjectPath(global_projectconfig.BuildFolder));
	file.Write("/N64Project/ROMHeader_Name", global_projectconfig.ROMHeader_Name);
	file.Write("/N64Project/ROMHeader_Manufacturer", global_projectconfig.ROMHeader_Manufacturer);
	file.Write("/N64Project/ROMHeader_ID", global_projectconfig.ROMHeader_ID);
	file.Write("/N64Project/ROMHeader_Country", global_projectconfig.ROMHeader_Country);
	file.Write("/N64Project/Flags_GCC", global_projectconfig.Flags_GCC);
	file.Write("/N64Project/Flags_LD", global_projectconfig.Flags_LD);
	file.Write("/N64Project/Flags_MILD", global_projectconfig.Flags_MILD);
	file.Flush();

	// Get the list of segments
	std::map<wxString, std::vector<wxFileName>> segments = std::map<wxString, std::vector<wxFileName>>();
	for (std::map<wxTreeListItem, CompUnit*>::iterator it = compunits->begin(); it != compunits->end(); ++it)
	{
		if (it->second->GetSegment().GetName() != "codesegment")
		{
			wxString segment = it->second->GetSegment().GetName();
			if (segments.count(segment) == 0)
				segments.insert(std::pair<wxString, std::vector<wxFileName>>(segment, std::vector<wxFileName>()));
			segments.at(segment).push_back(it->second->GetFilePath());
		}
	}

	// Write the segment data
	if (segments.size() > 0)
	{
		wxString segmentlist = wxEmptyString;
		for (std::map<wxString, std::vector<wxFileName>>::iterator it = segments.begin(); it != segments.end(); ++it)
			segmentlist = segmentlist + "\"" + it->first + "\"" + " ";
		file.Write("/N64Project/SegmentList", segmentlist);
		for (std::map<wxString, std::vector<wxFileName>>::iterator it = segments.begin(); it != segments.end(); ++it)
		{
			wxString filelist = wxEmptyString;
			for (std::vector<wxFileName>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
				filelist = filelist + "\"" + RelativeProjectPath(*it2) + "\"" + " ";
			file.Write("/N64Project/Segment_"+it->first, filelist);
		}
		file.Flush();
	}
}

void Config_LoadExternalProjectConfig(wxFileName path)
{
	wxFileConfig file(PROGRAM_NAME, "Buu342", path.GetFullPath());
	file.Read("/N64Project/TargetName", &global_projectconfig.TargetName);
	file.Read("/N64Project/BuildFolder", &global_projectconfig.BuildFolder);
	file.Read("/N64Project/ROMHeader_Name", &global_projectconfig.ROMHeader_Name);
	file.Read("/N64Project/ROMHeader_Manufacturer", &global_projectconfig.ROMHeader_Manufacturer);
	file.Read("/N64Project/ROMHeader_ID", &global_projectconfig.ROMHeader_ID);
	file.Read("/N64Project/ROMHeader_Country", &global_projectconfig.ROMHeader_Country);
	file.Read("/N64Project/Flags_GCC", &global_projectconfig.Flags_GCC);
	file.Read("/N64Project/Flags_LD", &global_projectconfig.Flags_LD);
	file.Read("/N64Project/Flags_MILD", &global_projectconfig.Flags_MILD);
	wxFileName(global_projectconfig.BuildFolder).MakeAbsolute(global_projectconfig.ProjectPath);

	// Handle segments
	if (file.HasEntry("/N64Project/SegmentList"))
	{
		std::vector<wxString> segments = std::vector<wxString>();
		wxString parse = file.Read("/N64Project/SegmentList");

		// Get the list of segments
		while (parse.size() > 0)
		{
			parse = parse.AfterFirst('\"');
			segments.push_back(parse.BeforeFirst('\"'));
			parse = parse.AfterFirst('\"');
			parse = parse.AfterFirst(' ');
		}

		// Now transpile them over to the project config file
		global_configfile->Write("/Project_" + global_projectconfig.ProjectPath + "/SegmentList", file.Read("/N64Project/SegmentList"));
		for (std::vector<wxString>::iterator it = segments.begin(); it != segments.end(); ++it)
			global_configfile->Write("/Project_" + global_projectconfig.ProjectPath + "/Segment_" + (*it), file.Read("/N64Project/Segment_"+(*it)));
		global_configfile->Write("/Project_" + global_projectconfig.ProjectPath + "/SegmentList", file.Read("/N64Project/SegmentList"));
		global_configfile->Flush();
	}
}

void Config_DefaultProjectConfig()
{
	global_projectconfig.TargetName  = DEFAULT_ROMNAME;
	global_projectconfig.BuildFolder = DEFAULT_BUILDPATH;
	global_projectconfig.ROMHeader_Name = DEFAULT_ROMHEADER_NAME;
	global_projectconfig.ROMHeader_ID = DEFAULT_ROMHEADER_ID;
	global_projectconfig.ROMHeader_Manufacturer = DEFAULT_ROMHEADER_MANUF;
	global_projectconfig.ROMHeader_Country = DEFAULT_ROMHEADER_COUNTRY;
	global_projectconfig.Flags_GCC = DEFAULT_GCCFLAGS;
	global_projectconfig.Flags_LD = DEFAULT_LDFLAGS;
	global_projectconfig.Flags_MILD = DEFAULT_MILDFLAGS;
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/TargetName"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/TargetName", global_projectconfig.TargetName);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/BuildFolder"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/BuildFolder", global_projectconfig.BuildFolder);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Name"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Name", global_projectconfig.ROMHeader_Name);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Manufacturer"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Manufacturer", global_projectconfig.ROMHeader_Manufacturer);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_ID"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_ID", global_projectconfig.ROMHeader_ID);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Country"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/ROMHeader_Country", global_projectconfig.ROMHeader_Country);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/Flags_GCC"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_GCC", global_projectconfig.Flags_GCC);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/Flags_LD"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_LD", global_projectconfig.Flags_LD);
	if (!global_configfile->HasEntry("/Project_"+global_projectconfig.ProjectPath+"/Flags_MILD"))
		global_configfile->Write("/Project_"+global_projectconfig.ProjectPath+"/Flags_MILD", global_projectconfig.Flags_MILD);
	global_configfile->Flush();
}