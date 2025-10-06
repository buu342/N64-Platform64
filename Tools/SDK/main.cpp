/***************************************************************
                            main.cpp

The main window that allows the user to launch all the other 
tools in the game's SDK.
***************************************************************/

#include "app.h"
#include "main.h"
#include "resource.h"
#include "json.h"
#include "Tools/Image/frame_image.h"
#include <wx/stdpaths.h>


/*********************************
              Types
*********************************/

typedef struct {
    wxString name;
    wxIcon icon;
    void (*function)(wxWindow*, wxString);
} Tools;


/*********************************
       Function Prototypes
*********************************/

void StartTool_Image(wxWindow* parent, wxString title);


/*********************************
             Globals
*********************************/

static std::vector<Tools> g_tools;


/*********************************
         Helper Functions
*********************************/

/*==============================
    Initialize_Tools
    Initializes the list of tools
==============================*/

static void Initialize_Tools()
{
    g_tools.push_back({wxString("Image Browser"), Icon_Texture, StartTool_Image});
    g_tools.push_back({wxString("Material Browser"), Icon_Material, NULL});
}


/*==============================
    StartTool_Image
    Starts the Image Browser tool
    @param The main window to attach this tool to
    @param The name of this tool
==============================*/

void StartTool_Image(wxWindow* parent, wxString title)
{
    Frame_ImageBrowser* w = new Frame_ImageBrowser(parent, -1, title);
    w->Show();
}


/*********************************
    Main Class Implementation
*********************************/

/*==============================
    Frame_Main (Constructor)
    Initializes the class
==============================*/

Frame_Main::Frame_Main() : wxFrame(nullptr, wxID_ANY, _("Platform64 SDK"), wxDefaultPosition, wxSize(300, 400), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
{
    // Initialize the list of tools
    Initialize_Tools();

    // Create the main sizer
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxGridSizer* Sizer_Main;
    Sizer_Main = new wxGridSizer(0, 1, 0, 0);

    // Create the data view list and populate it
    this->m_DataViewListCtrl_Main = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER);
    Sizer_Main->Add(this->m_DataViewListCtrl_Main, 0, wxALL|wxEXPAND, 5);
    this->m_DataViewListCtrl_Main->AppendIconTextColumn(wxString(), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_DEFAULT, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    for (Tools t : g_tools)
    {
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(t.name, t.icon));
        this->m_DataViewListCtrl_Main->AppendItem(items);
        items.pop_back();
    }

    // Finalize the layout
    this->SetSizer(Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect events
    this->m_DataViewListCtrl_Main->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(Frame_Main::m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated), NULL, this);
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
    Called when a selected item is double clicked.
    Used to launch the selected tool.
    @param The DataView event
==============================*/

void Frame_Main::m_DataViewListCtrl_Main_OnDataViewCtrlItemActivated(wxDataViewEvent& event)
{
    int row = this->m_DataViewListCtrl_Main->GetSelectedRow();
    if (row >= 0 && row < (int)g_tools.size() && g_tools[row].function != NULL)
        g_tools[row].function(this, g_tools[row].name);
    event.Skip();
}


/*==============================
    Frame_Main::OpenProject
    Opens the project by attempting to open a .p64 file in the
    same directory that the program is running from.
==============================*/

void Frame_Main::OpenProject()
{
    wxArrayString files;
    wxFileName exefullpath = wxStandardPaths::Get().GetExecutablePath();
    wxString exepath = exefullpath.GetPath();
    size_t filecount = wxDir::GetAllFiles(exepath, &files, wxString("*.p64"), wxDIR_FILES);

    // Check if there is a project in the executable's folder, if so, open it
    if (filecount == 1)
    {
        // If the project failed to load, ask in a loop to open another
        if (json_load(files[0]) == 0)
            Dialog_CreateOpenProject("Unable to open the P64 project file in this folder.\nPlease choose what action to take next:", "Error loading project", exepath);
        else // Project opened successfully, so store the filepath
            this->InitializeProject(files[0]);
    }
    else if (filecount == 0) // If there are no projects in the executable's folder, then ask if one should be opened or if it should be created
        Dialog_CreateOpenProject("No P64 projects were found in this folder.\nPlease choose what action to take:", "No project found", exepath);
    else // If there is multiple projects in the executable's folder, ask to open one of them
        Dialog_CreateOpenProject("There are multiple P64 projects in this folder.\nPlease choose what action to take:", "Multiple projects found", exepath);
}


/*==============================
    Frame_Main::Dialog_CreateOpenProject
    Creates a dialog to create or open a new P64 project
    @param The message to display in the dialog
    @param The title of the dialog
    @param The default path for the file to create/load
==============================*/

void Frame_Main::Dialog_CreateOpenProject(wxString message, wxString title, wxString defaultpath)
{
    while (true)
    {
        int ret;
        wxMessageDialog dialog(this, message, title, wxCENTER | wxNO_DEFAULT | wxYES_NO | wxCANCEL | wxICON_WARNING);
        dialog.SetYesNoCancelLabels("Create a new project", "Load an existing project", "Exit");
        ret = dialog.ShowModal();
        if (ret == wxID_YES)
        {
            wxFileDialog fd(this, "Project file to create", defaultpath, "Project.p64", "*.p64", wxFD_SAVE);
            ret = fd.ShowModal();
            if (ret == wxID_OK)
            {
                if (json_createproject(fd.GetPath()))
                {
                    this->InitializeProject(fd.GetPath());
                    return;
                }
                else
                {
                    wxMessageDialog err(this, "Unable to create P64 project file", "Error", wxCENTER | wxOK | wxICON_ERROR);
                    err.ShowModal();
                }
            }
        }
        else if (ret == wxID_NO)
        {
            wxFileDialog fd(this, "Project file to load", defaultpath, "Project.p64", "*.p64", wxFD_OPEN);
            ret = fd.ShowModal();
            if (ret == wxID_OK)
            {
                if (json_load(fd.GetPath()))
                {
                    this->InitializeProject(fd.GetPath());
                    return;
                }
                else
                {
                    wxMessageDialog err(this, "Unable to load P64 project file", "Error", wxCENTER | wxOK | wxICON_ERROR);
                    err.ShowModal();
                }
            }
        }
        else
        {
            this->Close();
            return;
        }
    }
}


/*==============================
    Frame_Main::InitializeProject
    Initializes a project from a given .p64 file by creating
    the asset directory
    @param The path to the project file
==============================*/

void Frame_Main::InitializeProject(wxString filepath)
{
    wxFileName assetspath;
    this->m_ProjectPath.Assign(filepath);
    assetspath.AssignDir(this->m_ProjectPath.GetPathWithSep() + ASSETS_FOLDER + wxFileName::GetPathSeparator());
    if (!wxDir::Exists(assetspath.GetPathWithSep()))
        wxDir::Make(assetspath.GetPathWithSep());
    this->m_AssetsPath.Assign(assetspath);
    this->SetTitle(this->GetTitle() + " - " + this->m_ProjectPath.GetFullName());
}


/*==============================
    Frame_Main::GetAssetsPath
    Gets the folder that contains the project's assets
    @return The path to the project's assets
==============================*/

wxString Frame_Main::GetAssetsPath()
{
    return this->m_AssetsPath.GetPathWithSep();
}