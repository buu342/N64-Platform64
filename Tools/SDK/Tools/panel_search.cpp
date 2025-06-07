/***************************************************************
                        panel_search.cpp

TODO
***************************************************************/

#include "panel_search.h"
#include "../resource.h"

Panel_Search::Panel_Search(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
    wxFlexGridSizer* m_Sizer_Search;
    m_Sizer_Search = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Search->AddGrowableCol(0);
    m_Sizer_Search->AddGrowableRow(1);
    m_Sizer_Search->AddGrowableRow(2);
    m_Sizer_Search->SetFlexibleDirection(wxBOTH);
    m_Sizer_Search->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxFlexGridSizer* m_Sizer_Inputs;
    m_Sizer_Inputs = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Inputs->AddGrowableCol(0);
    m_Sizer_Inputs->SetFlexibleDirection(wxBOTH);
    m_Sizer_Inputs->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxBoxSizer* m_Sizer_Buttons;
    m_Sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

    m_Button_Back = new wxBitmapButton(this, wxID_ANY, Icon_Back, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_Back, 0, wxALL, 5);

    m_Button_NewAsset = new wxBitmapButton(this, wxID_ANY, Icon_NewAsset, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_NewAsset, 0, wxALL, 5);

    m_Button_NewFolder = new wxBitmapButton(this, wxID_ANY, Icon_NewFolder, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_NewFolder, 0, wxALL, 5);


    m_Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);

    m_ToggleButton_Search = new wxBitmapToggleButton(this, wxID_ANY, Icon_Search, wxDefaultPosition, wxSize(28, 28), 0);
    m_Sizer_Buttons->Add(m_ToggleButton_Search, 0, wxALL, 5);

    m_Button_ViewMode = new wxBitmapButton(this, wxID_ANY, Icon_ViewGrid, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    m_Sizer_Buttons->Add(m_Button_ViewMode, 0, wxALL, 5);


    m_Sizer_Inputs->Add(m_Sizer_Buttons, 1, wxEXPAND, 5);

    m_TextCtrl_Search = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_TextCtrl_Search->Hide();

    m_Sizer_Inputs->Add(m_TextCtrl_Search, 0, wxALL | wxEXPAND, 5);


    m_Sizer_Search->Add(m_Sizer_Inputs, 1, wxEXPAND, 5);

    m_DataViewCtrl_ObjectList = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER);
    m_Sizer_Search->Add(m_DataViewCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);

    m_DataViewListCtrl_ObjectList = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES| wxDV_NO_HEADER);
    m_DataViewListCtrl_ObjectList->Hide();

    m_Sizer_Search->Add(m_DataViewListCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);


    this->SetSizer(m_Sizer_Search);
    this->Layout();

    // Connect Events
    this->m_Button_Back->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_Back_OnButtonClick), NULL, this);
    this->m_Button_NewAsset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewAsset_OnButtonClick), NULL, this);
    this->m_Button_NewFolder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewFolder_OnButtonClick), NULL, this);
    this->m_ToggleButton_Search->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_ToggleButton_Search_OnToggleButton), NULL, this);
    this->m_Button_ViewMode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_ViewMode_OnButtonClick), NULL, this);
    this->m_TextCtrl_Search->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Panel_Search::m_TextCtrl_Search_OnText), NULL, this);
}

Panel_Search::~Panel_Search()
{

}

void Panel_Search::m_Button_Back_OnButtonClick(wxCommandEvent& event)
{

}

void Panel_Search::m_Button_NewAsset_OnButtonClick(wxCommandEvent& event)
{

}

void Panel_Search::m_Button_NewFolder_OnButtonClick(wxCommandEvent& event)
{

}

void Panel_Search::m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event)
{
    if (this->m_TextCtrl_Search->IsShown())
        this->m_TextCtrl_Search->Hide();
    else
        this->m_TextCtrl_Search->Show();
    this->Layout();
    (void)event;
}

void Panel_Search::m_Button_ViewMode_OnButtonClick(wxCommandEvent& event)
{
    if (this->m_DataViewCtrl_ObjectList->IsShown())
    {
        this->m_DataViewCtrl_ObjectList->Hide();
        this->m_DataViewListCtrl_ObjectList->Show();
        this->m_Button_ViewMode->SetBitmap(Icon_ViewList);
    }
    else
    {
        this->m_DataViewCtrl_ObjectList->Show();
        this->m_DataViewListCtrl_ObjectList->Hide();
        this->m_Button_ViewMode->SetBitmap(Icon_ViewGrid);
    }
    this->Layout();
    (void)event;
}

void Panel_Search::m_TextCtrl_Search_OnText(wxCommandEvent& event)
{

}