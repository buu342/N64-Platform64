#include "main.h"
#include "app.h"
#include "traverser.h"
#include "preferences.h"
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <iterator>
#include "helper.h"


/*********************************
           Constructors
*********************************/

Main::Main() : wxFrame(nullptr, wxID_ANY, PROGRAM_NAME, wxPoint(0, 0), wxSize(640, 480), wxDEFAULT_FRAME_STYLE)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	this->m_CompUnits = new std::map<wxTreeItemId, CompUnit*>();

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
	wxString m_Choice_BuildModeChoices[] = { wxT("Autodetect from debug.h"), wxT("Debug"), wxT("Release") };
	int m_Choice_BuildModeNChoices = sizeof(m_Choice_BuildModeChoices) / sizeof(wxString);
	this->m_Choice_BuildMode = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_BuildModeNChoices, m_Choice_BuildModeChoices, 0);
	this->m_Choice_BuildMode->SetSelection(0);
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
	this->m_Button_Upload = new wxButton(this, wxID_ANY, wxT("Upload"), wxDefaultPosition, wxDefaultSize, 0);
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

	for (std::map<wxTreeItemId, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
		delete (it->second);
	delete this->m_CompUnits;
}

void Main::m_Choice_BuildMode_OnChoice(wxCommandEvent& event)
{

}

void Main::m_Button_Disassemble_OnButtonClick(wxCommandEvent& event)
{
	/*
	wxDir builddir(OUTPUTPATH);
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
	wxShell(wxString(MIPSEFULLPATH) + wxString("/objdump.exe --disassemble-all --source  --wide --all-header --line-numbers ") + LIBULTRAPATH + wxString("/d.out > ") + LIBULTRAPATH + wxString("/disassembly.txt"));

	// Cleanup
	wxCopyFile(LIBULTRAPATH + wxString("/disassembly.txt"), DISASSPATH, true);
	wxRemoveFile(LIBULTRAPATH + wxString("/d.out"));
	wxRemoveFile(LIBULTRAPATH + wxString("/disassembly.txt"));
	wxMessageDialog dialog(this, "Dumped to '" + wxString(DISASSNAME) + "'", "Ok");
	dialog.ShowModal();
	*/
}

void Main::m_Button_Clean_OnButtonClick(wxCommandEvent& event)
{
	/*
	if (!wxDirExists(OUTPUTPATH) && !wxFileExists(DISASSPATH))
	{
		wxMessageDialog dialog2(this, "Nothing to clean", "Error");
		dialog2.ShowModal();
		return;
	}

	// Ask for confirmation
	wxMessageDialog dialog1(this, "Clean up the build?", "Cleanup?" , wxYES_NO | wxICON_QUESTION);
	if (dialog1.ShowModal() == wxID_YES)
	{
		// Delete the build folder
		if (wxDirExists(OUTPUTPATH))
			wxFileName(OUTPUTPATH+"/").Rmdir(wxPATH_RMDIR_RECURSIVE);

		// Remove the disassembly file
		if (wxFileExists(DISASSPATH))
			wxRemoveFile(DISASSPATH);
		wxMessageDialog dialog2(this, "Project cleaned", "Success");
		dialog2.ShowModal();
	}
	*/
}

void Main::m_Button_Build_OnButtonClick(wxCommandEvent& event)
{
	/*
	wxDir::Make(OUTPUTPATH, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	wxArrayString* out = new wxArrayString();
	bool isdebug = this->CheckDebugEnabled();
	bool builtsomething = false;
	bool compilefail = false;
	wxString target = isdebug ? FINALROM_D : FINALROM;

	// Setup the log window
	this->m_LogWin->Show(true);

	// Rebuild what is necessary
	for (std::map<wxTreeItemId, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
	{
		if (it->second->ShouldRebuild())
		{
			if (!builtsomething)
			{
				if (isdebug)
					wxLogMessage("Debug Mode Enabled");
				else
					wxLogMessage("Debug Mode Disabled");
				builtsomething = true;
			}
			wxLogMessage(it->second->GetOutputName().GetFullName() + wxString(" needs to be rebuilt"));
			it->second->Compile(isdebug);
			if (it->second->ShouldRebuild())
				compilefail = true;
		}
	}

	// Check if everything compiled
	if (compilefail)
	{
		wxLogError("An error occurred during compilation");
		return;
	}

	// Link the codesegment together
	if (!wxFileExists(CODESEGMENT) || builtsomething)
	{
		wxExecuteEnv env;
		wxString command;
		wxArrayString output;
		wxEnvVariableHashMap vars;
		wxStructStat stat_oldcodeseg;
		wxStructStat stat_newcodeseg;
		wxString libultraver = "-lgultra_d";
		wxString files = "";

		// Log what we're doing
		wxLogMessage("Generating codesegment");

		// Set debug flags
		if (!isdebug)
			libultraver = "-lgultra_rom";

		// Setup the environment
		vars["ROOT"] = LIBULTRAPATH;
		vars["gccdir"] = LIBULTRAPATH + wxString("/gcc");
		vars["PATH"] = MIPSEFULLPATH + ";" + LIBULTRAPATH + "/usr/sbin;C:/WINDOWS/system32;";
		vars["gccsw"] = "-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__";
		vars["n64align"] = "on";
		env.cwd = PROJECTPATH;
		env.env = vars;

		// Get all the files we compiled
		for (std::map<wxTreeItemId, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
			files += it->second->GetOutputName().GetFullPath() + " ";

		// Run LD
		stat_oldcodeseg.st_mtime = LastModTime(CODESEGMENT);
		command = MIPSEFULLPATH + "/exew32.exe ld.exe "
				+ "-o " + CODESEGMENT + " "
				+ "-r " + files
				+ global_projectconfig.Flags_LD + " "
				+ libultraver + " "
		wxLogVerbose("> " + command);
		wxExecute(command, output, wxEXEC_SYNC, &env);
		stat_newcodeseg.st_mtime = LastModTime(CODESEGMENT);
		
		// Output errors
		for (size_t i = 0; i < output.size(); i++)
			wxLogError(output[i]);

		// Check for success
		if (!wxFileExists(CODESEGMENT) || !wxDateTime(stat_newcodeseg.st_mtime).IsLaterThan(wxDateTime(stat_oldcodeseg.st_mtime)))
			compilefail = true;
		else
			builtsomething = true;
	}

	// Check if everything compiled
	if (compilefail)
	{
		wxLogError("An error occurred during compilation");
		return;
	}

	// Call MakeROM
	if (builtsomething || !wxFileExists(target))
	{
		wxExecuteEnv env;
		wxString command;
		wxArrayString output;
		wxEnvVariableHashMap vars;
		wxStructStat stat_oldrom;
		wxStructStat stat_newrom;
		wxString flags = "-d";

		// Log what we're doing
		wxLogMessage("Generating ROM");

		// Handle debug differences
		if (!isdebug)
		{
			target = FINALROM;
			flags = "";
		}

		// Setup the environment
		vars["ROOT"] = LIBULTRAPATH;
		vars["gccdir"] = LIBULTRAPATH + wxString("/gcc");
		vars["PATH"] = MIPSEFULLPATH + ";" + LIBULTRAPATH + "/usr/sbin;C:/WINDOWS/system32;";
		vars["gccsw"] = "-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__";
		vars["n64align"] = "on";
		env.cwd = PROJECTPATH;
		env.env = vars;

		// Run makerom
		stat_oldrom.st_mtime = LastModTime(target);
		command = MIPSEFULLPATH + "/exew32.exe mild.exe "
				+ PROJECTPATH + "/spec "
				+ flags + " "
				+ "-r " + target + " "
				+ "-e " + wxFileName(target).GetPath() + "/" + wxFileName(target).GetName() + ".out";
		wxLogVerbose("> " + command);
		wxExecute(command, output, wxEXEC_SYNC, &env);
		stat_newrom.st_mtime = LastModTime(target);

		// Log makrom output
		for (size_t i = 0; i < output.size(); i++)
			wxLogMessage(output[i]);

		// Cleanup temp files created by makerom
		if (isdebug)
		{
			wxRemoveFile(PROJECTPATH + "/a.out");
			wxRemoveFile(PROJECTPATH + "/aaaaa000.cmd");
			wxRemoveFile(PROJECTPATH + "/aaaaa000.o");
			wxRemoveFile(PROJECTPATH + "/aaaaa000.s");
			wxRemoveFile(PROJECTPATH + "/aaaaa000.spc");
			wxRemoveFile(PROJECTPATH + "/spec.cvt");
		}

		// Check for success
		if (wxFileExists(target) && wxDateTime(stat_newrom.st_mtime).IsLaterThan(wxDateTime(stat_oldrom.st_mtime)))
		{
			// Makemask
			output.clear();
			command = MIPSEFULLPATH + "/exew32.exe makemask.exe " + target;
			wxLogVerbose("> " + command);
			wxExecute(command, output, wxEXEC_SYNC, &env);
			for (size_t i = 0; i < output.size(); i++)
				wxLogMessage(output[i]);

			// NRDC
			output.clear();
			command = MIPSEFULLPATH + "/nrdc.exe " + target + " " + REGISTERINFO;
			wxLogVerbose("> " + command);
			wxExecute(command, output, wxEXEC_SYNC, &env);
			for (size_t i = 0; i < output.size(); i++)
				wxLogMessage(output[i]);

			// Move the ROM to the USB folder
			wxMessageDialog dialog1(this, "Move the compiled ROM to the USB directory?", "Move?", wxYES_NO | wxICON_QUESTION);
			if (dialog1.ShowModal() == wxID_YES)
				wxCopyFile(target, MOVEROMFOLDER + wxString("/") + wxFileName(target).GetFullName());

			// Done!
			wxLogMessage("Success!");
		}
		else
			wxLogError("An error occurred during compilation");
	}
	else
		wxLogMessage("Nothing to build");
	*/
}

void Main::m_Button_Upload_OnButtonClick(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Open_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Refresh_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Exit_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Build_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Clean_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Disassemble_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Upload_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_ForceRebuild_OnSelection(wxCommandEvent& event)
{

}

void Main::m_MenuItem_Config_OnSelection(wxCommandEvent& event)
{
	Preferences* pref = new Preferences(this);
	pref->SetIcon(iconbm_config);
	pref->Show();
}

void Main::RefreshProjectTree()
{
	// Clear the project tree
	this->m_TreeCtrl_ProjectDir->DeleteAllItems();
	wxTreeItemId root = this->m_TreeCtrl_ProjectDir->AddRoot("Project");

	// Clean up the map
	for (std::map<wxTreeItemId, CompUnit*>::iterator it = this->m_CompUnits->begin(); it != this->m_CompUnits->end(); it++)
		delete (it->second);
	delete this->m_CompUnits;

	// Make a new one
	this->m_CompUnits = new std::map<wxTreeItemId, CompUnit*>();

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
	Traverser traverser(path, this->m_TreeCtrl_ProjectDir, root, this->m_CompUnits);
	dir.Traverse(traverser);
	dir.Close();

}

bool Main::CheckDebugEnabled()
{
	bool retval = false;
	wxTextFile debugh;
	if (!wxFileExists(global_projectconfig.ProjectPath + wxString("/debug.h")))
		return false;
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