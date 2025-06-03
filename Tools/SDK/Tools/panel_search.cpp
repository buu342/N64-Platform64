#include "panel_search.h"

Panel_Search::Panel_Search(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
    wxFlexGridSizer* m_Sizer_Search;
    m_Sizer_Search = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Search->AddGrowableCol(0);
    m_Sizer_Search->AddGrowableRow(1);
    m_Sizer_Search->AddGrowableRow(2);
    m_Sizer_Search->SetFlexibleDirection(wxBOTH);
    m_Sizer_Search->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxGridSizer* m_Sizer_Inputs;
    m_Sizer_Inputs = new wxGridSizer(0, 1, 0, 0);

    wxBoxSizer* m_Sizer_Buttons;
    m_Sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

    m_Button_Back = new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_Back, 0, wxALL, 5);

    m_Button_NewItem = new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_NewItem, 0, wxALL, 5);

    m_Button_NewFolder = new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_NewFolder, 0, wxALL, 5);

    m_Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);

    m_ToggleButton_Search = new wxBitmapToggleButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(29, 29), 0);
    m_Sizer_Buttons->Add(m_ToggleButton_Search, 0, wxALL, 5);

    m_Button_ViewMode = new wxBitmapButton(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_ViewMode, 0, wxALL, 5);

    m_Sizer_Inputs->Add(m_Sizer_Buttons, 1, wxEXPAND, 5);

    m_TextCtrl_Search = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Inputs->Add(m_TextCtrl_Search, 0, wxALL | wxEXPAND, 5);

    m_Sizer_Search->Add(m_Sizer_Inputs, 1, wxEXPAND, 5);

    m_DataViewCtrl_ObjectList = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Search->Add(m_DataViewCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);

    m_DataViewListCtrl_ObjectList = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
    m_DataViewListCtrl_ObjectList->Hide();

    m_Sizer_Search->Add(m_DataViewListCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);

    this->SetSizer(m_Sizer_Search);
    this->Layout();
}

Panel_Search::~Panel_Search()
{

}