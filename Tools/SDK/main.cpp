#include "app.h"
#include "main.h"
#include "resource.h"

/*==============================
    Frame_Main (Constructor)
    Initializes the class
==============================*/

Frame_Main::Frame_Main() : wxFrame(nullptr, wxID_ANY, _("Platform64 SDK"), wxDefaultPosition, wxSize(300, 400), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
{
	wxVector<wxVariant> items;

	// Create the main sizer
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	wxGridSizer* m_Sizer_Main;
	m_Sizer_Main = new wxGridSizer(0, 1, 0, 0);

	// Create the data view list
	m_DataViewListCtrl_Main = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER);
	m_Sizer_Main->Add(m_DataViewListCtrl_Main, 0, wxALL|wxEXPAND, 5);
	this->m_DataViewListCtrl_Main->AppendIconTextColumn(wxString(), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_DEFAULT, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	items.push_back((wxVariant)wxDataViewIconText(wxString("Image Browser"), Icon_Texture));
	this->m_DataViewListCtrl_Main->AppendItem(items);
	items.pop_back();
	items.push_back((wxVariant)wxDataViewIconText(wxString("Material Browser"), Icon_Material));
	this->m_DataViewListCtrl_Main->AppendItem(items);

	// Finalize the layout
	this->SetSizer(m_Sizer_Main);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect Events
	m_DataViewListCtrl_Main->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(Frame_Main::m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated), NULL, this);
}


/*==============================
    Frame_Main (Destructor)
    Cleans up the class before deletion
==============================*/

Frame_Main::~Frame_Main()
{
    
}


/*==============================
	Frame_Main::m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated
	Called when a selected item is double clicked
	@param The generated event
==============================*/

void Frame_Main::m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated(wxDataViewEvent& event)
{
	// Prevent unused parameter warning
	(void)event;
}