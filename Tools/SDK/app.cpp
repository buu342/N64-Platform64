/***************************************************************
                            app.cpp

This file handles the wxWidgets initialization.
***************************************************************/

#include "app.h"
#include "Resources/Icons/Icon_Main.h"

wxIMPLEMENT_APP(App);


/*********************************
             Globals
*********************************/

wxIcon   Icon_Main = wxNullIcon;
wxCursor Cursor_Blank = wxNullCursor;


/*==============================
    App (Constructor)
    Initializes the class
==============================*/

App::App()
{
    
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

App::~App()
{
    
}


/*==============================
    App::OnInit
    Called when the application is initialized
    @returns Whether the application initialized correctly
==============================*/

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // Initialize image handlers
    wxInitAllImageHandlers();

    // Initialize icons
    wxBitmap temp = wxBITMAP_PNG_FROM_DATA(Icon_Main);
    Icon_Main.CopyFromBitmap(temp);

    // Initialize blank cursor
    wxBitmap img = wxBitmap(1, 1, 1);
    wxBitmap mask = wxBitmap(1, 1, 1);
    img.SetMask(new wxMask(mask));
    Cursor_Blank = wxCursor(img.ConvertToImage());

    // Show the main window
    this->m_Frame = new Frame_Main();
    this->m_Frame->SetIcon(Icon_Main);
    this->m_Frame->Show();
    SetTopWindow(this->m_Frame);
    return true;
}