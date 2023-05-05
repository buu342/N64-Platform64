#include "app.h"
#include <wx/dir.h>

// Icons
#include "resources/icon_prog.h"
#include "resources/icon_c.h"
#include "resources/icon_h.h"
#include "resources/icon_config.h"

wxIMPLEMENT_APP(App);

wxIcon   iconbm_prog = wxNullIcon;
wxBitmap iconbm_c = wxNullBitmap;
wxBitmap iconbm_h = wxNullBitmap;
wxIcon   iconbm_config = wxNullIcon;

App::App()
{
	this->m_ProgramFrame = NULL;
	wxDir::Make(wxStandardPaths::Get().GetUserConfigDir() + "/N64" + PROGRAM_NAME + "/", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	global_configfile = new wxFileConfig(PROGRAM_NAME, "Buu342", wxStandardPaths::Get().GetUserConfigDir() + "/N64" + PROGRAM_NAME + "/Config.ini");
	Config_DefaultProgramConfig();
	Config_LoadProgramConfig();
	Config_DefaultProjectConfig();
	Config_LoadProjectConfig();
}

App::~App()
{
	// Nothing
}

bool App::OnInit()
{
	wxBitmap temp;
	if (!wxApp::OnInit())
		return false;
	wxInitAllImageHandlers();
	iconbm_prog.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_prog));
	iconbm_c = wxBITMAP_PNG_FROM_DATA(icon_c);
	iconbm_h = wxBITMAP_PNG_FROM_DATA(icon_h);
	iconbm_config.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_config));
	this->m_ProgramFrame = new Main();
	this->m_ProgramFrame->SetIcon(iconbm_prog);
	this->m_ProgramFrame->Show();
	SetTopWindow(this->m_ProgramFrame);
	return true;
}