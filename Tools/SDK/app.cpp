/***************************************************************
                            app.cpp

This file handles the wxWidgets initialization.
***************************************************************/

#include "app.h"
#include "resource.h"

wxIMPLEMENT_APP(App);


/*=============================================================
                            Globals
=============================================================*/

wxCursor Cursor_Blank = wxNullCursor;
wxCursor Cursor_Pipette = wxNullCursor;
wxIcon   Icon_Back = wxNullIcon;
wxIcon   Icon_Blank = wxNullIcon;
wxIcon   Icon_BlankLarge = wxNullIcon;
wxIcon   Icon_Build = wxNullIcon;
wxIcon   Icon_BuildDebug = wxNullIcon;
wxIcon   Icon_C = wxNullIcon;
wxIcon   Icon_Clean = wxNullIcon;
wxIcon   Icon_Code = wxNullIcon;
wxIcon   Icon_Copy = wxNullIcon;
wxIcon   Icon_Cut = wxNullIcon;
wxIcon   Icon_Folder = wxNullIcon;
wxIcon   Icon_Folder_Large = wxNullIcon;
wxIcon   Icon_H = wxNullIcon;
wxIcon   Icon_Main = wxNullIcon;
wxIcon   Icon_Material = wxNullIcon;
wxIcon   Icon_MissingSmall = wxNullIcon;
wxIcon   Icon_MissingLarge = wxNullIcon;
wxIcon   Icon_NewAsset = wxNullIcon;
wxIcon   Icon_NewFolder = wxNullIcon;
wxIcon   Icon_Palette = wxNullIcon;
wxIcon   Icon_Paste = wxNullIcon;
wxIcon   Icon_Pipette = wxNullIcon;
wxIcon   Icon_Save = wxNullIcon;
wxIcon   Icon_Search = wxNullIcon;
wxIcon   Icon_Sound = wxNullIcon;
wxIcon   Icon_Texture = wxNullIcon;
wxIcon   Icon_ToggleAlpha = wxNullIcon;
wxIcon   Icon_ToggleFilter = wxNullIcon;
wxIcon   Icon_ToggleMask = wxNullIcon;
wxIcon   Icon_ToggleStatistics = wxNullIcon;
wxIcon   Icon_ToggleTiling = wxNullIcon;
wxIcon   Icon_USBUpload = wxNullIcon;
wxIcon   Icon_ViewGrid = wxNullIcon;
wxIcon   Icon_ViewList = wxNullIcon;
wxIcon   Icon_Waveform = wxNullIcon;
wxIcon   Icon_Waveform_Large = wxNullIcon;
wxIcon   Icon_ZoomFit = wxNullIcon;
wxIcon   Icon_ZoomIn = wxNullIcon;
wxIcon   Icon_ZoomOut = wxNullIcon;
wxBitmap Tex_Missing = wxNullBitmap;


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
    wxImage tempimg;

    if (!wxApp::OnInit())
        return false;

    // Initialize image handlers
    wxInitAllImageHandlers();

    // Initialize cursors
    tempimg = wxBITMAP_PNG_FROM_DATA(Cursor_Pipette).ConvertToImage();
    tempimg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
    tempimg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16);
    Cursor_Pipette = wxCursor(tempimg);

    // Initialize icons
    Icon_Back.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Back));
    Icon_Build.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Build));
    Icon_BuildDebug.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_BuildDebug));
    Icon_C.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_C));
    Icon_Clean.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Clean));
    Icon_Code.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Code));
    Icon_Copy.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Copy));
    Icon_Cut.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Cut));
    Icon_Folder.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Folder));
    Icon_Folder_Large.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Folder_Large));
    Icon_H.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_H));
    Icon_Main.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Main));
    Icon_Material.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Material));
    Icon_NewAsset.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_NewAsset));
    Icon_NewFolder.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_NewFolder));
    Icon_Palette.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Palette));
    Icon_Paste.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Paste));
    Icon_Pipette.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Pipette));
    Icon_Save.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Save));
    Icon_Search.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Search));
    Icon_Sound.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Sound));
    Icon_Texture.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Texture));
    Icon_ToggleAlpha.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ToggleAlpha));
    Icon_ToggleFilter.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ToggleFilter));
    Icon_ToggleMask.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ToggleMask));
    Icon_ToggleStatistics.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ToggleStatistics));
    Icon_ToggleTiling.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ToggleTiling));
    Icon_USBUpload.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_USBUpload));
    Icon_ViewGrid.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ViewGrid));
    Icon_ViewList.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ViewList));
    Icon_Waveform.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Waveform));
    Icon_Waveform_Large.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_Waveform_Large));
    Icon_ZoomFit.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ZoomFit));
    Icon_ZoomIn.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ZoomIn));
    Icon_ZoomOut.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(Icon_ZoomOut));

    // Initialize textures
    Tex_Missing = wxBITMAP_PNG_FROM_DATA(MISSING);

    // Initialize missing icons
    tempimg = Tex_Missing.ConvertToImage();
    tempimg.Rescale(16, 16);
    Icon_MissingSmall.CopyFromBitmap(tempimg);
    tempimg = Tex_Missing.ConvertToImage();
    tempimg.Rescale(64, 64);
    Icon_MissingLarge.CopyFromBitmap(tempimg);

    // Initialize blank cursor
    wxBitmap img = wxBitmap(16, 16, 1);
    wxBitmap mask = wxBitmap(16, 16, 1);
    img.SetMask(new wxMask(mask));
    Cursor_Blank = wxCursor(img.ConvertToImage());
    Icon_Blank.CopyFromBitmap(img);
    img = wxBitmap(64, 64);
    mask = wxBitmap(64, 64);
    img.SetMask(new wxMask(img, wxColour(0, 0, 0, 0)));
    Icon_BlankLarge.CopyFromBitmap(img);

    // Show the main window
    this->m_Frame = new Frame_Main();
    this->m_Frame->SetIcon(Icon_Main);
    this->m_Frame->OpenProject();
    this->m_Frame->Show();
    SetTopWindow(this->m_Frame);
    return true;
}