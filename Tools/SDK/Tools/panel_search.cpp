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
    this->m_TargetFrame = NULL;

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
    wxFileName curr = this->m_CurrFolder;
    curr.RemoveLastDir();
    this->SetCurrentFolder(curr);
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    event.Skip();
}

void Panel_Search::m_Button_NewAsset_OnButtonClick(wxCommandEvent& event)
{
    this->CreateNewAsset();
    event.Skip();
}

void Panel_Search::m_Button_NewFolder_OnButtonClick(wxCommandEvent& event)
{
    this->CreateNewFolder();
    event.Skip();
}

void Panel_Search::m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event)
{
    if (this->m_TextCtrl_Search->IsShown())
    {
        this->m_TextCtrl_Search->Hide();
        if (this->m_TextCtrl_Search->GetValue().Length() > 0)
            this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    }
    else
    {
        this->m_TextCtrl_Search->Show();
        if (this->m_TextCtrl_Search->GetValue().Length() > 0)
            this->m_Display_Current->LoadDirectory(this->m_CurrFolder, this->m_TextCtrl_Search->GetValue());
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
    event.Skip(false); // False is needed because for some reason SetBitmap causes OnButtonClick to fire twice on the first click
}

void Panel_Search::m_TextCtrl_Search_OnText(wxCommandEvent& event)
{
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder, event.GetString());
    event.Skip();
}

void Panel_Search::SetMainFolder(wxFileName path)
{
    this->m_MainFolder = path;
    this->SetCurrentFolder(path);
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
    this->m_Display_List->SetIconGenerator(function);
    this->m_Display_Grid->SetIconGenerator(function);
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

void Panel_Search::SetCurrentFolder(wxFileName path)
{
    this->m_CurrFolder = path;
    OutputDebugStringA((const char*)wxString(this->m_CurrFolder.GetPathWithSep() + " " + this->m_MainFolder.GetPathWithSep() + "\n").c_str());
    this->m_Button_Back->Enable(this->m_CurrFolder.GetPathWithSep().Cmp(this->m_MainFolder.GetPathWithSep()));
}

void Panel_Search::ClearSearchbox()
{
    this->m_TextCtrl_Search->SetValue(wxEmptyString);
}

wxFileName Panel_Search::GetMainFolder()
{
    return this->m_MainFolder;
}

wxFileName Panel_Search::GetCurrentFolder()
{
    return this->m_CurrFolder;
}

wxString Panel_Search::GetAssetType()
{
    return this->m_AssetType;
}

wxString Panel_Search::GetAssetExtension(bool asterisk)
{
    return asterisk ? this->m_AssetExt : this->m_AssetExt_NoAsterisk;
}

wxFrame* Panel_Search::GetTargetFrame()
{
    return this->m_TargetFrame;
}

void Panel_Search::CreateNewAsset()
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
        } while (wxFileName(this->m_CurrFolder.GetPathWithSep() + testname).Exists());
        name = testname;
    }
    else
        name += this->m_AssetExt_NoAsterisk;
    this->m_NewAssetFunc(this->m_TargetFrame, this->m_CurrFolder.GetPathWithSep() + name);
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    this->m_Display_Current->SelectItem(name, false, true);
}

void Panel_Search::CreateNewFolder()
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
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    this->m_Display_Current->SelectItem(name, true, true);
}

void Panel_Search::LoadAsset(wxFileName path)
{
    this->m_LoadAssetFunc(this->m_TargetFrame, path);
}

void Panel_Search::RenameAsset(wxFileName oldpath, wxFileName newpath)
{
    this->m_RenameAssetFunc(this->m_TargetFrame, oldpath, newpath);
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

    // Try to open the directory
    dir.Open(path.GetPathWithSep());
    if (!dir.IsOpened())
        return false;

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
    cont = dir.GetFirst(&filename, parent->GetAssetExtension(true), wxDIR_FILES);
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

void Panel_AssetDisplay::SelectItem(wxString itemname, bool isfolder, bool rename)
{
    (void)itemname;
    (void)isfolder;
    (void)rename;
}


/*==============================
    Panel_AssetDisplay::StartThread_IconGenerator
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
    Panel_AssetDisplay::StopThread_IconGenerator
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

void Panel_AssetDisplay::SetIconGenerator(wxIcon(*function)(bool, wxFileName))
{
    this->m_IconGenFunc = function;
}

wxMessageQueue<ThreadWork*>* Panel_AssetDisplay::GetThreadQueue()
{
    return &this->m_ThreadQueue;
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
        wxFileName newpath = parent->GetCurrentFolder().GetPathWithSep() + icontext.GetText() + wxFileName::GetPathSeparator();
        if (this->LoadDirectory(newpath))
            parent->SetCurrentFolder(newpath);
    }
    else
        parent->LoadAsset(parent->GetCurrentFolder().GetPathWithSep() + icontext.GetText() + parent->GetAssetExtension());

    // Clear the search bar
    parent->ClearSearchbox();
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
                                wxString path = parent->GetCurrentFolder().GetPathWithSep() + it.GetText();
                                if (wxDir::Remove(path, wxPATH_RMDIR_RECURSIVE))
                                    this->m_DataViewListCtrl_ObjectList->DeleteItem(row);
                            }
                        }
                        else
                        {
                            wxMessageDialog dialog(this, wxString::Format("Are you sure you want to delete the asset '%s'?", it.GetText()), "Delete?", wxCENTER | wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
                            if (dialog.ShowModal() == wxID_YES)
                            {
                                wxString path = parent->GetCurrentFolder().GetPathWithSep() + it.GetText() + parent->GetAssetExtension();
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
                case 1: parent->CreateNewAsset(); break;
                case 2: parent->CreateNewFolder(); break;
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
        oldname = parent->GetCurrentFolder().GetPathWithSep() + oldicontext.GetText();
        newname = parent->GetCurrentFolder().GetPathWithSep() + newicontext.GetText();
        if (!isfolder)
        {
            oldname += parent->GetAssetExtension();
            newname += parent->GetAssetExtension();
        }
        if (((isfolder && wxDirExists(newname)) || (!isfolder && wxFileExists(newname))) || !wxRenameFile(oldname, newname, false))
        {
            event.Veto();
            return;
        }
        parent->RenameAsset(oldname, newname);

        // Reload the list and select the renamed item
        // Trust me, trying to "sort it" by using InsertItem is going to lead to headaches, so it's easier to just reload the entire directory
        event.Veto();
        this->LoadDirectory(parent->GetCurrentFolder());
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
    event.Skip();
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
        this->m_ThreadQueue.Post(new ThreadWork{wxFileName(path.GetPathWithSep() + f + parent->GetAssetExtension()), false});
    }
    return true;
}

void Panel_AssetDisplay_List::SelectItem(wxString itemname, bool isfolder, bool rename)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    for (int i=0; i<this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
    {
        wxVariant variant;
        wxDataViewIconText icontext;
        wxString finalname;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
        icontext << variant;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 1);
        finalname = icontext.GetText();
        if (!variant.GetBool())
            finalname += parent->GetAssetExtension();
        if (variant.GetBool() == isfolder && !finalname.Cmp(itemname))
        {
            if (rename)
                this->m_DataViewListCtrl_ObjectList->EditItem(this->m_DataViewListCtrl_ObjectList->RowToItem(i), this->m_DataViewListCtrl_ObjectList->GetColumn(0));
            else
                this->m_DataViewListCtrl_ObjectList->SelectRow(i);
            break;
        }
    }
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
                icontext.SetIcon(iconresult->icon);
                variant << icontext;
                this->m_DataViewListCtrl_ObjectList->SetValue(variant, i, 0);
                break;
            }
        }
    }
    event.Skip();
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
        this->m_ThreadQueue.Post(new ThreadWork{wxFileName(path.GetPathWithSep() + f + parent->GetAssetExtension()), true});
    return true;
}

void Panel_AssetDisplay_Grid::SelectItem(wxString itemname, bool isfolder, bool rename)
{
    (void)itemname;
    (void)isfolder;
    (void)rename;
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
        while (this->m_Parent->GetThreadQueue()->ReceiveTimeout(0, t) == wxMSGQUEUE_NO_ERROR)
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