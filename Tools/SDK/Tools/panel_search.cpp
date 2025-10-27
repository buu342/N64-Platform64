/***************************************************************
                        panel_search.cpp

This file provides an asset and folder browser which filters 
based on a specific extension. The search panel allows you see
files in either a small list format or a large grid format. The
preview icon for each asset is generated in a separate thread
and cached for the best possible user experience.
***************************************************************/

#include "panel_search.h"
#include "../resource.h"
#include "../json.h"
#include <wx/msgqueue.h>
#include <wx/textwrapper.h>


/*=============================================================
                            Macros
=============================================================*/

#define ICONCACHESIZE 100


/*=============================================================
                         Search Panel
=============================================================*/

/*==============================
    Panel_Search (Constructor)
    Initializes the class
    @param The parent window
    @param The panel's ID (default wxID_ANY)
    @param The panel's position (default wxDefaultPosition)
    @param The panel's size (default wxDefaultSize)
    @param The panel's style (default wxTAB_TRAVERSAL)
    @param The panel's name (default wxEmptyString)
==============================*/

Panel_Search::Panel_Search(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
    // Initialize object members
    this->m_MainFolder = wxFileName("");
    this->m_CurrFolder = wxFileName("");
    this->m_TargetFrame = NULL;
    this->m_AssetType = wxString("");
    this->m_AssetExt = wxString("");
    this->m_AssetExt_NoAsterisk = wxString("");
    this->m_NewAssetFunc = NULL;
    this->m_LoadAssetFunc = NULL;
    this->m_RenameAssetFunc = NULL;

    // Create the sizer for the search bar
    wxFlexGridSizer* Sizer_Search;
    Sizer_Search = new wxFlexGridSizer(0, 1, 0, 0);
    Sizer_Search->AddGrowableCol(0);
    Sizer_Search->AddGrowableRow(1);
    Sizer_Search->AddGrowableRow(2);
    Sizer_Search->SetFlexibleDirection(wxBOTH);
    Sizer_Search->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Create the sizer for the inputs (buttons + search bar)
    wxFlexGridSizer* Sizer_Inputs;
    Sizer_Inputs = new wxFlexGridSizer(0, 1, 0, 0);
    Sizer_Inputs->AddGrowableCol(0);
    Sizer_Inputs->SetFlexibleDirection(wxBOTH);
    Sizer_Inputs->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Create the sizer for the buttons
    wxBoxSizer* Sizer_Buttons;
    Sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

    // Create the buttons
    this->m_Button_Back = new wxBitmapButton(this, wxID_ANY, Icon_Back, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_Back->SetToolTip(_("Go back a folder"));
    Sizer_Buttons->Add(this->m_Button_Back, 0, wxALL, 5);
    this->m_Button_NewAsset = new wxBitmapButton(this, wxID_ANY, Icon_NewAsset, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_NewAsset->SetToolTip(_("Create a new asset"));
    Sizer_Buttons->Add(this->m_Button_NewAsset, 0, wxALL, 5);
    this->m_Button_NewFolder = new wxBitmapButton(this, wxID_ANY, Icon_NewFolder, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_NewFolder->SetToolTip(_("Create a new folder"));
    Sizer_Buttons->Add(this->m_Button_NewFolder, 0, wxALL, 5);
    Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);
    this->m_ToggleButton_Search = new wxBitmapToggleButton(this, wxID_ANY, Icon_Search, wxDefaultPosition, wxSize(28, 28), 0);
    this->m_ToggleButton_Search->SetToolTip(_("Toggle the search bar"));
    Sizer_Buttons->Add(this->m_ToggleButton_Search, 0, wxALL, 5);
    this->m_Button_ViewMode = new wxBitmapButton(this, wxID_ANY, Icon_ViewGrid, wxDefaultPosition, wxSize(28, 28), wxBU_AUTODRAW | 0);
    this->m_Button_ViewMode->SetToolTip(_("Change the view mode"));
    Sizer_Buttons->Add(this->m_Button_ViewMode, 0, wxALL, 5);
    Sizer_Inputs->Add(Sizer_Buttons, 1, wxEXPAND, 5);

    // Add the search bar and hide it by default
    this->m_TextCtrl_Search = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Search->Hide();
    Sizer_Inputs->Add(this->m_TextCtrl_Search, 0, wxALL|wxEXPAND, 5);
    Sizer_Search->Add(Sizer_Inputs, 1, wxEXPAND, 5);

    // Create the asset display panels and add them to the search sizer
    this->m_Display_List = new Panel_AssetDisplay_List(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    Sizer_Search->Add(this->m_Display_List, 1, wxEXPAND | wxALL, 5);
    this->m_Display_Grid = new Panel_AssetDisplay_Grid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    Sizer_Search->Add(this->m_Display_Grid, 1, wxEXPAND | wxALL, 5);
    this->m_Display_Grid->Hide();
    this->m_Display_Current = this->m_Display_List;

    // Finalize the layout
    this->SetSizer(Sizer_Search);
    this->Layout();

    // Connect events
    m_Button_Back->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_Back_OnButtonClick), NULL, this);
    m_Button_NewAsset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewAsset_OnButtonClick), NULL, this);
    m_Button_NewFolder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_NewFolder_OnButtonClick), NULL, this);
    m_ToggleButton_Search->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_ToggleButton_Search_OnToggleButton), NULL, this);
    m_Button_ViewMode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Panel_Search::m_Button_ViewMode_OnButtonClick), NULL, this);
    m_TextCtrl_Search->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Panel_Search::m_TextCtrl_Search_OnText), NULL, this);
}


/*==============================
    Panel_Search (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_Search::~Panel_Search()
{
    // Nothing to clean up
}


/*==============================
    Panel_Search::m_Button_Back_OnButtonClick
    Handles pressing the back button
    @param The command event
==============================*/

void Panel_Search::m_Button_Back_OnButtonClick(wxCommandEvent& event)
{
    wxFileName curr = this->m_CurrFolder;
    curr.RemoveLastDir();
    this->SetCurrentFolder(curr);
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    event.Skip();
}


/*==============================
    Panel_Search::m_Button_NewAsset_OnButtonClick
    Handles pressing the new asset button
    @param The command event
==============================*/

void Panel_Search::m_Button_NewAsset_OnButtonClick(wxCommandEvent& event)
{
    this->CreateNewAsset();
    event.Skip();
}


/*==============================
    Panel_Search::m_Button_NewFolder_OnButtonClick
    Handles pressing the new asset folder
    @param The command event
==============================*/

void Panel_Search::m_Button_NewFolder_OnButtonClick(wxCommandEvent& event)
{
    this->CreateNewFolder();
    event.Skip();
}


/*==============================
    Panel_Search::m_ToggleButton_Search_OnToggleButton
    Handles pressing the search bar toggle button
    @param The command event
==============================*/

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


/*==============================
    Panel_Search::m_Button_ViewMode_OnButtonClick
    Handles pressing the view mode button
    @param The command event
==============================*/

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


/*==============================
    Panel_Search::m_TextCtrl_Search_OnText
    Handles inputting text into the search bar
    @param The command event
==============================*/

void Panel_Search::m_TextCtrl_Search_OnText(wxCommandEvent& event)
{
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder, event.GetString());
    event.Skip();
}


/*==============================
    Panel_Search::SetMainFolder
    Sets the base folder for the search panel
    @param The base folder where all the assets are stored
==============================*/

void Panel_Search::SetMainFolder(wxFileName path)
{
    this->m_MainFolder = path;
    this->SetCurrentFolder(path);
    this->m_Display_Current->LoadDirectory(path);
}


/*==============================
    Panel_Search::SetAssetType
    Sets the asset's descriptive name and extension
    @param The descriptive name for the asset type
    @param The asset's extension (in "*.name" format)
==============================*/

void Panel_Search::SetAssetType(wxString type, wxString ext)
{
    this->m_AssetType = type;
    this->m_AssetExt = ext;
    this->m_AssetExt_NoAsterisk = ext.SubString(1, ext.Length() - 1);
}


/*==============================
    Panel_Search::SetIconGenerator
    Sets the icon generation function
    @param A function pointer to the icon generator
==============================*/

void Panel_Search::SetIconGenerator(wxIcon (*function)(bool, wxFileName))
{
    this->m_Display_List->SetIconGenerator(function);
    this->m_Display_Grid->SetIconGenerator(function);
}


/*==============================
    Panel_Search::SetAssetGenerator
    Sets the asset generation function
    @param A function pointer to the asset generator
==============================*/

void Panel_Search::SetAssetGenerator(void (*function)(wxFrame*, wxFileName))
{
    this->m_NewAssetFunc = function;
}


/*==============================
    Panel_Search::SetLoadAssetFunc
    Sets the asset loading function. This should handle the
    loading of the asset and initialization of the panel.
    @param A function pointer to the asset loader
==============================*/

void Panel_Search::SetLoadAssetFunc(void (*function)(wxFrame*, wxFileName))
{
    this->m_LoadAssetFunc = function;
}


/*==============================
    Panel_Search::SetRenameAssetFunc
    Sets the asset rename function. This should handle changing
    the titlebar of the program.
    @param A function pointer to the asset renamer
==============================*/

void Panel_Search::SetRenameAssetFunc(void (*function)(wxFrame*, wxFileName, wxFileName))
{
    this->m_RenameAssetFunc = function;
}


/*==============================
    Panel_Search::SetTargetFrame
    Sets the target frame which loading assets will affect
    @param A pointer to the target frame
==============================*/

void Panel_Search::SetTargetFrame(wxFrame* target)
{
    this->m_TargetFrame = target;
}


/*==============================
    Panel_Search::SetCurrentFolder
    Sets the current folder that the search panel is viewing
    @param A path to the folder to view
==============================*/

void Panel_Search::SetCurrentFolder(wxFileName path)
{
    this->m_CurrFolder = path;
    this->m_Button_Back->Enable(this->m_CurrFolder.GetPathWithSep().Cmp(this->m_MainFolder.GetPathWithSep()));
}


/*==============================
    Panel_Search::ClearSearchbox
    Clears the search bar's contents
==============================*/

void Panel_Search::ClearSearchbox()
{
    this->m_TextCtrl_Search->SetValue(wxEmptyString);
}


/*==============================
    Panel_Search::GetMainFolder
    Gets the path to base folder where all the assets are stored
    @return The path to the base folder
==============================*/

wxFileName Panel_Search::GetMainFolder()
{
    return this->m_MainFolder;
}


/*==============================
    Panel_Search::GetCurrentFolder
    Gets the path to the current directory that is being viewed
    @return The path to the currently viewed folder
==============================*/

wxFileName Panel_Search::GetCurrentFolder()
{
    return this->m_CurrFolder;
}


/*==============================
    Panel_Search::GetAssetType
    Gets the descriptive asset name
    @return The descriptive asset name
==============================*/

wxString Panel_Search::GetAssetType()
{
    return this->m_AssetType;
}


/*==============================
    Panel_Search::GetAssetExtension
    Gets the asset's extension
    @param  Whether or not to include the asterisk at the start
            of the string (default false)
    @return The asset's extension
==============================*/

wxString Panel_Search::GetAssetExtension(bool asterisk)
{
    return asterisk ? this->m_AssetExt : this->m_AssetExt_NoAsterisk;
}


/*==============================
    Panel_Search::GetTargetFrame
    Gets the target frame which loading assets will affect
    @return A pointer to the target frame
==============================*/

wxFrame* Panel_Search::GetTargetFrame()
{
    return this->m_TargetFrame;
}


/*==============================
    Panel_Search::CreateNewAsset
    Handles the creation of a new asset
==============================*/

void Panel_Search::CreateNewAsset()
{
    wxString name = wxString("New ") + this->m_AssetType;
    wxString namewithext;
    if (wxFileName(this->m_CurrFolder.GetPathWithSep() + name + this->m_AssetExt_NoAsterisk).Exists())
    {
        wxString testname;
        int i = 2;
        do
        {
            testname = wxString::Format("%s (%d)", name, i);
            i++;
        } while (wxFileName(this->m_CurrFolder.GetPathWithSep() + testname + this->m_AssetExt_NoAsterisk).Exists());
        name = testname;
    }

    // Create the base directory(ies) if it doesn't exist
    if (!wxDirExists(this->m_CurrFolder.GetPath()))
        wxFileName::Mkdir(this->m_CurrFolder.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Create the file and select it for renaming
    this->m_NewAssetFunc(this->m_TargetFrame, this->m_CurrFolder.GetPathWithSep() + name + this->m_AssetExt_NoAsterisk);
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    this->m_Display_Current->SelectItem(name, false, true);
}


/*==============================
    Panel_Search::CreateNewFolder
    Handles the creation of a new folder
==============================*/

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

    // Create the base directory(ies) if it doesn't exist
    if (!wxDirExists(this->m_CurrFolder.GetPath()))
        wxFileName::Mkdir(this->m_CurrFolder.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Create the folder and select it for renaming
    wxDir::Make(this->m_CurrFolder.GetPathWithSep() + name);
    this->m_TextCtrl_Search->ChangeValue(wxEmptyString);
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
    this->m_Display_Current->SelectItem(name, true, true);
}


/*==============================
    Panel_Search::LoadAsset
    Loads an asset and executes the asset loading function
    @param The path to the asset file
==============================*/

void Panel_Search::LoadAsset(wxFileName path)
{
    this->m_LoadAssetFunc(this->m_TargetFrame, path);
}


/*==============================
    Panel_Search::RenameAsset
    Renames an asset and executes the asset renaming function
    @param The old path to the asset file
    @param The new path to the asset file
==============================*/

void Panel_Search::RenameAsset(wxFileName oldpath, wxFileName newpath)
{
    this->m_RenameAssetFunc(this->m_TargetFrame, oldpath, newpath);
    this->m_Display_List->RenameIconInCache(oldpath.GetFullPath(), newpath.GetFullPath());
    this->m_Display_Grid->RenameIconInCache(oldpath.GetFullPath(), newpath.GetFullPath());
}


/*==============================
    Panel_Search::ReloadThumbnail
    Reloads the thumbnail of an asset and updates the cache.
    @param The path to the asset file
==============================*/

void Panel_Search::ReloadThumbnail(wxFileName path)
{
    if (this->m_Display_List->IsIconInCache(path.GetFullPath()))
        this->m_Display_List->GetThreadQueue()->Post(new ThreadWork{path, false});
    if (this->m_Display_Grid->IsIconInCache(path.GetFullPath()))
        this->m_Display_Grid->GetThreadQueue()->Post(new ThreadWork{path, true});
}


/*==============================
    Panel_Search::ReloadDirectory
    Reloads the view of the currently active directory.
==============================*/

void Panel_Search::ReloadDirectory()
{
    this->m_Display_Current->LoadDirectory(this->m_CurrFolder);
}


/*=============================================================
                   Asset Display Base Class
=============================================================*/

/*==============================
    Panel_AssetDisplay (Constructor)
    Initializes the class
    @param The panel's parent window
    @param The panel's ID (default inherited from base class)
    @param The panel's position (default inherited from base class)
    @param The panel's size (default inherited from base class)
    @param The panel's style (default inherited from base class)
==============================*/

Panel_AssetDisplay::Panel_AssetDisplay(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
    this->m_IconGenFunc = NULL;
    this->m_IconGeneratorThread = NULL;
    this->Connect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay::ThreadEvent));
}


/*==============================
    Panel_AssetDisplay (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_AssetDisplay::~Panel_AssetDisplay()
{
    this->StopThread_IconGenerator();
    this->Disconnect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay::ThreadEvent));
}


/*==============================
    Panel_AssetDisplay::ThreadEvent
    Handles results from the icon generator thread
    @param The thread event
==============================*/

void Panel_AssetDisplay::ThreadEvent(wxThreadEvent& event)
{
    // This method should be overwritten by other classes, so it's blank
    event.Skip();
}


/*==============================
    Panel_AssetDisplay::SetIconGenerator
    Set the icon generation function
    @param A function pointer to the icon generation function
==============================*/

void Panel_AssetDisplay::SetIconGenerator(wxIcon(*function)(bool, wxFileName))
{
    this->m_IconGenFunc = function;
}


/*==============================
    Panel_AssetDisplay::LoadDirectory
    Loads the contents in a specific path
    @param The path to load folders and assets of
    @param A filter to apply (default "")
==============================*/

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
    parent->SetCurrentFolder(path);

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


/*==============================
    Panel_AssetDisplay::SelectItem
    Highlights an item (and optionally enables renaming)
    @param The name of the item
    @param Whether the item is a folder
    @param Enable renaming (default false)
==============================*/

void Panel_AssetDisplay::SelectItem(wxString itemname, bool isfolder, bool rename)
{
    // This method should be overwritten by other classes, so it's blank
    // Still need to void out arguments to prevent "Unused arguments" compiler warning
    (void)itemname;
    (void)isfolder;
    (void)rename;
}


/*==============================
    Panel_AssetDisplay::ItemAtMouse
    Gets the item at the mouse's position
    @return The filename of the item at the mouse's position
==============================*/

wxFileName Panel_AssetDisplay::ItemAtMouse()
{
    // This method should be overwritten by other classes, so it's blank
    return wxFileName();
}


/*==============================
    Panel_AssetDisplay::DeleteItem
    Deletes an item with a given path
    @param The path to the item
==============================*/

void Panel_AssetDisplay::DeleteItem(wxFileName itempath)
{
    // This method should be overwritten by other classes, so it's blank
    // Still need to void out arguments to prevent "Unused arguments" compiler warning
    (void)itempath;
}


/*==============================
    Panel_AssetDisplay::IsIconInCache
    Checks whether the icon for a specific file is stored in
    the icon cache
    @param  The path to the item to check the icon of
    @return Whether or not the icon is in the cache
==============================*/

bool Panel_AssetDisplay::IsIconInCache(wxString filepath)
{
    std::unordered_map<wxString, std::list<std::tuple<wxString, wxIcon>>::iterator>::iterator it = this->m_IconCache_Map.find(filepath);
    return it != this->m_IconCache_Map.end();
}


/*==============================
    Panel_AssetDisplay::GetIconFromCache
    Gets the icon from the cache, or generates it in a 
    different thread
    @param  The path to the item to get the icon of
    @param  Whether to fetch a large icon (64x64) or small
            icon (16x16)
    @return The icon, or wxNullIcon if it needs to be generated
==============================*/

wxIcon Panel_AssetDisplay::GetIconFromCache(wxString filepath, bool islarge)
{
    wxIcon icon = wxNullIcon;
    std::unordered_map<wxString, std::list<std::tuple<wxString, wxIcon>>::iterator>::iterator it = this->m_IconCache_Map.find(filepath);
    if (it != this->m_IconCache_Map.end()) // If the icon exists in the cache, get it and mark the icon as recently used
    {
        icon = std::get<1>(*it->second);
        this->m_IconCache_LRU.splice(this->m_IconCache_LRU.begin(), this->m_IconCache_LRU, it->second);
    }
    else // Icon does not exist in the cache, generate it in another thread
        this->GetThreadQueue()->Post(new ThreadWork{wxFileName(filepath), islarge});
    return icon;
}


/*==============================
    Panel_AssetDisplay::InsertOrUpdateCachedIcon
    Inserts the icon to the cache if it doesn't exist yet, or
    updates it if it does
    @param  The path to the item to update the icon of
    @param  The icon to replace with
==============================*/

void Panel_AssetDisplay::InsertOrUpdateCachedIcon(wxString filepath, wxIcon icon)
{
    std::unordered_map<wxString, std::list<std::tuple<wxString, wxIcon>>::iterator>::iterator it = this->m_IconCache_Map.find(filepath);
    if (it == this->m_IconCache_Map.end()) // If the icon was not in the cache, pop it
    {
        if (this->m_IconCache_LRU.size() == ICONCACHESIZE) // Cache is full, pop the least recently used icon
        {
            this->m_IconCache_Map.erase(std::get<0>(this->m_IconCache_LRU.back()));
            this->m_IconCache_LRU.pop_back();
        }
        this->m_IconCache_LRU.push_front({filepath, icon});
        this->m_IconCache_Map[filepath] = this->m_IconCache_LRU.begin();
    }
    else // If the icon was was in the cache, update it and move it to the beginning of the recently accessed list
    {
        std::get<1>(*it->second) = icon;
        this->m_IconCache_LRU.splice(this->m_IconCache_LRU.begin(), this->m_IconCache_LRU, it->second);
    }
}


/*==============================
    Panel_AssetDisplay::RenameIconInCache
    Renames an icon in the cache. Exists so that renamed items
    don't need to regenerate their icon
    @param  The old path of the item
    @param  The new path of the item
==============================*/

void Panel_AssetDisplay::RenameIconInCache(wxString oldpath, wxString newpath)
{
    std::unordered_map<wxString, std::list<std::tuple<wxString, wxIcon>>::iterator>::iterator it = this->m_IconCache_Map.find(oldpath);
    if (it == this->m_IconCache_Map.end())
        return;
    wxIcon icon = std::get<1>(*it->second);
    this->m_IconCache_LRU.erase(it->second);
    this->m_IconCache_Map.erase(oldpath);

    this->m_IconCache_LRU.push_front({newpath, icon});
    this->m_IconCache_Map[newpath] = this->m_IconCache_LRU.begin();
}


/*==============================
    Panel_AssetDisplay::ContextMenu
    Pops open a context menu
==============================*/

void Panel_AssetDisplay::ContextMenu()
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    wxFileName item = this->ItemAtMouse();
    if (item.IsOk())
    {
        bool isfolder = item.IsDir();
        wxString name = isfolder ? item.GetDirs().back() : item.GetName();
        this->SelectItem(name, isfolder);
        wxMenu menu;
        menu.Append(1, "Rename");
        menu.Append(2, "Delete");
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent& event) {
            switch (event.GetId())
            {
                case 1:
                    this->SelectItem(name, isfolder, true);
                    break;
                case 2:
                {
                    wxMessageDialog dialog(this, wxEmptyString, "Delete?", wxCENTER | wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
                    if (isfolder)
                        dialog.SetMessage(wxString::Format("Are you sure you want to delete the directory '%s' and all of its contents?", name));
                    else
                        dialog.SetMessage(wxString::Format("Are you sure you want to delete the asset '%s'?", name));
                    if (dialog.ShowModal() == wxID_YES)
                        this->DeleteItem(item);
                    break;
                }
            }
            event.Skip(false);
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


/*==============================
    Panel_AssetDisplay::GetThreadQueue
    Gets the thread queue. Should only be used by the icon 
    thread
    @return A pointer to the thread message queue
==============================*/

wxMessageQueue<ThreadWork*>* Panel_AssetDisplay::GetThreadQueue()
{
    return &this->m_ThreadQueue;
}


/*=============================================================
                      Asset Display List
=============================================================*/


/*==============================
    Panel_AssetDisplay_List (Constructor)
    Initializes the class
    @param The panel's parent window
    @param The panel's ID (default inherited from base class)
    @param The panel's position (default inherited from base class)
    @param The panel's size (default inherited from base class)
    @param The panel's style (default inherited from base class)
==============================*/

Panel_AssetDisplay_List::Panel_AssetDisplay_List(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : Panel_AssetDisplay(parent, id, pos, size, style)
{
    // Initialize the main sizer
    wxFlexGridSizer* Sizer_Main;
    Sizer_Main = new wxFlexGridSizer(1, 1, 0, 0);
    Sizer_Main->AddGrowableCol(0);
    Sizer_Main->AddGrowableRow(0);
    Sizer_Main->SetFlexibleDirection(wxBOTH);
    Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Initialize the DataViewListCtrl
    this->m_DataViewListCtrl_ObjectList = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_ROW_LINES);
    this->m_DataViewListColumn_Assets = this->m_DataViewListCtrl_ObjectList->AppendIconTextColumn(_("Assets"), wxDATAVIEW_CELL_EDITABLE, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_RESIZABLE);
    this->m_DataViewListColumn_IsFolder = this->m_DataViewListCtrl_ObjectList->AppendToggleColumn(_("Assets"), wxDATAVIEW_CELL_INERT, -1, static_cast<wxAlignment>(wxALIGN_LEFT), wxDATAVIEW_COL_HIDDEN);
    Sizer_Main->Add(this->m_DataViewListCtrl_ObjectList, 0, wxALL | wxEXPAND, 5);

    // Finalize the layout
    this->SetSizer(Sizer_Main);
    this->Layout();

    // Connect events
    this->m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_OnItemActivated), NULL, this);
    this->m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ContextMenu), NULL, this);
    this->m_DataViewListCtrl_ObjectList->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE, wxDataViewEventHandler(Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ItemEditingDone), NULL, this);
    this->Connect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay_List::ThreadEvent));
}


/*==============================
    Panel_AssetDisplay_List (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_AssetDisplay_List::~Panel_AssetDisplay_List()
{
    // Nothing to clean up
}


/*==============================
    Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_OnItemActivated
    Handles clicking on a list item
    @param The DataView event
==============================*/

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
        this->LoadDirectory(newpath);
    }
    else
        parent->LoadAsset(parent->GetCurrentFolder().GetPathWithSep() + icontext.GetText() + parent->GetAssetExtension());

    // Clear the search bar
    parent->ClearSearchbox();
    event.Skip();
}


/*==============================
    Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ContextMenu
    Handles popping open the context menu
    @param The DataView event
==============================*/

void Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ContextMenu(wxDataViewEvent& event)
{
    this->ContextMenu();
    event.Skip(false);
}


/*==============================
    Panel_AssetDisplay_List::m_DataViewListCtrl_ObjectList_ItemEditingDone
    Handles finishing writing an item's name
    @param The DataView event
==============================*/

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
        this->SelectItem(newicontext.GetText(), isfolder);
    }
    event.Skip();
}


/*==============================
    Panel_AssetDisplay_List::LoadDirectory
    Loads the contents in a specific path
    @param The path to load folders and assets of
    @param A filter to apply (default "")
==============================*/

bool Panel_AssetDisplay_List::LoadDirectory(wxFileName path, wxString filter)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    
    // Call the base class's load directory function to populate the folder and asset list
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

    // Add assets to the view
    this->StartThread_IconGenerator();
    for (wxString f : this->m_Assets)
    {
        wxIcon icon = this->GetIconFromCache(path.GetPathWithSep() + f + parent->GetAssetExtension(), false);
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(f, icon));
        items.push_back((wxVariant)false);
        this->m_DataViewListCtrl_ObjectList->AppendItem(items);
    }
    return true;
}


/*==============================
    Panel_AssetDisplay_List::SelectItem
    Highlights an item (and optionally enables renaming)
    @param The name of the item
    @param Whether the item is a folder
    @param Enable renaming (default false)
==============================*/

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


/*==============================
    Panel_AssetDisplay_List::ItemAtMouse
    Gets the item at the mouse's position
    @return The filename of the item at the mouse's position
==============================*/

wxFileName Panel_AssetDisplay_List::ItemAtMouse()
{

    int row;
    bool isfolder;
    wxVariant v;
    wxDataViewItem item;
    wxDataViewIconText it;
    wxPoint mousepos = wxGetMousePosition();
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    wxDataViewColumn col = wxDataViewColumn("", new wxDataViewIconTextRenderer(), 0);
    wxDataViewColumn* colptr = &col;
    wxPoint hit = this->m_DataViewListCtrl_ObjectList->ScreenToClient(mousepos);
    this->m_DataViewListCtrl_ObjectList->HitTest(hit, item, colptr);

    // No file found, stop here
    if (!item.IsOk())
        return wxFileName();              

    // Get item data
    row = this->m_DataViewListCtrl_ObjectList->ItemToRow(item);
    this->m_DataViewListCtrl_ObjectList->GetValue(v, row, 0);
    it << v;
    this->m_DataViewListCtrl_ObjectList->GetValue(v, row, 1);
    isfolder = v.GetBool();

    // Return the path to this file
    if (isfolder)
        return parent->GetCurrentFolder().GetPathWithSep() + it.GetText() + wxFileName::GetPathSeparator();
    else
        return parent->GetCurrentFolder().GetPathWithSep() + it.GetText() + parent->GetAssetExtension();
}


/*==============================
    Panel_AssetDisplay_List::DeleteItem
    Deletes an item with a given path
    @param The path to the item
==============================*/

void Panel_AssetDisplay_List::DeleteItem(wxFileName itempath)
{
    wxString itemname;
    bool isfolder = itempath.IsDir();
    if (isfolder)
        itemname = itempath.GetDirs().back();
    else
        itemname = itempath.GetName();
    for (int i=0; i<this->m_DataViewListCtrl_ObjectList->GetItemCount(); i++)
    {
        bool itemisfolder;
        wxVariant variant;
        wxDataViewIconText icontext;
        wxDataViewItem item = this->m_DataViewListCtrl_ObjectList->RowToItem(i);
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 0);
        icontext << variant;
        this->m_DataViewListCtrl_ObjectList->GetValue(variant, i, 1);
        itemisfolder = variant.GetBool();

        if (itemisfolder == isfolder)
        {
            if (!icontext.GetText().Cmp(itemname))
            {
                int row = this->m_DataViewListCtrl_ObjectList->ItemToRow(item);
                if (isfolder)
                {
                    if (wxDir::Remove(itempath.GetFullPath(), wxPATH_RMDIR_RECURSIVE))
                        this->m_DataViewListCtrl_ObjectList->DeleteItem(row);
                }
                else
                {
                    if (wxRemoveFile(itempath.GetFullPath()))
                        this->m_DataViewListCtrl_ObjectList->DeleteItem(row);
                }
                break;
            }
        }
    }
}


/*==============================
    Panel_AssetDisplay_List::ThreadEvent
    Handles results from the icon generator thread
    @param The thread event
==============================*/

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
            if (!icontext.GetText().Cmp(iconresult->file.GetName()))
            {
                this->InsertOrUpdateCachedIcon(iconresult->file.GetFullPath(), iconresult->icon);
                icontext.SetIcon(iconresult->icon);
                variant << icontext;
                this->m_DataViewListCtrl_ObjectList->SetValue(variant, i, 0);
                break;
            }
        }
    }
    free(iconresult);
    event.Skip();
}


/*=============================================================
                      Asset Display Grid
=============================================================*/

/*==============================
    Panel_AssetDisplay_Grid (Constructor)
    Initializes the class
    @param The panel's parent window
    @param The panel's ID (default inherited from base class)
    @param The panel's position (default inherited from base class)
    @param The panel's size (default inherited from base class)
    @param The panel's style (default inherited from base class)
==============================*/

Panel_AssetDisplay_Grid::Panel_AssetDisplay_Grid(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : Panel_AssetDisplay(parent, id, pos, size, style)
{
    // Initialize member objects
    this->m_ImgList = new wxImageList(64, 64, true);

    // Initialize the main sizer
    wxFlexGridSizer* Sizer_Main;
    Sizer_Main = new wxFlexGridSizer(0, 0, 0, 0);
    Sizer_Main->AddGrowableCol(0);
    Sizer_Main->AddGrowableRow(0);
    Sizer_Main->SetFlexibleDirection(wxBOTH);
    Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Initialize the ListCtrl
    this->m_ListCtrl_ObjectGrid = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE | wxLC_EDIT_LABELS | wxLC_ICON | wxLC_SINGLE_SEL);
    Sizer_Main->Add(this->m_ListCtrl_ObjectGrid, 0, wxALL | wxEXPAND, 5);

    // Finalize layout
    this->SetSizer(Sizer_Main);
    this->Layout();

    // Connect events
    this->m_ListCtrl_ObjectGrid->Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListEndLabelEdit), NULL, this);
    this->m_ListCtrl_ObjectGrid->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListItemActivated), NULL, this);
    this->m_ListCtrl_ObjectGrid->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnRightDown), NULL, this);
    this->Connect(wxID_ANY, wxEVT_THREAD, wxThreadEventHandler(Panel_AssetDisplay_Grid::ThreadEvent));
}


/*==============================
    Panel_AssetDisplay_Grid (Destructor)
    Cleans up the class before deletion
==============================*/

Panel_AssetDisplay_Grid::~Panel_AssetDisplay_Grid()
{
    // Nothing to clean up
}


/*==============================
    Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListItemActivated
    Handles clicking on a grid item
    @param The list event
==============================*/

void Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListItemActivated(wxListEvent& event)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    bool isfolder = (event.GetIndex() < (long)(this->m_Folders.size()));
    wxString item = this->m_ListCtrl_ObjectGrid->GetItemText(event.GetIndex());

    // Handle activation
    if (isfolder)
    {
        wxFileName newpath = parent->GetCurrentFolder().GetPathWithSep() + item + wxFileName::GetPathSeparator();
        this->LoadDirectory(newpath);
    }
    else
        parent->LoadAsset(parent->GetCurrentFolder().GetPathWithSep() + item + parent->GetAssetExtension());

    // Clear the search bar
    parent->ClearSearchbox();
    event.Skip(false);
}


/*==============================
    Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListEndLabelEdit
    Handles finishing writing an item's name
    @param The list event
==============================*/

void Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnListEndLabelEdit(wxListEvent& event)
{
    if (!event.IsEditCancelled())
    {
        Panel_Search* parent = (Panel_Search*)this->GetParent();
        bool isfolder = (event.GetIndex() < (long)(this->m_Folders.size()));
        wxString oldname = this->m_ListCtrl_ObjectGrid->GetItemText(event.GetIndex());
        wxString newname = event.GetText();
        wxString oldpath = parent->GetCurrentFolder().GetPathWithSep() + oldname;
        wxString newpath = parent->GetCurrentFolder().GetPathWithSep() + newname;
        if (!isfolder)
        {
            oldpath += parent->GetAssetExtension();
            newpath += parent->GetAssetExtension();
        }
        if (((isfolder && wxDirExists(newpath)) || (!isfolder && wxFileExists(newpath))) || !wxRenameFile(oldpath, newpath, false))
        {
            event.Veto();
            return;
        }
        parent->RenameAsset(oldpath, newpath);

        // Reload the list and select the renamed item
        // Trust me, trying to "sort it" by using InsertItem is going to lead to headaches, so it's easier to just reload the entire directory
        event.Veto();
        this->LoadDirectory(parent->GetCurrentFolder());
        this->SelectItem(newname, isfolder);
    }
    event.Skip();
}


/*==============================
    Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnRightDown
    Handles popping open the context menu
    @param The mouse event
==============================*/

void Panel_AssetDisplay_Grid::m_ListCtrl_ObjectGrid_OnRightDown(wxMouseEvent& event)
{
    this->ContextMenu();
    event.Skip(false);
}


/*==============================
    Panel_AssetDisplay_Grid::LoadDirectory
    Loads the contents in a specific path
    @param The path to load folders and assets of
    @param A filter to apply (default "")
==============================*/

bool Panel_AssetDisplay_Grid::LoadDirectory(wxFileName path, wxString filter)
{
    int id = 0;
    Panel_Search* parent = (Panel_Search*)this->GetParent();

    // Call the base class's load directory function to populate the folder and asset list
    this->m_ListCtrl_ObjectGrid->DeleteAllItems();
    if (Panel_AssetDisplay::LoadDirectory(path, filter) == false)
        return false;

    // Get asset icons
    this->m_ImgList->RemoveAll();
    this->m_ImgList->Add(Icon_Folder_Large);
    this->StartThread_IconGenerator();
    for (wxString file : this->m_Assets)
    {
        wxIcon icon = this->GetIconFromCache(path.GetPathWithSep() + file + parent->GetAssetExtension(), true);
        if (icon.IsOk())
            this->m_ImgList->Add(icon);
        else
            this->m_ImgList->Add(Icon_BlankLarge);
    }
    this->m_ListCtrl_ObjectGrid->SetImageList(this->m_ImgList, wxIMAGE_LIST_NORMAL);

    // Add folders to the view
    for (wxString folder : this->m_Folders)
    {
        wxIcon icon = Icon_Folder_Large;
        wxListItem item;
        item.SetId(id);
        item.SetColumn(0);
        item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
        item.SetText(folder);
        item.SetImage(0);
        this->m_ListCtrl_ObjectGrid->InsertItem(item);
        id++;
    }

    // Add assets to the view
    for (wxString file : this->m_Assets)
    {
        wxListItem item;
        item.SetId(id);
        item.SetColumn(0);
        item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
        item.SetText(file); // No word wrapping on Linux. I tried word wrapping manually but that causes a ton of visual bugs
        item.SetImage(1 + (id - this->m_Folders.size()));
        this->m_ListCtrl_ObjectGrid->InsertItem(item);
        id++;
    }
    return true;
}


/*==============================
    Panel_AssetDisplay_Grid::SelectItem
    Highlights an item (and optionally enables renaming)
    @param The name of the item
    @param Whether the item is a folder
    @param Enable renaming (default false)
==============================*/

void Panel_AssetDisplay_Grid::SelectItem(wxString itemname, bool isfolder, bool rename)
{
    int start = isfolder ? 0 : this->m_Folders.size();
    int item = this->m_ListCtrl_ObjectGrid->FindItem(start, itemname);
    if (item != wxNOT_FOUND)
    {
        this->m_ListCtrl_ObjectGrid->SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        this->m_ListCtrl_ObjectGrid->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        if (rename)
            this->m_ListCtrl_ObjectGrid->EditLabel(item);
    }
}


/*==============================
    Panel_AssetDisplay_Grid::ItemAtMouse
    Gets the item at the mouse's position
    @return The filename of the item at the mouse's position
==============================*/

wxFileName Panel_AssetDisplay_Grid::ItemAtMouse()
{
    int flags = wxLIST_HITTEST_ONITEM;
    wxPoint mousepos = wxGetMousePosition();
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    wxPoint hit = this->m_ListCtrl_ObjectGrid->ScreenToClient(mousepos);
    long id = this->m_ListCtrl_ObjectGrid->HitTest(hit, flags, NULL);

    // No file found, stop here
    if (id == wxNOT_FOUND)
        return wxFileName("");

    if (id < (long)this->m_Folders.size())
        return parent->GetCurrentFolder().GetPathWithSep() + this->m_ListCtrl_ObjectGrid->GetItemText(id) + wxFileName::GetPathSeparator();
    else
        return parent->GetCurrentFolder().GetPathWithSep() + this->m_ListCtrl_ObjectGrid->GetItemText(id) + parent->GetAssetExtension();
}


/*==============================
    Panel_AssetDisplay_Grid::DeleteItem
    Deletes an item with a given path
    @param The path to the item
==============================*/

void Panel_AssetDisplay_Grid::DeleteItem(wxFileName itempath)
{
    long id;
    int start;
    wxString itemname;
    bool isfolder = itempath.IsDir();
    if (isfolder)
        itemname = itempath.GetDirs().back();
    else
        itemname = itempath.GetName();
    start = isfolder ? 0 : this->m_Folders.size();
    id = this->m_ListCtrl_ObjectGrid->FindItem(start, itemname);
    if (id != wxNOT_FOUND)
    {
        if (isfolder)
        {
            if (wxDir::Remove(itempath.GetFullPath(), wxPATH_RMDIR_RECURSIVE))
                this->m_ListCtrl_ObjectGrid->DeleteItem(id);
        }
        else
        {
            if (wxRemoveFile(itempath.GetFullPath()))
                this->m_ListCtrl_ObjectGrid->DeleteItem(id);
        }
    }
}


/*==============================
    Panel_AssetDisplay_Grid::ThreadEvent
    Handles results from the icon generator thread
    @param The thread event
==============================*/

void Panel_AssetDisplay_Grid::ThreadEvent(wxThreadEvent& event)
{
    Panel_Search* parent = (Panel_Search*)this->GetParent();
    ThreadResult* iconresult = event.GetPayload<ThreadResult*>();
    int item = this->m_ListCtrl_ObjectGrid->FindItem(this->m_Folders.size(), iconresult->file.GetName());
    if (item != -1)
    {
        this->m_ImgList->Replace(1 + (item - this->m_Folders.size()), iconresult->icon);
        this->InsertOrUpdateCachedIcon(iconresult->file.GetFullPath(), iconresult->icon);
    }
    this->m_ListCtrl_ObjectGrid->SetImageList(this->m_ImgList, wxIMAGE_LIST_NORMAL);
    free(iconresult);
    event.Skip();
}


/*=============================================================
                     Icon Generator Thread
=============================================================*/

/*==============================
    IconGeneratorThread (Constructor)
    Initializes the class
    @param The parent panel that this thread is assigned to
    @param The icon generator function to call
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
    // Nothing to clean up
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
            free(t);
        }
    }
    return NULL;
}