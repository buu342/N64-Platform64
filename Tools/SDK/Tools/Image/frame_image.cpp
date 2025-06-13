/***************************************************************
                        frame_image.cpp

TODO
***************************************************************/

#include "frame_image.h"
#include "../../resource.h"
#include "../../main.h"

#define CONTENT_FOLDER     wxString("Images")
#define CONTENT_NAME       wxString("Image")
#define CONTENT_EXTENSION  wxString("*.p64_img")

static wxIcon IconGenerator(bool large)
{
    return Icon_Texture;
}

static void AssetGenerator(wxFileName path)
{
    return;
}

static void AssetLoad(wxFileName path)
{
    return;
}

Frame_ImageBrowser::Frame_ImageBrowser( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetIcon(Icon_Texture);
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer( wxVERTICAL );

    m_Splitter_Vertical = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_Splitter_Vertical->Connect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_VerticalOnIdle ), NULL, this );

    m_Panel_Search = new Panel_Search(m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    this->m_Panel_Search->Search_SetAssetType(CONTENT_NAME, CONTENT_EXTENSION);
    this->m_Panel_Search->Search_IconGenerator(IconGenerator);
    this->m_Panel_Search->Search_NewAssetGenerator(AssetGenerator);
    this->m_Panel_Search->Search_LoadAssetFunc(AssetLoad);
    this->m_Panel_Search->Search_SetFolder(((Frame_Main*)this->GetParent())->GetAssetsPath() + CONTENT_FOLDER + wxFileName::GetPathSeparator());

    m_Panel_Edit = new wxPanel( m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* m_Sizer_Edit;
    m_Sizer_Edit = new wxBoxSizer( wxVERTICAL );

    m_Splitter_Horizontal = new wxSplitterWindow( m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_Splitter_Horizontal->Connect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_HorizontalOnIdle ), NULL, this );

    m_Panel_Preview = new wxPanel( m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_Panel_Config = new wxPanel( m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_Splitter_Horizontal->SplitHorizontally( m_Panel_Preview, m_Panel_Config, 0 );
    m_Sizer_Edit->Add( m_Splitter_Horizontal, 1, wxEXPAND, 5 );

    m_Panel_Edit->SetSizer( m_Sizer_Edit );
    m_Panel_Edit->Layout();
    m_Sizer_Edit->Fit( m_Panel_Edit );
    m_Splitter_Vertical->SplitVertically( m_Panel_Search, m_Panel_Edit, 0 );
    m_Sizer_Main->Add( m_Splitter_Vertical, 1, wxEXPAND, 5 );

    this->SetSizer( m_Sizer_Main );
    this->Layout();

    this->Centre( wxBOTH );
}

Frame_ImageBrowser::~Frame_ImageBrowser()
{
    
}