/***************************************************************
                            app.cpp

This file handles the wxWidgets initialization.
***************************************************************/

#include "app.h"
#include "resource.h"

wxIMPLEMENT_APP(App);


/*********************************
             Globals
*********************************/

wxIcon   Icon_Main = wxNullIcon;
wxIcon   Icon_Texture = wxNullIcon;
wxIcon   Icon_Material = wxNullIcon;
wxCursor Cursor_Blank = wxNullCursor;
wxIcon   Icon_Back = wxNullIcon;
wxIcon   Icon_NewAsset = wxNullIcon;
wxIcon   Icon_NewFolder = wxNullIcon;
wxIcon   Icon_Search = wxNullIcon;
wxIcon   Icon_ViewGrid = wxNullIcon;
wxIcon   Icon_ViewList = wxNullIcon;
wxIcon   Icon_Folder = wxNullIcon;


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
    temp = wxBITMAP_PNG_FROM_DATA(Icon_Texture);
    Icon_Texture.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_Material);
    Icon_Material.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_Back);
    Icon_Back.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_NewAsset);
    Icon_NewAsset.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_NewFolder);
    Icon_NewFolder.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_Search);
    Icon_Search.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_ViewGrid);
    Icon_ViewGrid.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_ViewList);
    Icon_ViewList.CopyFromBitmap(temp);
    temp = wxBITMAP_PNG_FROM_DATA(Icon_Folder);
    Icon_Folder.CopyFromBitmap(temp);

    // Initialize blank cursor
    wxBitmap img = wxBitmap(1, 1, 1);
    wxBitmap mask = wxBitmap(1, 1, 1);
    img.SetMask(new wxMask(mask));
    Cursor_Blank = wxCursor(img.ConvertToImage());

    // Show the main window
    this->m_Frame = new Frame_Main();
    this->m_Frame->SetIcon(Icon_Main);
    this->m_Frame->OpenProject();
    this->m_Frame->Show();
    SetTopWindow(this->m_Frame);
    return true;
}