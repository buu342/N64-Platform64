#include "main.h"
#include "app.h"
#include "traverser.h"
#include "preferences.h"
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <iterator>
#include "helper.h"

bool global_modifieddebug = false;


/*********************************
           Constructors
*********************************/

Main::Main() : wxFrame(nullptr, wxID_ANY, PROGRAM_NAME, wxPoint(0, 0), wxSize(640, 480), wxDEFAULT_FRAME_STYLE)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	this->m_CompUnits = new std::map<wxTreeListItem, CompUnit*>();
	this->m_Segments = new std::map<wxString, std::vector<wxFileName>*>();
	this->m_Segments->insert(std::pair<wxString, std::vector<wxFileName>*>("codesegment", new std::vector<wxFileName>()));

	// Setup the log window
	this->m_LogWin = new wxLogWindow(this, wxT("Compilation Log"), false);
	this->m_LogWin->SetVerbose(true);
	this->m_LogWin->GetFrame()->SetIcon(iconbm_prog);
	this->m_LogWin->DisableTimestamp();

	// Create the main sizer
	wxFlexGridSizer* m_Sizer_Main;
	m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
	m_Sizer_Main->AddGrowableCol(0);
	m_Sizer_Main->AddGrowableRow(1);
	m_Sizer_Main->SetFlexibleDirection(wxBOTH);
	m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	// Create the compile mode choice select
	this->m_Choice_BuildMode = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	this->PopulateCompileChoices();
	m_Sizer_Main->Add(this->m_Choice_BuildMode, 0, wxALL | wxEXPAND, 5);

	// Create the tree icons list
	wxSize iconSize = wxArtProvider::GetSizeHint(wxART_LIST);
	if (iconSize == wxDefaultSize)
		iconSize = wxSize(16, 16);
	this->m_treeIcons = new wxImageList();
	this->m_treeIcons->Create(iconSize.x, iconSize.y);
	this->m_treeIcons->Add(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FOLDER"))));
	this->m_treeIcons->Add(iconbm_c);
	this->m_treeIcons->Add(iconbm_h);
	
	// Create the tree control
	this->m_TreeCtrl_ProjectDir = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE);
	this->m_TreeCtrl_ProjectDir->AppendColumn(wxT("File"), 500, wxALIGN_LEFT, wxCOL_RESIZABLE);
	this->m_TreeCtrl_ProjectDir->AppendColumn(wxT("Segment"), wxCOL_WIDTH_DEFAULT, wxALIGN_CENTER, wxCOL_RESIZABLE);
	this->m_TreeCtrl_ProjectDir->AssignImageList(this->m_treeIcons);
	m_Sizer_Main->Add(this->m_TreeCtrl_ProjectDir, 0, wxALL | wxEXPAND, 5);

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
	this->m_Button_Upload = new wxButton(this, wxID_ANY, wxT("Upload"), wxDefaultPosition, wxDefaultSize, 0);
	this->m_Button_Upload->Enable(false);
	m_Sizer_Bottom->Add(this->m_Button_Upload, 0, wxALL, 5);
	m_Sizer_Main->Add(m_Sizer_Bottom, 1, wxEXPAND, 5);

	// File menu bar
	this->m_MenuBar = new wxMenuBar(0);
	this->m_Menu_File = new wxMenu();
	wxMenuItem* m_MenuItem_Open;
	m_MenuItem_Open = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Open Project Folder")) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_File->Append(m_MenuItem_Open);
	wxMenuItem* m_MenuItem_Refresh;
	m_MenuItem_Refresh = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Refresh")) + wxT('\t') + wxT("F5"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_File->Append(m_MenuItem_Refresh);
	this->m_Menu_File->AppendSeparator();
	wxMenuItem* m_MenuItem_Exit;
	m_MenuItem_Exit = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Exit")) + wxT('\t') + wxT("ALF+F4"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_File->Append(m_MenuItem_Exit);
	this->m_MenuBar->Append(this->m_Menu_File, wxT("File"));
	this->m_Menu_Build = new wxMenu();
	wxMenuItem* m_MenuItem_Build;

	// Build menu bar
	m_MenuItem_Build = new wxMenuItem(this->m_Menu_Build, wxID_ANY, wxString(wxT("Build")) + wxT('\t') + wxT("CTRL+B"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_Build->Append(m_MenuItem_Build);
	wxMenuItem* m_MenuItem_Clean;
	m_MenuItem_Clean = new wxMenuItem(this->m_Menu_Build, wxID_ANY, wxString(wxT("Clean")) + wxT('\t') + wxT("CTRL+C"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_Build->Append(m_MenuItem_Clean);
	wxMenuItem* m_MenuItem_Disassemble;
	m_MenuItem_Disassemble = new wxMenuItem(this->m_Menu_Build, wxID_ANY, wxString(wxT("Disassemble")) + wxT('\t') + wxT("CTRL+D"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_Build->Append(m_MenuItem_Disassemble);
	wxMenuItem* m_MenuItem_Upload;
	m_MenuItem_Upload = new wxMenuItem(this->m_Menu_Build, wxID_ANY, wxString(wxT("Upload")) + wxT('\t') + wxT("CTRL+U"), wxEmptyString, wxITEM_NORMAL);
	m_MenuItem_Upload->Enable(false);
	this->m_Menu_Build->Append(m_MenuItem_Upload);
	this->m_Menu_Build->AppendSeparator();
	wxMenuItem* m_MenuItem_ForceRebuild;
	m_MenuItem_ForceRebuild = new wxMenuItem(this->m_Menu_Build, wxID_ANY, wxString(wxT("Force ROM Rebuild")) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_Build->Append(m_MenuItem_ForceRebuild);
	this->m_MenuBar->Append(this->m_Menu_Build, wxT("Build"));
	this->m_Menu_Settings = new wxMenu();
	wxMenuItem* m_MenuItem_Config;

	// Settings menu bar
	m_MenuItem_Config = new wxMenuItem(this->m_Menu_Settings, wxID_ANY, wxString(wxT("Preferences")) + wxT('\t') + wxT("CTRL+P"), wxEmptyString, wxITEM_NORMAL);
	this->m_Menu_Settings->Append(m_MenuItem_Config);
	this->m_MenuBar->Append(this->m_Menu_Settings, wxT("Settings"));
	this->SetMenuBar(this->m_MenuBar);

	// Fix the layout
	this->RefreshProjectTree();
	this->SetSizer(m_Sizer_Main);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect Events
	this->m_TreeCtrl_ProjectDir->Connect(wxEVT_TREELIST_ITEM_ACTIVATED, wxTreeListEventHandler(Main::m_TreeCtrl_ProjectDir_OnActivated), NULL, this);
	this->m_Choice_BuildMode->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Main::m_Choice_BuildMode_OnChoice), NULL, this);
	this->m_Button_Disassemble->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Disassemble_OnButtonClick), NULL, this);
	this->m_Button_Clean->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Clean_OnButtonClick), NULL, this);
	this->m_Button_Build->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Build_OnButtonClick), NULL, this);
	this->m_Button_Upload->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Upload_OnButtonClick), NULL, this);
	this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Open_OnSelection), this, m_MenuItem_Open->GetId());
	this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Refresh_OnSelection), this, m_MenuItem_Refresh->GetId());
	this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Exit_OnSelection), this, m_MenuItem_Exit->GetId());
	this->m_Menu_Build->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Build_OnSelection), this, m_MenuItem_Build->GetId());
	this->m_Menu_Build->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Clean_OnSelection), this, m_MenuItem_Clean->GetId());
	this->m_Menu_Build->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Disassemble_OnSelection), this, m_MenuItem_Disassemble->GetId());
	this->m_Menu_Build->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Upload_OnSelection), this, m_MenuItem_Upload->GetId());
	this->m_Menu_Build->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_ForceRebuild_OnSelection), this, m_MenuItem_ForceRebuild->GetId());
	this->m_Menu_Settings->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Config_OnSelection), this, m_MenuItem_Config->GetId());
}

Main::~Main()
{
	this->m_treeIcons->RemoveAll();
	this->m_TreeCtrl_ProjectDir->DeleteAllItems();
	this->m_Button_Disassemble->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Disassemble_OnButtonClick), NULL, this);
	this->m_Button_Clean->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Clean_OnButtonClick), NULL, this);
	this->m_Button_Build->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Build_OnButtonClick), NULL, this);
	this->m_Button_Upload->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Upload_OnButtonClick), NULL, this);

	for (std::map<wxTreeListItem, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
		delete (it->second);
	delete this->m_CompUnits;
	for (std::map<wxString, std::vector<wxFileName>*>::iterator it = this->m_Segments->begin(); it != this->m_Segments->end(); it++)
		delete (it->second);
	delete this->m_Segments;
}

void Main::m_TreeCtrl_ProjectDir_OnActivated(wxTreeListEvent& event)
{
	wxTreeListItem id = event.GetItem();
	wxTextEntryDialog dialog(this, "Please enter the segment name:", "Segment name", this->m_TreeCtrl_ProjectDir->GetItemText(id, 1), wxOK | wxCANCEL);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxString seg = "codesegment";

		// If the segment name is invalid, then rename it to codesegment
		if (dialog.GetValue() != "")
			seg = dialog.GetValue();

		// Insert it into a new map entry and change the column text
		SetTreeSegment(id, seg, this->m_TreeCtrl_ProjectDir, this->m_Segments);
		this->m_TreeCtrl_ProjectDir->SetItemText(id, 1, seg);
	}
}

void Main::m_Choice_BuildMode_OnChoice(wxCommandEvent& event)
{

}

void Main::m_Button_Disassemble_OnButtonClick(wxCommandEvent& event)
{
	this->DisassembleROM();
}

void Main::m_Button_Clean_OnButtonClick(wxCommandEvent& event)
{
	this->CleanProject();
}

void Main::m_Button_Build_OnButtonClick(wxCommandEvent& event)
{
	this->BuildProject();
}

void Main::m_Button_Upload_OnButtonClick(wxCommandEvent& event)
{
	this->UploadROM();
}

void Main::m_MenuItem_Open_OnSelection(wxCommandEvent& event)
{
	wxDirDialog dlg(this, "Open project folder", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK)
	{
		Config_DefaultProjectConfig();
		global_projectconfig.ProjectPath = dlg.GetPath();
		Config_LoadProjectConfig();
		this->RefreshProjectTree();
		this->PopulateCompileChoices();
	}
}

void Main::m_MenuItem_Refresh_OnSelection(wxCommandEvent& event)
{
	this->RefreshProjectTree();
	this->PopulateCompileChoices();
}

void Main::m_MenuItem_Exit_OnSelection(wxCommandEvent& event)
{
	this->Close();
}

void Main::m_MenuItem_Build_OnSelection(wxCommandEvent& event)
{
	this->BuildProject();
}

void Main::m_MenuItem_Clean_OnSelection(wxCommandEvent& event)
{
	this->CleanProject();
}

void Main::m_MenuItem_Disassemble_OnSelection(wxCommandEvent& event)
{
	this->DisassembleROM();
}

void Main::m_MenuItem_Upload_OnSelection(wxCommandEvent& event)
{
	this->UploadROM();
}

void Main::m_MenuItem_ForceRebuild_OnSelection(wxCommandEvent& event)
{
	this->BuildROM();
}

void Main::m_MenuItem_Config_OnSelection(wxCommandEvent& event)
{
	Preferences* pref = new Preferences(this);
	pref->SetIcon(iconbm_config);
	pref->Show();
}

void Main::PopulateCompileChoices()
{
	int oldcount = this->m_Choice_BuildMode->GetCount();
	wxString choices[3] = {wxT("Release"), wxT("Debug"), wxT("Autodetect from debug.h") };

	if (wxFileExists(global_projectconfig.ProjectPath + "/debug.h") && (oldcount == 0 || oldcount == 2))
	{
		this->m_Choice_BuildMode->Clear();
		this->m_Choice_BuildMode->Append(3, choices);
		this->m_Choice_BuildMode->SetSelection(2);
	}
	else if (!wxFileExists(global_projectconfig.ProjectPath + "/debug.h") && (oldcount == 0 || oldcount == 3))
	{
		this->m_Choice_BuildMode->Clear();
		this->m_Choice_BuildMode->Append(2, choices);
		this->m_Choice_BuildMode->SetSelection(1);
	}
	this->Refresh();
}

void Main::RefreshProjectTree()
{
	// Clear the project tree
	this->m_TreeCtrl_ProjectDir->DeleteAllItems();
	wxTreeListItem root = this->m_TreeCtrl_ProjectDir->GetRootItem();

	// Clean up the map
	for (std::map<wxTreeListItem, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
		delete (it->second);
	delete this->m_CompUnits;

	// Make a new one
	this->m_CompUnits = new std::map<wxTreeListItem, CompUnit*>();

	// Open the project directory
	wxString path = global_projectconfig.ProjectPath;
	wxDir dir(path);
	if (!dir.IsOpened())
	{
		wxMessageDialog dialog(this, "Unable to open project path", "Error", wxICON_ERROR);
		dialog.ShowModal();
		return;
	}

	// Traverse the tree
	Traverser traverser(path, this->m_TreeCtrl_ProjectDir, root, this->m_CompUnits, this->m_Segments, {"c", "h"});
	dir.Traverse(traverser);
	dir.Close();

}

bool Main::CheckDebugEnabled()
{
	if (this->m_Choice_BuildMode->GetSelection() == 2)
	{
		bool retval = false;
		wxTextFile debugh;
		debugh.Open("debug.h");
		while (!debugh.Eof())
		{
			wxString str = debugh.GetNextLine();
			if (str.Find("#define DEBUG_MODE") != wxNOT_FOUND)
			{
				wxStringTokenizer tokenizer(str, " ");
				while (tokenizer.HasMoreTokens())
				{
					wxString token = tokenizer.GetNextToken();
					if (token == "1")
					{
						retval = true;
						goto done;
					}
					else if (token == "0")
						goto done;
				}
			}
		}
		done:
			debugh.Close();
			return retval;
	}
	else
		return (this->m_Choice_BuildMode->GetSelection());
}

void Main::ModifyDebugH()
{
	wxTextFile debugh;
	wxTextFile copy;
	if (!wxFileExists("debug.h") || global_modifieddebug || this->m_Choice_BuildMode->GetSelection() == 2)
		return;
	if (this->CheckDebugEnabled() == (this->m_Choice_BuildMode->GetSelection() == 1))
		return;
	wxRenameFile("debug.h", "_debug.h");
	debugh.Open("_debug.h");
	copy.Open("debug.h");
	while (!debugh.Eof())
	{
		wxString str = debugh.GetNextLine();
		if (str.Find("#define DEBUG_MODE") != wxNOT_FOUND)
			copy.AddLine("#define DEBUG_MODE " + wxString::Format("%d", this->m_Choice_BuildMode->GetSelection()));
		else
			copy.AddLine(str);
	}
	debugh.Close();
	copy.Close();
	global_modifieddebug = true;
}

void Main::FixDebugH()
{
	if (!global_modifieddebug)
		return;
	wxRemoveFile("debug.h");
	wxRenameFile("_debug.h", "debug.h");
	global_modifieddebug = false;
}

void Main::CleanProject()
{
	// First, check if there are objects to clean
	wxTreeListCtrl cleantree(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_DEFAULT_STYLE);
	wxTreeListItem root = cleantree.GetRootItem();
	wxDir dir(global_projectconfig.ProjectPath);
	Traverser traverser(global_projectconfig.ProjectPath, &cleantree, root, {"o", "out", "n64"});
	dir.Traverse(traverser);
	dir.Close();

	// Stop if there's nothing to clean
	if (!cleantree.GetFirstChild(root).IsOk() && !wxDirExists(global_projectconfig.BuildFolder) && !wxFileExists(global_projectconfig.ProjectPath+"/"+global_programconfig.DissamblyName))
	{
		wxMessageDialog dialog2(this, "Nothing to clean", "Error");
		dialog2.ShowModal();
		return;
	}

	// Ask for confirmation?
	wxMessageDialog dialog1(this, "Clean up the build?", "Cleanup?" , wxYES_NO | wxICON_QUESTION);
	if (!global_programconfig.Prompt_Clean || dialog1.ShowModal() == wxID_YES)
	{
		// Delete compiled objects
		traverser.DeleteFiles(root);

		// Delete the build folder
		if (global_programconfig.Use_Build && wxDirExists(global_projectconfig.BuildFolder))
			wxFileName(global_projectconfig.BuildFolder+"/").Rmdir(wxPATH_RMDIR_RECURSIVE);

		// Remove the disassembly file
		if (wxFileExists(global_projectconfig.ProjectPath+"/"+global_programconfig.DissamblyName))
			wxRemoveFile(global_projectconfig.ProjectPath+"/"+global_programconfig.DissamblyName);
		wxMessageDialog dialog2(this, "Project cleaned", "Success");
		dialog2.ShowModal();
	}
}

void Main::BuildProject()
{
	bool isdebug = this->CheckDebugEnabled();
	bool builtsomething = false;
	bool compilefail = false;
	wxArrayString* out = new wxArrayString();
	wxFileName target = global_projectconfig.ProjectPath + "/" + global_projectconfig.TargetName;
	wxFileName codeseg = global_projectconfig.ProjectPath + "/codesegment.o";

	// Setup before building
	if (global_programconfig.Use_Build)
	{
		wxDir::Make(global_projectconfig.BuildFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
		target = global_projectconfig.BuildFolder + "/" + global_projectconfig.TargetName;
		codeseg = global_projectconfig.BuildFolder + "/codesegment.o";
	}
	if (isdebug && global_programconfig.SeparateDebug)
	{
		target = target.GetPath() + "/" + target.GetName() + "_d." + target.GetExt();
		codeseg = codeseg.GetPath() + "/" + codeseg.GetName() + "_d." + codeseg.GetExt();
	}
	this->ModifyDebugH();

	// Setup the log window
	this->m_LogWin->Show(true);
	if (isdebug)
		wxLogMessage("Debug Mode Enabled");
	else
		wxLogMessage("Debug Mode Disabled");

	// Rebuild what is necessary
	for (std::map<wxTreeListItem, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
	{
		if (it->second->ShouldRebuild(isdebug))
		{
			if (!builtsomething)
				builtsomething = true;
			wxLogMessage(it->second->GetOutputPath(isdebug).GetFullName() + wxString(" needs to be rebuilt"));
			it->second->Compile(isdebug);
			if (it->second->ShouldRebuild(isdebug))
				compilefail = true;
		}
	}

	// Check if everything compiled
	if (compilefail)
	{
		wxLogError("An error occurred during compilation\n\n");
		this->FixDebugH();
		return;
	}

	// Link the codesegment together
	if (!wxFileExists(codeseg.GetFullPath()) || builtsomething)
	{
		wxString command;
		wxArrayString output;
		wxStructStat stat_oldcodeseg;
		wxStructStat stat_newcodeseg;
		wxString libultraver = "-lgultra_d";
		wxString files = "";
		wxString exew32 = global_programconfig.Use_EXEW32 ? "exew32.exe " : "";

		// Log what we're doing
		wxLogMessage("Generating codesegment");

		// Set debug flags
		if (!isdebug)
			libultraver = "-lgultra_rom";

		// Get all the files we compiled
		for (std::map<wxTreeListItem, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
			files += it->second->GetOutputPath(isdebug).GetFullPath() + " ";

		// Run LD
		stat_oldcodeseg.st_mtime = LastModTime(codeseg.GetFullPath());
		command = global_programconfig.Path_Toolkit + "/" + exew32 + "ld.exe "
				+ "-o " + codeseg.GetFullPath() + " "
				+ "-r " + files
				+ libultraver + " " 
				+ global_projectconfig.Flags_LD;
		wxLogVerbose("> " + command);
		wxExecute(command, output, wxEXEC_SYNC, &GetProgramEnvironment());
		stat_newcodeseg.st_mtime = LastModTime(codeseg.GetFullPath());
		
		// Output errors
		for (size_t i = 0; i < output.size(); i++)
			wxLogError(output[i]);

		// Check for success
		if (!wxFileExists(codeseg.GetFullPath()) || !wxDateTime(stat_newcodeseg.st_mtime).IsLaterThan(wxDateTime(stat_oldcodeseg.st_mtime)))
			compilefail = true;
		else
			builtsomething = true;
	}

	// Check if everything compiled
	if (compilefail)
	{
		wxLogError("An error occurred during compilation\n\n");
		this->FixDebugH();
		return;
	}

	// Call MakeROM
	if (builtsomething || !wxFileExists(target.GetFullPath()))
		this->BuildROM();
	else
		wxLogMessage("Nothing to build\n\n");
	this->FixDebugH();
}

void Main::BuildROM()
{
	wxString command;
	wxArrayString output;
	wxStructStat stat_oldrom;
	wxStructStat stat_newrom;
	wxExecuteEnv env = GetProgramEnvironment();
	wxEnvVariableHashMap vars = env.env;
	wxString flags = "-d";
	bool isdebug = this->CheckDebugEnabled();
	wxFileName target = global_projectconfig.ProjectPath + "/" + global_projectconfig.TargetName;
	wxFileName codeseg = global_projectconfig.ProjectPath + "/codesegment.o";
	wxString exew32 = global_programconfig.Use_EXEW32 ? "exew32.exe " : "";

	// Setup before building
	if (global_programconfig.Use_Build)
	{
		wxDir::Make(global_projectconfig.BuildFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
		target = global_projectconfig.BuildFolder + "/" + global_projectconfig.TargetName;
		codeseg = global_projectconfig.BuildFolder + "/codesegment.o";
	}
	if (isdebug && global_programconfig.SeparateDebug)
	{
		target = target.GetPath() + "/" + target.GetName() + "_d." + target.GetExt();
		codeseg = codeseg.GetPath() + "/" + codeseg.GetName() + "_d." + codeseg.GetExt();
	}
	this->m_LogWin->Show(true);
	this->ModifyDebugH();

	// Log what we're doing
	wxLogMessage("Generating ROM");

	// Handle debug differences
	if (!isdebug)
		flags = "";

	// Add the codesegment to the environment variables
	vars["CODESEGMENT"] = "\"" + codeseg.GetFullPath() + "\"";
	env.env = vars;

	// Run makerom
	stat_oldrom.st_mtime = LastModTime(target.GetFullPath());
	command = global_programconfig.Path_Toolkit + "/" + exew32 +"mild.exe "
			+ global_projectconfig.ProjectPath + "/spec "
			+ flags + " "
			+ "-r " + target.GetFullPath() + " "
			+ "-e " + target.GetPath() + "/" + target.GetName() + ".out"
			+ global_projectconfig.Flags_MILD;
	wxLogVerbose("> " + command);
	wxExecute(command, output, wxEXEC_SYNC, &env);
	stat_newrom.st_mtime = LastModTime(target.GetFullPath());

	// Log makrom output
	for (size_t i = 0; i < output.size(); i++)
		wxLogMessage(output[i]);

	// Cleanup temp files created by makerom
	if (isdebug)
	{
		wxRemoveFile(global_projectconfig.ProjectPath + "/a.out");
		wxRemoveFile(global_projectconfig.ProjectPath + "/aaaaa000.cmd");
		wxRemoveFile(global_projectconfig.ProjectPath + "/aaaaa000.o");
		wxRemoveFile(global_projectconfig.ProjectPath + "/aaaaa000.s");
		wxRemoveFile(global_projectconfig.ProjectPath + "/aaaaa000.spc");
		wxRemoveFile(global_projectconfig.ProjectPath + "/spec.cvt");
	}

	// Check for success
	if (wxFileExists(target.GetFullPath()) && wxDateTime(stat_newrom.st_mtime).IsLaterThan(wxDateTime(stat_oldrom.st_mtime)))
	{
		// Makemask
		if (global_programconfig.Use_MakeMask)
		{
			output.clear();
			command = global_programconfig.Path_Toolkit + "/" + exew32 + "makemask.exe " + target.GetFullPath();
			wxLogVerbose("> " + command);
			wxExecute(command, output, wxEXEC_SYNC, &GetProgramEnvironment());
			for (size_t i = 0; i < output.size(); i++)
				wxLogMessage(output[i]);
		}

		// NRDC
		if (global_programconfig.Use_NRDC)
			this->RegisterROM(target.GetFullPath());

		// Move the ROM to the USB folder
		if (global_programconfig.Use_Move)
			this->MoveROM(target.GetFullPath());

		// Done!
		wxLogMessage("Success!\n\n");
	}
	else
		wxLogError("An error occurred during compilation\n\n");
	this->FixDebugH();
}

void Main::DisassembleROM()
{
	wxFileName target = global_projectconfig.ProjectPath + "/" + global_projectconfig.TargetName;

	// Get the right name
	if (global_programconfig.Use_Build)
		target = global_projectconfig.BuildFolder + "/" + global_projectconfig.TargetName;
	if (global_programconfig.SeparateDebug)
		target = target.GetPath() + "/" + target.GetName() + "_d." + target.GetExt();
	target = target.GetPath() + "/" + target.GetName() + ".out";

	// Make sure we have a .out to disassemble
	if (!wxFileExists(target.GetFullPath()))
	{
		wxMessageDialog dialog(this, "No .out found to disassemble. Did you build the project?", "Error", wxICON_ERROR);
		dialog.ShowModal();
		return;
	}

	// Alright, we have the .out, lets disassemble it.
	// To workaround EXEGCC bugs, make a copy of it and move it to the libultra folder
	wxCopyFile(target.GetFullPath(), global_programconfig.Path_Libultra + wxString("/d.out"), true);

	// Run objdump and move the disassembly over
	wxShell(global_programconfig.Path_Toolkit + "/objdump.exe --disassemble-all --source  --wide --all-header --line-numbers " + global_programconfig.Path_Libultra + "/d.out > " + global_programconfig.Path_Libultra + "/" + global_programconfig.DissamblyName);
	wxCopyFile(global_programconfig.Path_Libultra + "/" + global_programconfig.DissamblyName, global_projectconfig.ProjectPath + "/" + global_programconfig.DissamblyName, true);

	// Cleanup
	wxRemoveFile(global_programconfig.Path_Libultra + "/d.out");
	wxRemoveFile(global_programconfig.Path_Libultra + "/" + global_programconfig.DissamblyName);
	wxMessageDialog dialog(this, "Dumped to '" + global_programconfig.DissamblyName + "'", "Ok");
	dialog.ShowModal();
}

void Main::RegisterROM(wxString target)
{
	wxString command;
	wxArrayString output;
	command = global_programconfig.Path_Toolkit + "/nrdc.exe " + target
		+ " \"" + global_projectconfig.ROMHeader_Name + "\""
		+ " " + global_projectconfig.ROMHeader_Manufacturer
		+ " " + global_projectconfig.ROMHeader_ID
		+ " " + global_projectconfig.ROMHeader_Country;
	wxLogVerbose("> " + command);
	wxExecute(command, output, wxEXEC_SYNC, &GetProgramEnvironment());
	for (size_t i = 0; i < output.size(); i++)
		wxLogMessage(output[i]);
}

void Main::MoveROM(wxString target)
{
	wxMessageDialog dialog(this, "Move the compiled ROM to the USB directory?", "Move?", wxYES_NO | wxICON_QUESTION);
	if (!global_programconfig.Prompt_Move || dialog.ShowModal() == wxID_YES)
		wxCopyFile(target, global_programconfig.Path_Move + "/" + wxFileName(target).GetFullName());
}

void Main::UploadROM()
{
	wxMessageDialog dialog(this, "Not implemented yet.", "Sorry");
	dialog.ShowModal();
}