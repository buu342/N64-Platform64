#include "app.h"

// Icons

wxIMPLEMENT_APP(App);

App::App()
{
	// Nothing
}

App::~App()
{
	// Nothing
}

bool App::OnInit()
{
	if (!wxApp::OnInit())
		return false;
	wxInitAllImageHandlers();
	this->m_ProgramFrame = new Main();
	//this->m_ProgramFrame->SetIcon(iconbm_prog);
	this->m_ProgramFrame->Show();
	SetTopWindow(this->m_ProgramFrame);
	return true;
}