/***************************************************************
                        panel_search.cpp

TODO
***************************************************************/

#include "panel_search.h"
#include "../resource.h"
#include "../json.h"
#include <wx/msgqueue.h>


/*=============================================================
                         Search Panel
=============================================================*/

Panel_Search::Panel_Search(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
    this->m_MainFolder = wxFileName("");
    this->m_NewAssetFunc = NULL;
    this->m_LoadAssetFunc = NULL;
    this->m_RenameAssetFunc = NULL;

    wxFlexGridSizer* m_Sizer_Search;
    m_Sizer_Search = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_Search->AddGrowableCol( 0 );
    m_Sizer_Search->AddGrowableRow( 1 );
    m_Sizer_Search->AddGrowableRow( 2 );
    m_Sizer_Search->SetFlexibleDirection( wxBOTH );
    m_Sizer_Search->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxFlexGridSizer* m_Sizer_Inputs;
    m_Sizer_Inputs = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_Inputs->AddGrowableCol( 0 );
    m_Sizer_Inputs->SetFlexibleDirection( wxBOTH );
    m_Sizer_Inputs->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxBoxSizer* m_Sizer_Buttons;
    m_Sizer_Buttons = new wxBoxSizer( wxHORIZONTAL );

    this->m_Button_Back = new wxBitmapButton(this, wxID_ANY, Icon_Back, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_Back->SetToolTip(_("Go back a folder"));
    m_Sizer_Buttons->Add(this->m_Button_Back, 0, wxALL, 5);

    this->m_Button_NewAsset = new wxBitmapButton(this, wxID_ANY, Icon_NewAsset, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_NewAsset->SetToolTip(_("Create a new asset"));
    m_Sizer_Buttons->Add(this->m_Button_NewAsset, 0, wxALL, 5);

    this->m_Button_NewFolder = new wxBitmapButton(this, wxID_ANY, Icon_NewFolder, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_NewFolder->SetToolTip(_("Create a new folder"));
    m_Sizer_Buttons->Add(this->m_Button_NewFolder, 0, wxALL, 5);


    m_Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);

    this->m_ToggleButton_Search = new wxBitmapToggleButton(this, wxID_ANY, Icon_Search, wxDefaultPosition, wxSize(28, 28), 0);
    this->m_ToggleButton_Search->SetToolTip(_("Toggle the search bar"));
    m_Sizer_Buttons->Add(this->m_ToggleButton_Search, 0, wxALL, 5);

    this->m_Button_ViewMode = new wxBitmapButton(this, wxID_ANY, Icon_ViewGrid, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_ViewMode->SetToolTip(_("Change the view mode"));
    m_Sizer_Buttons->Add(this->m_Button_ViewMode, 0, wxALL, 5);


    m_Sizer_Inputs->Add( m_Sizer_Buttons, 1, wxEXPAND, 5 );

    m_TextCtrl_Search = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TextCtrl_Search->Hide();

    m_Sizer_Inputs->Add(m_TextCtrl_Search, 0, wxALL|wxEXPAND, 5);

    m_Sizer_Search->Add(m_Sizer_Inputs, 1, wxEXPAND, 5);

    this->m_Display_List = new Panel_AssetDisplay_List(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_Sizer_Search->Add(this->m_Display_List, 1, wxEXPAND | wxALL, 5);
    this->m_Display_Grid = new Panel_AssetDisplay_Grid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_Sizer_Search->Add(this->m_Display_Grid, 1, wxEXPAND | wxALL, 5);
    this->m_Display_Grid->Hide();
    this->m_Display_Current = this->m_Display_List;

    this->SetSizer( m_Sizer_Search );
    this->Layout();

    // Connect Events
    m_Button_Back->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Panel_Search::m_Button_Back_OnButtonClick ), NULL, this );
    m_Button_NewAsset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Panel_Search::m_Button_NewAsset_OnButtonClick ), NULL, this );
    m_Button_NewFolder->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Panel_Search::m_Button_NewFolder_OnButtonClick ), NULL, this );
    m_ToggleButton_Search->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Panel_Search::m_ToggleButton_Search_OnToggleButton ), NULL, this );
    m_Button_ViewMode->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Panel_Search::m_Button_ViewMode_OnButtonClick ), NULL, this );
    m_TextCtrl_Search->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Panel_Search::m_TextCtrl_Search_OnText ), NULL, this );
}

Panel_Search::~Panel_Search()
{

}

void Panel_Search::m_Button_Back_OnButtonClick(wxCommandEvent& event)
{
    this->m_CurrFolder.RemoveLastDir();
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    event.Skip();
}

void Panel_Search::m_Button_NewAsset_OnButtonClick(wxCommandEvent& event)
{
    wxString name = wxString("New ") + this->m_AssetType;
    if (wxFileName(this->m_CurrFolder.GetPathWithSep() + name + this->m_AssetExt_NoAsterisk).Exists())
    {
        wxString testname;
        int i = 2;
        do
        {
            testname = wxString::Format("%s (%d)%s", name, i, this->m_AssetExt_NoAsterisk);
            i++;
        } 
        while (wxFileName(this->m_CurrFolder.GetPathWithSep() + testname).Exists());
        name = testname;
    }
    else
        name += this->m_AssetExt_NoAsterisk;
    this->m_NewAssetFunc(this->m_TargetFrame, this->m_CurrFolder.GetPathWithSep() + name);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);

    // TODO: Select the item and make the textbox editable
    /*
    for (int i=0; i<this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
    {
        wxVariant variant;
        wxDataViewIconText icontext;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
        icontext << variant;
        if ((icontext.GetText() + extwithoutasterisk) == name)
        {
            this->m_DataViewListCtrl_ObjectList->EditItem(this->m_DataViewListCtrl_ObjectList->RowToItem(i), this->m_DataViewListCtrl_ObjectList->GetColumn(0));
            break;
        }
    }
    */
    event.Skip();
}

void Panel_Search::m_Button_NewFolder_OnButtonClick(wxCommandEvent& event)
{
    wxString name = "New folder";
    if (wxDir::Exists(this->m_CurrFolder.GetPathWithSep() + name))
    {
        wxString testname;
        int i = 2;
        do
        {
            testname = wxString::Format("%s (%d)", name, i);
            i++;
        } while (wxDir::Exists(this->m_CurrFolder.GetPathWithSep() + testname));
        name = testname;
    }
    wxDir::Make(this->m_CurrFolder.GetPathWithSep() + name);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);

    // TODO: Select the item and make the textbox editable
    /*
    this->SelectItem(name, true);
    for (int i = 0; i < this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
    {
        wxVariant variant;
        wxDataViewIconText icontext;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
        icontext << variant;
        if (icontext.GetText() == name)
        {
            this->m_DataViewListCtrl_ObjectList->EditItem(this->m_DataViewListCtrl_ObjectList->RowToItem(i), this->m_DataViewListCtrl_ObjectList->GetColumn(0));
            break;
        }
    }
    */
    event.Skip();
}

void Panel_Search::m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event)
{
    if (this->m_TextCtrl_Search->IsShown())
    {
        this->m_TextCtrl_Search->Hide();
        //if (this->m_TextCtrl_Search->GetValue().Length() > 0)
        //    this->LoadAssetsInDir(this->m_CurrFolder);
    }
    else
    {
        this->m_TextCtrl_Search->Show();
        //if (this->m_TextCtrl_Search->GetValue().Length() > 0)
        //    this->LoadAssetsInDir(this->m_CurrFolder, this->m_TextCtrl_Search->GetValue());
    }
    this->Layout();

    event.Skip();
}

void Panel_Search::m_Button_ViewMode_OnButtonClick(wxCommandEvent& event)
{
    this->m_Display_Current->Hide();
    if (this->m_Display_Current == this->m_Display_List)
    {
        this->m_Button_ViewMode->SetBitmap(Icon_ViewList);
        this->m_Display_Current = this->m_Display_Grid;
    }
    else
    {
        this->m_Button_ViewMode->SetBitmap(Icon_ViewGrid);
        this->m_Display_Current = this->m_Display_List;
    }
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    this->m_Display_Current->Show();
    this->Layout();
    event.Skip();
}

void Panel_Search::m_TextCtrl_Search_OnText(wxCommandEvent& event)
{
    event.Skip();
}

wxFileName Panel_Search::GetMainFolder()
{
    return this->m_MainFolder;
}

void Panel_Search::SetMainFolder(wxFileName path)
{
    this->m_MainFolder = path;
    this->m_CurrFolder = path;
    this->m_Display_Current->LoadDirectory(path);
}

void Panel_Search::SetAssetType(wxString type, wxString ext)
{
    this->m_AssetType = type;
    this->m_AssetExt = ext;
    this->m_AssetExt_NoAsterisk = ext.SubString(1, ext.Length() - 1);
}

void Panel_Search::SetIconGenerator(wxIcon (*function)(bool, wxFileName))
{
    this->m_Display_List->m_IconGenFunc = function;
    this->m_Display_Grid->m_IconGenFunc = function;
}

void Panel_Search::SetAssetGenerator(void (*function)(wxFrame*, wxFileName))
{
    this->m_NewAssetFunc = function;
}

void Panel_Search::SetLoadAssetFunc(void (*function)(wxFrame*, wxFileName))
{
    this->m_LoadAssetFunc = function;
}

void Panel_Search::SetRenameAssetFunc(void (*function)(wxFrame*, wxFileName, wxFileName))
{
    this->m_RenameAssetFunc = function;
}

void Panel_Search::SetTargetFrame(wxFrame* target)
{
    this->m_TargetFrame = target;
}


/*=============================================================
                   Asset Display Base Class
=============================================================*/

Panel_AssetDisplay::Panel_AssetDisplay(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
    this->m_IconGenFunc = NULL;
    this->m_IconGeneratorThread = NULL;
    this->Connect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay::ThreadEvent));
}

Panel_AssetDisplay::~Panel_AssetDisplay()
{
    this->StopThread_IconGenerator();
    this->Disconnect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay::ThreadEvent));
}

bool Panel_AssetDisplay::LoadDirectory(wxFileName path, wxString filter)
{
    bool cont;
    wxArrayString list;
    wxString filename;
    wxDir dir;
    Panel_Search* parent = (Panel_Search*)this->GetParent();

    // Try to oepn the directory
    dir.Open(path.GetPathWithSep());
    if (!dir.IsOpened())
        return false;

    // Disable the back button if we're in the root
    parent->m_Button_Back->Enable(parent->GetMainFolder().GetPathWithSep().Cmp(path.GetPathWithSep()));

    // Clear the lists
    this->m_Assets.clear();
    this->m_Folders.clear();

    // List folders
    cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        if (filter == wxEmptyString || filename.Lower().Contains(filter.Lower()))
            list.Add(filename);
        cont = dir.GetNext(&filename);
    }
    list.Sort();
    for (wxString folder : list)
        this->m_Folders.push_back(folder);

    // List assets
    list.Empty();
    cont = dir.GetFirst(&filename, parent->m_AssetExt, wxDIR_FILES);
    while (cont)
    {
        if (filter == wxEmptyString || filename.Lower().Contains(filter.Lower()))
        {
            wxFileName f(filename);
            list.Add(f.GetName());
        }
        cont = dir.GetNext(&filename);
    }
    list.Sort();
    for (wxString asset : list)
        this->m_Assets.push_back(asset);

    // Success
    return true;
}


/*==============================
    ServerBrowser::StartThread_IconGenerator
    Starts the icon generator thread
==============================*/

void Panel_AssetDisplay::StartThread_IconGenerator()
{
    if (this->m_IconGeneratorThread == NULL)
    {
        this->m_IconGeneratorThread = new IconGeneratorThread(this, this->m_IconGenFunc);
        if (this->m_IconGeneratorThread->Run() != wxTHREAD_NO_ERROR)
        {
            delete this->m_IconGeneratorThread;
            this->m_IconGeneratorThread = NULL;
        }
    }
}


/*==============================
    ServerBrowser::StopThread_IconGenerator
    Stops the icon generator thread
==============================*/

void Panel_AssetDisplay::StopThread_IconGenerator()
{
    if (this->m_IconGeneratorThread != NULL)
    {
        this->m_IconGeneratorThread->Delete();
        delete this->m_IconGeneratorThread;
        this->m_IconGeneratorThread = NULL;
    }
}

void Panel_AssetDisplay::ThreadEvent(wxThreadEvent& event)
{
    event.Skip();
}


/*=============================================================
                      Asset Display List
=============================================================*/

Panel_AssetDisplay_List::Panel_AssetDisplay_List(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : Panel_AssetDisplay(parent, id, pos, size, style)
{
    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer(1, 1, 0, 0);
    m_Sizer_Main->AddGrowableCol(0);
    m_Sizer_Main->AddGrowableRow(0);
    m_Sizer_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_DataViewListCtrl_ObjectList = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_ROW_LINES);
    m_DataViewListColumn_Assets = m_DataViewListCtrl_ObjectList->AppendIconTextColumn(_("Assets"), wxDATAVIEW_CELL_EDITABLE, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE);
    m_DataViewListColumn_IsFolder = m_DataViewListCtrl_ObjectList->AppendIconTextColumn(_("Assets"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_HIDDEN);
    m_Sizer_Main->Add(m_DataViewListCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);

    this->SetSizer(m_Sizer_Main);
    this->Layout();

    // Connect Events
    m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_OnItemActivated), NULL, this);
    m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ContextMenu), NULL, this);
    m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ItemEditingDone), NULL, this);
    this->Connect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay_List::ThreadEvent));
}

Panel_AssetDisplay_List::~Panel_AssetDisplay_List()
{

}

void Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_OnItemActivated(wxDataViewEvent& event)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    wxVariant variant;
    wxDataViewIconText icontext;
    bool isfolder;
    int row = this->m_DataViewListCtrl_ObjectList->ItemToRow(event.GetItem());

    // Get the IconText data
    this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 0);
    icontext << variant;

    // Get the boolean that says if this activated item is a folder or not
    this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 1);
    isfolder = variant.GetBool();

    // Handle activation
    if (isfolder)
    {
        wxFileName newpath = parent->m_CurrFolder.GetPathWithSep() + icontext.GetText() + wxFileName::GetPathSeparator();
        if (this->LoadDirectory(newpath))
            parent->m_CurrFolder.Assign(newpath);
    }
    else
    {
        wxString extwithoutasterisk = parent->m_AssetExt.SubString(1, parent->m_AssetExt.Length() - 1);
        parent->m_LoadAssetFunc(parent->m_TargetFrame, parent->m_CurrFolder.GetPathWithSep() + icontext.GetText() + extwithoutasterisk);
    }

    // Clear the search bar
    parent->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    event.Skip();
}

void Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item;
    wxDataViewColumn col = wxDataViewColumn("", new wxDataViewIconTextRenderer(), 0);
    wxDataViewColumn* colptr = &col;
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    wxPoint hit = this->m_DataViewListCtrl_ObjectList->ScreenToClient(wxGetMousePosition());
    this->m_DataViewListCtrl_ObjectList->HitTest(hit, item, colptr);
    if (item.IsOk())
    {
        wxMenu menu;
        menu.Append(1, "Rename");
        menu.Append(2, "Delete");
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent& event) {
            switch (event.GetId())
            {
                case 1: 
                    this->m_DataViewListCtrl_ObjectList->EditItem(item, this->m_DataViewListCtrl_ObjectList->GetColumn(0));
                    break;
                case 2: 
                    {
                        wxVariant v;
                        bool isfolder;
                        wxDataViewIconText it;
                        int row = this->m_DataViewListCtrl_ObjectList->ItemToRow(item);
                        this->m_DataViewListCtrl_ObjectList->GetValue(v, row, 0);
                        it << v;
                        this->m_DataViewListCtrl_ObjectList->GetValue(v, row, 1);
                        isfolder = v.GetBool();
                        if (isfolder)
                        {
                            wxMessageDialog dialog(this, wxString::Format("Are you sure you want to delete the directory '%s' and all of its contents?", it.GetText()), "Delete?", wxCENTER | wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
                            if (dialog.ShowModal() == wxID_YES)
                            {
                                wxString path = parent->m_CurrFolder.GetPathWithSep() + it.GetText();
                                if (wxDir::Remove(path, wxPATH_RMDIR_RECURSIVE))
                                    this->m_DataViewListCtrl_ObjectList->DeleteItem(row);
                            }
                        }
                        else
                        {
                            wxMessageDialog dialog(this, wxString::Format("Are you sure you want to delete the asset '%s'?", it.GetText()), "Delete?", wxCENTER | wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
                            if (dialog.ShowModal() == wxID_YES)
                            {
                                wxString path = parent->m_CurrFolder.GetPathWithSep() + it.GetText() + parent->m_AssetExt_NoAsterisk;
                                if (wxRemoveFile(path))
                                    this->m_DataViewListCtrl_ObjectList->DeleteItem(row);
                            }
                        }
                    }
                    break;
            }
            event.Skip();
        });
        PopupMenu(&menu);
    }
    else
    {
        wxMenu menu;
        menu.Append(1, "New Asset");
        menu.Append(2, "New Folder");
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent& event) {
            wxCommandEvent evt;
            switch (event.GetId())
            {
                case 1: parent->m_Button_NewAsset_OnButtonClick(evt); break;
                case 2: parent->m_Button_NewFolder_OnButtonClick(evt); break;
            }
            event.Skip();
        });
        PopupMenu(&menu);
    }
    event.Skip();
}

void Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ItemEditingDone(wxDataViewEvent& event)
{
    if (!event.IsEditCancelled())
    {
        int row;
        wxVariant variant;
        wxDataViewIconText oldicontext, newicontext;
        wxString oldname, newname;
        bool isfolder;
        Panel_Search* parent = (Panel_Search*)this->GetParent();

        // Get the old data
        row = this->m_DataViewListCtrl_ObjectList->ItemToRow(event.GetItem());
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 0);
        oldicontext << variant;

        // Check if we're modifying a folder
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, row, 1);
        isfolder = variant.GetBool();

        // Get the new data
        variant = event.GetValue();
        newicontext << variant;

        // Perform the rename
        oldname = parent->m_CurrFolder.GetPathWithSep() + oldicontext.GetText();
        newname = parent->m_CurrFolder.GetPathWithSep() + newicontext.GetText();
        if (!isfolder)
        {
            wxString extwithoutasterisk = parent->m_AssetExt_NoAsterisk;
            oldname += extwithoutasterisk;
            newname += extwithoutasterisk;
        }
        if (((isfolder && wxDirExists(newname)) || (!isfolder && wxFileExists(newname))) || !wxRenameFile(oldname, newname, false))
        {
            event.Veto();
            return;
        }
        parent->m_RenameAssetFunc(parent->m_TargetFrame, oldname, newname);

        // Reload the list and select the renamed item
        // Trust me, trying to "sort it" by using InsertItem is going to lead to headaches, so it's easier to just reload the entire directory
        event.Veto();
        this->LoadDirectory(parent->m_CurrFolder);
        for (int i = 0; i < this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
        {
            wxDataViewItem item = this->m_DataViewListCtrl_ObjectList->RowToItem(i);
            this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
            oldicontext << variant;
            if (oldicontext.GetText() == newicontext.GetText())
            {
                this->m_DataViewListCtrl_ObjectList->Select(item);
                break;
            }
        }
    }
}

bool Panel_AssetDisplay_List::LoadDirectory(wxFileName path, wxString filter)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    
    // Call the base class's load directory function
    this->m_DataViewListCtrl_ObjectList->DeleteAllItems();
    if (Panel_AssetDisplay::LoadDirectory(path, filter) == false)
        return false;

    // Add folders to the view
    for (wxString folder : this->m_Folders)
    {
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(folder, Icon_Folder));
        items.push_back((wxVariant)true);
        this->m_DataViewListCtrl_ObjectList->AppendItem(items);
    }

    // Add assets to the view and queue the icon generation
    this->StartThread_IconGenerator();
    for (wxString f : this->m_Assets)
    {
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(f, wxNullIcon));
        items.push_back((wxVariant)false);
        this->m_DataViewListCtrl_ObjectList->AppendItem(items);
        this->m_ThreadQueue.Post(new ThreadWork{wxFileName(path.GetPathWithSep() + f + parent->m_AssetExt_NoAsterisk), false});
    }
    return true;
}

void Panel_AssetDisplay_List::ThreadEvent(wxThreadEvent& event)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    ThreadResult* iconresult = event.GetPayload<ThreadResult*>();
    for (int i=0; i<this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
    {
        bool isfolder;
        wxVariant variant;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 1);
        isfolder = variant.GetBool();
        if (!isfolder)
        {
            wxDataViewIconText icontext;
            this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
            icontext << variant;
            if (icontext.GetText() == iconresult->file.GetName())
            {
                wxVector<wxVariant> newitem;
                bool wasselected = this->m_DataViewListCtrl_ObjectList->IsRowSelected(i);
                icontext.SetIcon(iconresult->icon);
                this->m_DataViewListCtrl_ObjectList->DeleteItem(i);
                newitem.push_back((wxVariant)icontext);
                newitem.push_back((wxVariant)isfolder);
                this->m_DataViewListCtrl_ObjectList->InsertItem(i, newitem);
                newitem.pop_back();
                if (wasselected)
                    this->m_DataViewListCtrl_ObjectList->SelectRow(i);
                break;
            }
        }
    }
}


/*=============================================================
                      Asset Display Grid
=============================================================*/

Panel_AssetDisplay_Grid::Panel_AssetDisplay_Grid(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : Panel_AssetDisplay(parent, id, pos, size, style)
{

}

Panel_AssetDisplay_Grid::~Panel_AssetDisplay_Grid()
{

}

bool Panel_AssetDisplay_Grid::LoadDirectory(wxFileName path, wxString filter)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    
    // Call the base class's load directory function
    if (Panel_AssetDisplay::LoadDirectory(path, filter) == false)
        return false;

    // Generate icons
    for (wxString f : this->m_Assets)
        this->m_ThreadQueue.Post(new ThreadWork{wxFileName(path.GetPathWithSep() + f + parent->m_AssetExt_NoAsterisk), true});
    return true;
}


/*=============================================================
                     Icon Generator Thread
=============================================================*/

/*==============================
    IconGeneratorThread (Constructor)
    Initializes the class
==============================*/

IconGeneratorThread::IconGeneratorThread(Panel_AssetDisplay* parent, wxIcon(*func)(bool, wxFileName)) : wxThread(wxTHREAD_JOINABLE)
{
    this->m_Parent = parent;
    this->m_IconGenFunc = func;
}


/*==============================
    IconGeneratorThread (Destructor)
    Cleans up the class before deletion
==============================*/

IconGeneratorThread::~IconGeneratorThread()
{

}


/*==============================
    IconGeneratorThread::Entry
    The entry function for the thread
    @return The exit code
==============================*/

void* IconGeneratorThread::Entry()
{
    while (!TestDestroy())
    {
        ThreadWork* t;
        while (this->m_Parent->m_ThreadQueue.ReceiveTimeout(0, t) == wxMSGQUEUE_NO_ERROR)
        {
            wxIcon i = this->m_IconGenFunc(t->large, t->file);
            wxThreadEvent evt = wxThreadEvent(wxEVT_THREAD, wxID_ANY);
            ThreadResult* r = new ThreadResult();
            r->file = t->file;
            r->icon = i;
            evt.SetPayload<ThreadResult*>(r);
            wxQueueEvent(this->m_Parent, evt.Clone());
        }
    }
    return NULL;
}