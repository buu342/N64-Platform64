#include "main.h"
#include "traverser.h"
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <chrono>
#include <thread>

#define PROJECTPATH   wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath()
//#define PROJECTPATH   "C:/Users/Lourenço/Desktop/Game Development/Game Projects/N64 Platform64/Engine/N64"
#define LIBULTRAPATH  "C:/ultra"
//#define LIBULTRAPATH  "X:/N64/ultra"
#define MIPSEPATH     "GCC/MIPSE/BIN"
#define MIPSEFULLPATH (LIBULTRAPATH + wxString("/") + MIPSEPATH)
#define OUTPUTPATH    "build"

/*********************************
           Constructors
*********************************/

Main::Main() : wxFrame(nullptr, wxID_ANY, PROGRAM_NAME, wxPoint(0, 0), wxSize(640, 480), wxDEFAULT_FRAME_STYLE)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

	// Create the main sizer
	wxFlexGridSizer* m_Sizer_Main;
	m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
	m_Sizer_Main->AddGrowableCol(0);
	m_Sizer_Main->AddGrowableRow(0);
	m_Sizer_Main->SetFlexibleDirection(wxBOTH);
	m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	// Create the tree icons list
	wxSize iconSize = wxArtProvider::GetSizeHint(wxART_LIST);
	if (iconSize == wxDefaultSize)
		iconSize = wxSize(16, 16);
	this->m_treeIcons = new wxImageList();
	this->m_treeIcons->Create(iconSize.x, iconSize.y);
	this->m_treeIcons->Add(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FOLDER"))));
	this->m_treeIcons->Add(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NORMAL_FILE"))));

	// Create the tree control
	this->m_TreeCtrl_ProjectDir = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxBORDER_SUNKEN);
	m_Sizer_Main->Add(this->m_TreeCtrl_ProjectDir, 0, wxALL | wxEXPAND, 5);
	this->m_TreeCtrl_ProjectDir->AssignImageList(this->m_treeIcons);

	// Create the bottom sizer
	wxBoxSizer* m_Sizer_Bottom;
	m_Sizer_Bottom = new wxBoxSizer(wxHORIZONTAL);

	// Add the bottom buttons
	this->m_Button_Disassemble = new wxButton(this, wxID_ANY, wxT("Disassemble"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Bottom->Add(this->m_Button_Disassemble, 0, wxALL, 5);
	m_Sizer_Bottom->Add(0, 0, 1, wxEXPAND, 5);
	this->m_Button_Clean = new wxButton(this, wxID_ANY, wxT("Clean"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Bottom->Add(this->m_Button_Clean, 0, wxALL, 5);
	this->m_Button_Build = new wxButton(this, wxID_ANY, wxT("Build"), wxDefaultPosition, wxDefaultSize, 0);
	m_Sizer_Bottom->Add(this->m_Button_Build, 0, wxALL, 5);
	m_Sizer_Main->Add(m_Sizer_Bottom, 1, wxEXPAND, 5);

	// Fix the layout
	this->RefreshProjectTree();
	this->SetSizer(m_Sizer_Main);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect Events
	this->m_Button_Disassemble->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Disassemble_OnButtonClick), NULL, this);
	this->m_Button_Clean->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Clean_OnButtonClick), NULL, this);
	this->m_Button_Build->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Build_OnButtonClick), NULL, this);
}

Main::~Main()
{
	this->m_treeIcons->RemoveAll();
	this->m_TreeCtrl_ProjectDir->DeleteAllItems();
	this->m_Button_Disassemble->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Disassemble_OnButtonClick), NULL, this);
	this->m_Button_Clean->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Clean_OnButtonClick), NULL, this);
	this->m_Button_Build->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Build_OnButtonClick), NULL, this);
}

void Main::m_Button_Disassemble_OnButtonClick(wxCommandEvent& event)
{
	wxDir builddir(PROJECTPATH + wxString("/") + wxString(OUTPUTPATH));
	wxArrayString files;
	wxFileName candidate;
	builddir.GetAllFiles(builddir.GetName(), &files, "*.out");

	// Make sure we have a .out to disassemble
	if (files.IsEmpty())
	{
		wxMessageDialog dialog(this, "No .out found to disassemble. Did you build the project?", "Error", wxICON_ERROR);
		dialog.ShowModal();
		return;
	}

	// Find the largest .out file, as it's likely to have the ELF information we need
	candidate.Assign(files[0]);
	for (size_t i=1; i<files.size(); i++)
		if (wxFileName(files[i]).GetSize() > candidate.GetSize())
			candidate.Assign(files[i]);

	// Alright, we have the .out, lets disassemble it.
	// To workaround EXEGCC bugs, make a copy of it and move it to the libultra folder
	wxCopyFile(builddir.GetName() + wxString("/") + candidate.GetFullName(), LIBULTRAPATH + wxString("/d.out"), true);

	// Run objdump
	wxShell(wxString(MIPSEFULLPATH) + wxString("/objdump.exe --disassemble-all --source  --wide --all-header --line-numbers ") + wxString(LIBULTRAPATH) + wxString("/d.out > ") + wxString(LIBULTRAPATH) + wxString("/disassembly.txt"));

	// Cleanup
	wxCopyFile(wxString(LIBULTRAPATH) + wxString("/disassembly.txt"), wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() + wxString("/disassembly.txt"), true);
	wxRemoveFile(wxString(LIBULTRAPATH) + wxString("/d.out"));
	wxRemoveFile(wxString(LIBULTRAPATH) + wxString("/disassembly.txt"));
	wxMessageDialog dialog(this, "Dumped to 'disassembly.txt'", "Ok");
	dialog.ShowModal();
}

void Main::m_Button_Clean_OnButtonClick(wxCommandEvent& event)
{
	// Ask for confirmation
	wxMessageDialog dialog1(this, "Clean up the build?", "Cleanup?" , wxYES_NO | wxICON_QUESTION);
	if (dialog1.ShowModal() == wxID_YES)
	{
		// Delete the build
		wxDir::Remove(PROJECTPATH + wxString("/") + wxString(OUTPUTPATH), wxPATH_RMDIR_RECURSIVE);
		wxMessageDialog dialog2(this, "Project cleaned", "Ok");
		dialog2.ShowModal();
	}
}

void Main::m_Button_Build_OnButtonClick(wxCommandEvent& event)
{
	wxDir::Make(PROJECTPATH + wxString("/build"));
	wxArrayString* out = new wxArrayString();
	//wxExecuteEnv* env = NULL;

	// First, check when the file was last compiled

	// Then, figure out its dependencies

	// Rebuild what is necessary

	// Link the codesegment together

	// Call MakeROM

	//long prog = wxExecute(wxString(MIPSEFULLPATH) + wxString("/gcc.exe"), *out);
}

void Main::RefreshProjectTree()
{
	// Clear the project tree
	this->m_TreeCtrl_ProjectDir->DeleteAllItems();
	wxTreeItemId root = this->m_TreeCtrl_ProjectDir->AddRoot("Project");

	// Open the project directory
	wxString path = PROJECTPATH;
	wxDir dir(path);
	if (!dir.IsOpened())
	{
		wxMessageDialog dialog(this, "Unable to open project path", "Error", wxICON_ERROR);
		dialog.ShowModal();
		return;
	}

	// Traverse the tree
	Traverser traverser(path, this->m_TreeCtrl_ProjectDir, root);
	dir.Traverse(traverser);
	dir.Close();
}