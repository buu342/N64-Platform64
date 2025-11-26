/***************************************************************
                        frame_code.cpp

This class implements the code editor panel
***************************************************************/

#include "frame_code.h"
#include "../../resource.h"
#include "../../main.h"


/*=============================================================
               Sound Editor Frame Implementation
=============================================================*/

Frame_CodeEditor::Frame_CodeEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    m_mgr.SetManagedWindow(this);
    m_mgr.SetFlags(wxAUI_MGR_ALLOW_FLOATING|wxAUI_MGR_DEFAULT|wxAUI_MGR_LIVE_RESIZE);

    m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
    m_Tool_Save = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Save, wxNullBitmap, wxITEM_NORMAL, _("Save changes"), wxEmptyString, NULL );
    m_ToolBar->AddSeparator();
    m_Tool_Copy = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Cut, wxNullBitmap, wxITEM_NORMAL, _("Cut"), wxEmptyString, NULL );
    m_Tool_Copy = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Copy, wxNullBitmap, wxITEM_NORMAL, _("Copy"), wxEmptyString, NULL );
    m_Tool_Paste = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Paste, wxNullBitmap, wxITEM_NORMAL, _("Paste"), wxEmptyString, NULL );
    m_ToolBar->AddSeparator();
    m_Tool_CompileDebug = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_BuildDebug, wxNullBitmap, wxITEM_NORMAL, _("Build the ROM in debug mode"), wxEmptyString, NULL );
    m_Tool_Compile = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Build, wxNullBitmap, wxITEM_NORMAL, _("Build the ROM"), wxEmptyString, NULL );
    m_Tool_Clean = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_Clean, wxNullBitmap, wxITEM_NORMAL, _("Clean the build files"), wxEmptyString, NULL );
    m_ToolBar->AddSeparator();
    m_Tool_UploadUSB = m_ToolBar->AddTool( wxID_ANY, _("tool"), Icon_USBUpload, wxNullBitmap, wxITEM_NORMAL, _("Upload ROM to flashcart"), wxEmptyString, NULL );
    m_ToolBar->Realize();
    m_mgr.AddPane( m_ToolBar, wxAuiPaneInfo() .Top() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Movable( false ).Dock().Resizable().FloatingSize( wxSize( -1,-1 ) ).DockFixed( true ).BottomDockable( false ).LeftDockable( false ).RightDockable( false ).Floatable( false ) );

    m_Menubar = new wxMenuBar( 0 );
    this->SetMenuBar( m_Menubar );

    m_Panel_Search = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_Panel_Search->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_Panel_Search->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );

    m_mgr.AddPane( m_Panel_Search, wxAuiPaneInfo() .Left() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Movable( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).BottomDockable( false ).TopDockable( false ).Floatable( false ).BestSize( wxSize( 200,-1 ) ).Layer( 2 ) );

    m_Notebook_Editor = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );
    m_mgr.AddPane( m_Notebook_Editor, wxAuiPaneInfo() .Center() .CaptionVisible( false ).CloseButton( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane().DefaultPane() );

    m_Panel_Page = new wxPanel( m_Notebook_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* Sizer_Page;
    Sizer_Page = new wxBoxSizer( wxVERTICAL );

    m_Editor = new wxStyledTextCtrl( m_Panel_Page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN, wxEmptyString );
    m_Editor->SetUseTabs( true );
    m_Editor->SetTabWidth( 4 );
    m_Editor->SetIndent( 4 );
    m_Editor->SetTabIndents( true );
    m_Editor->SetBackSpaceUnIndents( true );
    m_Editor->SetViewEOL( false );
    m_Editor->SetViewWhiteSpace( false );
    m_Editor->SetMarginWidth( 2, 0 );
    m_Editor->SetIndentationGuides( true );
    m_Editor->SetReadOnly( false );
    m_Editor->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
    m_Editor->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
    m_Editor->SetMarginWidth( 1, 16);
    m_Editor->SetMarginSensitive( 1, true );
    m_Editor->SetProperty( wxT("fold"), wxT("1") );
    m_Editor->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
    m_Editor->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
    m_Editor->SetMarginWidth( 0, m_Editor->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
    m_Editor->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
    m_Editor->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
    m_Editor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
    m_Editor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
    m_Editor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
    m_Editor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
    m_Editor->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
    m_Editor->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
    m_Editor->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
    m_Editor->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_Editor->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
    Sizer_Page->Add( m_Editor, 1, wxEXPAND, 5 );


    m_Panel_Page->SetSizer( Sizer_Page );
    m_Panel_Page->Layout();
    Sizer_Page->Fit( m_Panel_Page );
    m_Notebook_Editor->AddPage( m_Panel_Page, _("a page"), false, wxNullBitmap );

    m_AUINotebook_Output = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE|wxBORDER_DEFAULT );
    m_mgr.AddPane( m_AUINotebook_Output, wxAuiPaneInfo() .Bottom() .CaptionVisible( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).TopDockable( false ).LeftDockable( false ).RightDockable( false ).BestSize( wxSize( -1,200 ) ).Layer( 1 ) );

    m_Panel_Compiler = new wxPanel( m_AUINotebook_Output, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* Sizer_Compiler;
    Sizer_Compiler = new wxBoxSizer( wxVERTICAL );

    m_RichText_Compiler = new wxRichTextCtrl( m_Panel_Compiler, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
    Sizer_Compiler->Add( m_RichText_Compiler, 1, wxEXPAND | wxALL, 5 );


    m_Panel_Compiler->SetSizer( Sizer_Compiler );
    m_Panel_Compiler->Layout();
    Sizer_Compiler->Fit( m_Panel_Compiler );
    m_AUINotebook_Output->AddPage( m_Panel_Compiler, _("Compiler Output"), true, wxNullBitmap );
    m_Panel_Log = new wxPanel( m_AUINotebook_Output, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* Sizer_Log;
    Sizer_Log = new wxBoxSizer( wxVERTICAL );

    m_RichText_Log = new wxRichTextCtrl( m_Panel_Log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
    Sizer_Log->Add( m_RichText_Log, 1, wxEXPAND | wxALL, 5 );


    m_Panel_Log->SetSizer( Sizer_Log );
    m_Panel_Log->Layout();
    Sizer_Log->Fit( m_Panel_Log );
    m_AUINotebook_Output->AddPage( m_Panel_Log, _("Project Log"), false, wxNullBitmap );
    m_Panel_Flashcart = new wxPanel( m_AUINotebook_Output, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* Sizer_Flashcart;
    Sizer_Flashcart = new wxFlexGridSizer( 0, 1, 0, 0 );
    Sizer_Flashcart->AddGrowableCol( 0 );
    Sizer_Flashcart->AddGrowableRow( 0 );
    Sizer_Flashcart->SetFlexibleDirection( wxBOTH );
    Sizer_Flashcart->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_RichText_Flashcart = new wxRichTextCtrl( m_Panel_Flashcart, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
    Sizer_Flashcart->Add( m_RichText_Flashcart, 1, wxALL|wxEXPAND, 5 );

    wxFlexGridSizer* Sizer_FlashcartInput;
    Sizer_FlashcartInput = new wxFlexGridSizer( 0, 2, 0, 0 );
    Sizer_FlashcartInput->AddGrowableCol( 0 );
    Sizer_FlashcartInput->SetFlexibleDirection( wxBOTH );
    Sizer_FlashcartInput->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_TextCtrl_Flashcart = new wxTextCtrl( m_Panel_Flashcart, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    Sizer_FlashcartInput->Add( m_TextCtrl_Flashcart, 0, wxALL|wxEXPAND, 5 );

    m_Button_Flashcart = new wxButton( m_Panel_Flashcart, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
    Sizer_FlashcartInput->Add( m_Button_Flashcart, 0, wxALL, 5 );


    Sizer_Flashcart->Add( Sizer_FlashcartInput, 1, wxEXPAND, 5 );


    m_Panel_Flashcart->SetSizer( Sizer_Flashcart );
    m_Panel_Flashcart->Layout();
    Sizer_Flashcart->Fit( m_Panel_Flashcart );
    m_AUINotebook_Output->AddPage( m_Panel_Flashcart, _("Flashcart Log"), false, wxNullBitmap );

    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

    m_mgr.Update();
    this->Centre( wxBOTH );
}

Frame_CodeEditor::~Frame_CodeEditor()
{
    m_mgr.UnInit();

}