/***************************************************************
                        panel_search.cpp

TODO
***************************************************************/

#include "panel_search.h"
#include "../resource.h"
#include "../json.h"

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

    m_DataViewListCtrl_ObjectList = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES| wxDV_NO_HEADER);
    m_DataViewListColumn_Assets = m_DataViewListCtrl_ObjectList->AppendIconTextColumn(_("Assets"), wxDATAVIEW_CELL_EDITABLE, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE);
    m_DataViewListColumn_Assets = m_DataViewListCtrl_ObjectList->AppendToggleColumn(_("IsFolder"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_HIDDEN);
    m_Sizer_Search->Add(m_DataViewListCtrl_ObjectList, 0, wxALL | wxEXPAND, 5); 

    m_DataViewListCtrl_ObjectGrid = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES| wxDV_NO_HEADER);
    m_DataViewListCtrl_ObjectGrid->Hide();
    m_Sizer_Search->Add(m_DataViewListCtrl_ObjectGrid, 0, wxALL | wxEXPAND, 5);


    this->SetSizer(m_Sizer_Search);
    this->Layout();

    // Connect Events
    this->m_Button_Back->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_Back_OnButtonClick), NULL, this);
    this->m_Button_NewAsset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewAsset_OnButtonClick), NULL, this);
    this->m_Button_NewFolder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewFolder_OnButtonClick), NULL, this);
    this->m_ToggleButton_Search->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_ToggleButton_Search_OnToggleButton), NULL, this);
    this->m_Button_ViewMode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_ViewMode_OnButtonClick), NULL, this);
    this->m_TextCtrl_Search->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Panel_Search::m_TextCtrl_Search_OnText), NULL, this);
    this->m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(Panel_Search::m_DataViewListCtrl_ObjectList_OnItemActivated), NULL, this);
    this->m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE, wxDataViewEventHandler(Panel_Search::m_DataViewListCtrl_ObjectList_ItemEditingDone), NULL, this);

}

Panel_Search::~Panel_Search()
{

}

void Panel_Search::m_Button_Back_OnButtonClick(wxCommandEvent& event)
{
    this->m_CurrFolder.RemoveLastDir();
    this->LoadAssetsInDir(this->m_CurrFolder.GetPathWithSep());
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
    if (this->m_DataViewListCtrl_ObjectList->IsShown())
    {
        this->m_DataViewListCtrl_ObjectList->Hide();
        this->m_DataViewListCtrl_ObjectGrid->Show();
        this->m_Button_ViewMode->SetBitmap(Icon_ViewList);
    }
    else
    {
        this->m_DataViewListCtrl_ObjectList->Show();
        this->m_DataViewListCtrl_ObjectGrid->Hide();
        this->m_Button_ViewMode->SetBitmap(Icon_ViewGrid);
    }
    this->Layout();
    (void)event;
}

void Panel_Search::m_TextCtrl_Search_OnText(wxCommandEvent& event)
{

}

void Panel_Search::Search_SetFolder(wxFileName path)
{
    if (!wxDir::Exists(path.GetPathWithSep()))
        wxDir::Make(path.GetPathWithSep());
    this->m_MainFolder.Assign(path);
    this->m_CurrFolder.Assign(path);
    this->LoadAssetsInDir(path);
}

void Panel_Search::Search_SetAssetType(wxString type)
{
    this->m_AssetType = type;
}

void Panel_Search::Search_IconGenerator(wxIcon (*function)(bool))
{
    this->m_IconGenFunc = function;
}

void Panel_Search::LoadAssetsInDir(wxFileName path)
{
    bool cont;
    wxArrayString list;
    wxString filename;
    wxDir dir;

    // Disable the back button if we're in the root
    this->m_Button_Back->Enable(this->m_MainFolder.GetPathWithSep().Cmp(path.GetPathWithSep()));

    // Clear the list
    if (this->m_DataViewListCtrl_ObjectList->GetItemCount() > 0)
        this->m_DataViewListCtrl_ObjectList->DeleteAllItems();
    if (this->m_DataViewListCtrl_ObjectGrid->GetItemCount() > 0)
        this->m_DataViewListCtrl_ObjectGrid->DeleteAllItems();

    // List folders
    dir.Open(path.GetPathWithSep());
    cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        list.Add(filename);
        cont = dir.GetNext(&filename);
    }
    list.Sort();
    for (wxString folder : list)
    {
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(folder, Icon_Folder));
        items.push_back((wxVariant)true);
        this->m_DataViewListCtrl_ObjectList->AppendItem(items);
        items.pop_back();
    }

    // List assets
    list.Empty();
    cont = dir.GetFirst(&filename, this->m_AssetType, wxDIR_FILES);
    while (cont)
    {
        wxFileName f(filename);
        list.Add(f.GetName());
        cont = dir.GetNext(&filename);
    }
    list.Sort();
    for (wxString folder : list)
    {
        wxVector<wxVariant> items;
        if (this->m_IconGenFunc == NULL)
            items.push_back((wxVariant)wxDataViewIconText(folder, wxNullIcon));
        else
            items.push_back((wxVariant)wxDataViewIconText(folder, this->m_IconGenFunc(false)));
        items.push_back((wxVariant)false);
        this->m_DataViewListCtrl_ObjectList->AppendItem(items);
        items.pop_back();
    }

    // Done
    this->m_DataViewListCtrl_ObjectList->Refresh();
}

void Panel_Search::m_DataViewListCtrl_ObjectList_OnItemActivated(wxDataViewEvent& event)
{
    wxVariant variant;
    wxDataViewIconText icontext;
    bool isfolder;
    int row = this->m_DataViewListCtrl_ObjectList->GetSelectedRow();

    // Get the IconText data
    this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 0);
    icontext << variant;

    // Get the boolean that says if this activated item is a folder or not
    this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 1);
    isfolder = variant.GetBool();
    if (isfolder)
    {
        this->m_CurrFolder.Assign(this->m_CurrFolder.GetPathWithSep() + icontext.GetText() + wxFileName::GetPathSeparator());
        this->LoadAssetsInDir(this->m_CurrFolder.GetPathWithSep());
    }
}

void Panel_Search::m_DataViewListCtrl_ObjectList_ItemEditingDone(wxDataViewEvent& event)
{

}