/***************************************************************
                            main.cpp

TODO
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


/*==============================
    Frame_Main (Constructor)
    Initializes the class
==============================*/

static void Initialize_Tools()
{
    g_tools.push_back({wxString("Image Browser"), Icon_Texture, StartTool_Image});
    g_tools.push_back({wxString("Material Browser"), Icon_Material, NULL});
}

/*==============================
    Frame_Main (Constructor)
    Initializes the class
==============================*/

Frame_Main::Frame_Main() : wxFrame(nullptr, wxID_ANY, _("Platform64 SDK"), wxDefaultPosition, wxSize(300, 400), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
{
    Initialize_Tools();

    // Create the main sizer
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxGridSizer(0, 1, 0, 0);

    // Create the data view list and populate it
    m_DataViewListCtrl_Main = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER);
    m_Sizer_Main->Add(m_DataViewListCtrl_Main, 0, wxALL|wxEXPAND, 5);
    this->m_DataViewListCtrl_Main->AppendIconTextColumn(wxString(), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_DEFAULT, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    for (Tools t : g_tools)
    {
        wxVector<wxVariant> items;
        items.push_back((wxVariant)wxDataViewIconText(t.name, t.icon));
        this->m_DataViewListCtrl_Main->AppendItem(items);
        items.pop_back();
    }

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
    int row = this->m_DataViewListCtrl_Main->GetSelectedRow();
    if (g_tools[row].function != NULL)
        g_tools[row].function(this, g_tools[row].name);

    // Prevent unused parameter warning
    (void)event;
}


/*==============================
    StartTool_Image
    TODO
==============================*/

void StartTool_Image(wxWindow* parent, wxString title)
{
    Frame_ImageBrowser* w = new Frame_ImageBrowser(parent, -1, title);
    w->Show();
}


/*==============================
    OpenProject
    TODO
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
        wxMessageDialog* dialog = new wxMessageDialog(this, files[0]);
        dialog->ShowModal();
        this->m_ProjectPath.Open(files[0]);
        if (json_loadproject(files[0]) == 0)
        {
            this->Close();
            return;
        }
    }
    // If there is multiple projects in the executable's folder, ask to open one of them
    else if (filecount > 1)
    {
        int ret;
        wxMessageDialog* dialog = new wxMessageDialog(this, "There are multiple P64 projects in this folder.\nPlease choose what action to take:", "Multiple projects found", wxCENTER | wxNO_DEFAULT | wxYES_NO | wxICON_WARNING);
        dialog->SetYesNoLabels("Choose which project to load", "Exit");
        ret = dialog->ShowModal();
        if (ret == wxID_OK)
        {

        }
        else
        {
            this->Close();
            return;
        }
    }
    // If there are no projects in the executable's folder, then ask if one should be opened or if it should be created
    else
    {
        int ret;
        wxMessageDialog* dialog = new wxMessageDialog(this, "No P64 projects were found in this folder.\nPlease choose what action to take:", "No project found", wxCENTER | wxNO_DEFAULT | wxYES_NO | wxCANCEL | wxICON_WARNING);
        dialog->SetYesNoCancelLabels("Create a new project", "Load an existing project", "Exit");
        ret = dialog->ShowModal();
        if (ret == wxID_OK)
        {

        }
        else if (ret == wxID_NO)
        {

        }
        else
        {
            this->Close();
            return;
        }
    }
}