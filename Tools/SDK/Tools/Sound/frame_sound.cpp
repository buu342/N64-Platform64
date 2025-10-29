/***************************************************************
                        frame_sound.cpp

This class implements the sound editor panel
***************************************************************/

#include "frame_sound.h"
#include "../../resource.h"
#include "../../main.h"


/*=============================================================
                            Macros
=============================================================*/

#define VERSION_P64SND 1

#define CONTENT_FOLDER     wxString("Sounds")
#define CONTENT_NAME       wxString("Sound")
#define CONTENT_EXTENSION  wxString("*.p64_snd")


/*=============================================================
                      External Functions
=============================================================*/

/*==============================
    IconGenerator
    Thumbnail Generator function to handle thumbnail requests
    from the search panel
    @param  Whether to generate a large thumbnail or not
    @param  The path to the file to generate the thumbnail of
    @return The generated thumbnail
==============================*/

static wxIcon IconGenerator(bool large, wxFileName path)
{
    return wxNullIcon;
}


/*==============================
    AssetGenerator
    Asset generator function to handle when a new file is 
    created by the search panel
    @param The parent frame to attach error dialogs to
    @param The path to the file to create
==============================*/

static void AssetGenerator(wxFrame* frame, wxFileName path)
{
    wxFile file;
    P64Asset_Sound asset;
    std::vector<uint8_t> data = asset.Serialize();
    file.Create(path.GetFullPath());
    file.Open(path.GetFullPath(), wxFile::write);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog(frame, "Unable to open file for writing", "Error serializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }
    file.Write(data.data(), data.size());
    file.Close();
}


/*==============================
    AssetLoad
    Asset loading function to handle when a new file is loaded
    by the search panel
    @param The parent frame to attach error dialogs to
    @param The path to the file to load
==============================*/

static void AssetLoad(wxFrame* frame, wxFileName path)
{
    Frame_SoundBrowser* realframe = (Frame_SoundBrowser*)frame;

    // Load the asset from the path
    realframe->LoadAsset(path);
}


/*==============================
    AssetRename
    Asset renaming function to handle when a file is renamed
    by the search panel
    @param The parent frame that has the asset editor
    @param The old path to the file
    @param The new path to the file
==============================*/

static void AssetRename(wxFrame* frame, wxFileName oldpath, wxFileName newpath)
{
    Frame_SoundBrowser* realframe = (Frame_SoundBrowser*)frame;
    if (realframe->GetLoadedAssetPath() == oldpath)
        realframe->UpdateFilePath(newpath);
}


/*=============================================================
               Sound Editor Frame Implementation
=============================================================*/

/*==============================
    Frame_SoundBrowser (Constructor)
    Initializes the class
    @param The parent window
    @param The frame's ID (default wxID_ANY)
    @param The frame's title (default wxEmptyString)
    @param The frame's position (default wxDefaultPosition)
    @param The frame's size (default 640x480)
    @param The frame's style (default wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
==============================*/

Frame_SoundBrowser::Frame_SoundBrowser(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    // Initialize attributes to their defaults
    this->m_Title = title;
    this->m_LoadedAsset = NULL;
    this->m_AssetModified = false;
    this->m_AssetFilePath = wxFileName("");

    // Initialize the frame
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    // Create the main sizer
    wxBoxSizer* Sizer_Main;
    Sizer_Main = new wxBoxSizer(wxVERTICAL);

    // Split the editor panel vertically
    this->m_Splitter_Vertical = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    this->m_Splitter_Vertical->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_VerticalOnIdle), NULL, this);
    this->m_Splitter_Vertical->SetMinimumPaneSize(1);

    // Setup the search panel
    this->m_Panel_Search = new Panel_Search(this->m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    this->m_Panel_Search->SetAssetType(CONTENT_NAME, CONTENT_EXTENSION);
    this->m_Panel_Search->SetTargetFrame(this);
    this->m_Panel_Search->SetAssetGenerator(AssetGenerator);
    this->m_Panel_Search->SetLoadAssetFunc(AssetLoad);
    this->m_Panel_Search->SetRenameAssetFunc(AssetRename);
    this->m_Panel_Search->SetIconGenerator(IconGenerator);
    this->m_Panel_Search->SetMainFolder(((Frame_Main*)this->GetParent())->GetAssetsPath() + CONTENT_FOLDER + wxFileName::GetPathSeparator());

    // Create the settings editor panel
    this->m_Panel_Edit = new wxPanel(this->m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

    // Create the sizer for the settings editor
    wxBoxSizer* Sizer_Edit;
    Sizer_Edit = new wxBoxSizer(wxVERTICAL);

    // Split the settings editor panel horizontally
    this->m_Splitter_Horizontal = new wxSplitterWindow(this->m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_Horizontal->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    this->m_Splitter_Horizontal->SetMinimumPaneSize(1);

    // Add the preview panel and sizer
    this->m_Panel_Preview = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* Sizer_Preview;
    Sizer_Preview = new wxBoxSizer(wxVERTICAL);

    // Setup the preview toolbar
    this->m_ToolBar_Preview = new wxToolBar(this->m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);
    this->m_ToolBar_Preview->Disable();
    this->m_Tool_Save = this->m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_Save, wxNullBitmap, wxITEM_NORMAL, _("Save changes"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->AddSeparator();
    this->m_Tool_FlashcartUpload = this->m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_USBUpload, wxNullBitmap, wxITEM_NORMAL, _("Upload texture to flashcart"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->Realize();
    Sizer_Preview->Add(this->m_ToolBar_Preview, 0, wxEXPAND, 5);

    // Create the preview window
    this->m_ScrolledWin_Preview = new wxScrolledWindow(m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxHSCROLL | wxVSCROLL);
    this->m_ScrolledWin_Preview->SetScrollRate(5, 5);
    Sizer_Preview->Add(this->m_ScrolledWin_Preview, 1, wxEXPAND, 5);

    // Create the configuration panel
    this->m_Panel_Config = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL);
    this->m_Panel_Config->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    wxFlexGridSizer* Sizer_Config;
    Sizer_Config = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_Config->AddGrowableCol(1);
    Sizer_Config->SetFlexibleDirection(wxBOTH);
    Sizer_Config->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Sound path file picker
    wxStaticText* StaticText_Path;
    StaticText_Path = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("Sound path:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_Path->Wrap(-1);
    Sizer_Config->Add(StaticText_Path, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_FilePicker_Source = new wxFilePickerCtrl(this->m_Panel_Config, wxID_ANY, wxEmptyString, _("Select a file"), _("*.wav"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_OPEN|wxFLP_USE_TEXTCTRL);
    this->m_FilePicker_Source->Disable();
    Sizer_Config->Add(this->m_FilePicker_Source, 0, wxALL|wxEXPAND, 5);

    // Sample rate choice box
    wxStaticText* StaticText_SampleRate;
    StaticText_SampleRate = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("Sample Rate:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_SampleRate->Wrap(-1);
    Sizer_Config->Add(StaticText_SampleRate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxString SampleRateChoices[] = { _("44100 Hz"), _("32000 Hz"), _("22050 Hz"), _("16000 Hz"), _("11025 Hz") };
    int SampleRateNChoices = sizeof(SampleRateChoices)/sizeof(wxString);
    this->m_Choice_SampleRate = new wxChoice(this->m_Panel_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, SampleRateNChoices, SampleRateChoices, 0);
    this->m_Choice_SampleRate->SetSelection(0);
    this->m_Choice_SampleRate->Disable();
    Sizer_Config->Add(this->m_Choice_SampleRate, 0, wxALL, 5);

    // Mono checkbox
    this->m_CheckBox_Mono = new wxCheckBox(this->m_Panel_Config, wxID_ANY, _("Force Mono"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_CheckBox_Mono->SetValue(true);
    this->m_CheckBox_Mono->Disable();
    Sizer_Config->Add(this->m_CheckBox_Mono, 0, wxALL, 5);

    // Loop checkbox
    Sizer_Config->Add(0, 0, 1, wxEXPAND, 5);
    this->m_CheckBox_Loop = new wxCheckBox(this->m_Panel_Config, wxID_ANY, _("Loop"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_CheckBox_Loop->Disable();
    Sizer_Config->Add(this->m_CheckBox_Loop, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    // Loop start+end spin controls
    wxFlexGridSizer* Sizer_LoopPoints;
    Sizer_LoopPoints = new wxFlexGridSizer(0, 4, 0, 0);
    Sizer_LoopPoints->AddGrowableCol(1);
    Sizer_LoopPoints->AddGrowableCol(3);
    Sizer_LoopPoints->SetFlexibleDirection(wxBOTH);
    Sizer_LoopPoints->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    wxStaticText* StaticText_LoopStart;
    StaticText_LoopStart = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("Start:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_LoopStart->Wrap(-1);
    Sizer_LoopPoints->Add(StaticText_LoopStart, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    this->m_SpinCtrl_LoopStart = new wxSpinCtrl(this->m_Panel_Config, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
    this->m_SpinCtrl_LoopStart->Disable();
    Sizer_LoopPoints->Add(this->m_SpinCtrl_LoopStart, 0, wxALL | wxEXPAND, 5);
    wxStaticText* StaticText_LoopEnd;
    StaticText_LoopEnd = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("End:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_LoopEnd->Wrap(-1);
    Sizer_LoopPoints->Add(StaticText_LoopEnd, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    this->m_SpinCtrl_LoopEnd = new wxSpinCtrl(this->m_Panel_Config, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
    this->m_SpinCtrl_LoopEnd->Disable();
    Sizer_LoopPoints->Add(this->m_SpinCtrl_LoopEnd, 0, wxALL | wxEXPAND, 5);
    Sizer_Config->Add(Sizer_LoopPoints, 1, wxEXPAND, 5);

    // Finalize the layout
    this->m_Panel_Config->SetSizer(Sizer_Config);
    this->m_Panel_Config->Layout();
    Sizer_Config->Fit(this->m_Panel_Config);
    this->m_Splitter_Horizontal->SplitHorizontally(this->m_Panel_Preview, this->m_Panel_Config, 0);
    Sizer_Edit->Add(this->m_Splitter_Horizontal, 1, wxEXPAND, 5);
    this->m_Panel_Edit->SetSizer(Sizer_Edit);
    this->m_Panel_Edit->Layout();
    Sizer_Edit->Fit(m_Panel_Edit);
    this->m_Splitter_Vertical->SplitVertically(this->m_Panel_Search, this->m_Panel_Edit, 219);
    Sizer_Main->Add(this->m_Splitter_Vertical, 1, wxEXPAND, 5);
    this->SetSizer(Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Frame_SoundBrowser::OnClose));
    this->m_Panel_Edit->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(Frame_SoundBrowser::m_Panel_Edit_OnChar), NULL, this);
    this->Connect(this->m_Tool_Save->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_SoundBrowser::m_Tool_Save_OnToolClicked));
    this->Connect(this->m_Tool_FlashcartUpload->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_SoundBrowser::m_Tool_FlashcartUpload_OnToolClicked));
    this->m_FilePicker_Source->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(Frame_SoundBrowser::m_FilePicker_Source_OnFileChanged), NULL, this);
    this->m_Choice_SampleRate->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_SoundBrowser::m_Choice_SampleRate_OnChoice), NULL, this);
    this->m_CheckBox_Mono->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Frame_SoundBrowser::m_CheckBox_Mono_OnCheckBox), NULL, this);
    this->m_CheckBox_Loop->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Frame_SoundBrowser::m_CheckBox_Loop_OnCheckBox), NULL, this);
    this->m_SpinCtrl_LoopStart->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(Frame_SoundBrowser::m_SpinCtrl_LoopStart_OnSpinCtrl), NULL, this);
    this->m_SpinCtrl_LoopEnd->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(Frame_SoundBrowser::m_SpinCtrl_LoopEnd_OnSpinCtrl), NULL, this);
}


/*==============================
    Frame_SoundBrowser (Destructor)
    Cleans up the class before deletion
==============================*/

Frame_SoundBrowser::~Frame_SoundBrowser()
{
    if (this->m_LoadedAsset != NULL)
        delete this->m_LoadedAsset;
}


/*==============================
    Frame_SoundBrowser::OnClose
    Handles the user closing the window
    @param The Close event
==============================*/

void Frame_SoundBrowser::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && this->m_AssetModified)
    {
        if (wxMessageBox("Unsaved changes will be lost. Continue?", "Warning", wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Veto();
            return;
        }
    }
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Panel_Edit_OnChar
    Handles the user inputting keys
    @param The Key event
==============================*/

void Frame_SoundBrowser::m_Panel_Edit_OnChar(wxKeyEvent& event)
{
    wxChar uc = event.GetKeyCode();
    if (wxGetKeyState(WXK_CONTROL))
    {
        switch (event.GetKeyCode())
        {
            case 'S':
                this->SaveChanges();
                break;
        }
    }
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Splitter_VerticalOnIdle
    Handles the splitter's idle event.
    Used to set its initial position properly.
    @param The Idle event
==============================*/

void Frame_SoundBrowser::m_Splitter_VerticalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Vertical->SetSashPosition(0);
    this->m_Splitter_Vertical->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_VerticalOnIdle), NULL, this);
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Splitter_HorizontalOnIdle
    Handles the splitter's idle event.
    Used to set its initial position properly.
    @param The Idle event
==============================*/

void Frame_SoundBrowser::m_Splitter_HorizontalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Horizontal->SetSashPosition(0);
    this->m_Splitter_Horizontal->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Tool_Save_OnToolClicked
    Handles pressing the save toolbar button
    @param The command event
==============================*/

void Frame_SoundBrowser::m_Tool_Save_OnToolClicked(wxCommandEvent& event)
{
    this->SaveChanges();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Tool_FlashcartUpload_OnToolClicked
    Handles pressing the flashcart upload toolbar button
    @param The command event
==============================*/

void Frame_SoundBrowser::m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event)
{
    wxMessageDialog dialog(this, "This feature is not yet available.", "Whoops", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_WARNING);
    dialog.ShowModal();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_FilePicker_Source_OnFileChanged
    Handles changing the sound file path
    @param The FileDirPicker event
==============================*/

void Frame_SoundBrowser::m_FilePicker_Source_OnFileChanged(wxFileDirPickerEvent& event)
{
    bool foundsound = false;
    bool externalfile = false;
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }

    // Try to load the sound either from an absolute path or from a relative path
    if (wxFileExists(event.GetPath())) // Check absolute path first
    {
        foundsound = true;
        externalfile = true;
    }
    else if (wxFileExists(this->m_Panel_Search->GetMainFolder().GetFullPath() + wxFileName::GetPathSeparator() + event.GetPath()))
        foundsound = true;

    // Check a valid sound loaded
    if (foundsound)
    {
        // If not relative to our main folder, make a copy of the sound and make it relative
        if (externalfile)
        {
            wxFileName relative;
            wxString fileext = wxFileName(event.GetPath()).GetExt();
            wxString assetpath = this->m_AssetFilePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            wxString assetname = this->m_AssetFilePath.GetName();
            wxString copyfullpath = assetpath + assetname + "." + fileext;
            wxCopyFile(event.GetPath(), copyfullpath, true);
            relative = wxFileName(copyfullpath);
            relative.MakeRelativeTo(this->m_Panel_Search->GetMainFolder().GetFullPath());
            this->m_LoadedAsset->m_SourcePath = relative;
            this->m_FilePicker_Source->SetPath(relative.GetFullPath());
        }
        else
        {
            this->m_FilePicker_Source->SetPath(event.GetPath());
            this->m_LoadedAsset->m_SourcePath = event.GetPath();
        }
    }
    else
    {
        this->m_LoadedAsset->m_SourcePath = event.GetPath();
    }
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_Choice_SampleRate_OnChoice
    Handles changing the sample rate choice box
    @param The command event
==============================*/

void Frame_SoundBrowser::m_Choice_SampleRate_OnChoice(wxCommandEvent& event)
{
    switch (event.GetSelection())
    {
        case 0: this->m_LoadedAsset->m_SampleRate = 44100; break;
        case 1: this->m_LoadedAsset->m_SampleRate = 32000; break;
        case 2: this->m_LoadedAsset->m_SampleRate = 22050; break;
        case 3: this->m_LoadedAsset->m_SampleRate = 16000; break;
        case 4: this->m_LoadedAsset->m_SampleRate = 11025; break;
        default: this->m_LoadedAsset->m_SampleRate = 0; break;
    }
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_CheckBox_Mono_OnCheckBox
    Handles pressing the mono checkbox
    @param The command event
==============================*/

void Frame_SoundBrowser::m_CheckBox_Mono_OnCheckBox(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ForceMono = event.IsChecked();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_CheckBox_Loop_OnCheckBox
    Handles pressing the loop checkbox
    @param The command event
==============================*/

void Frame_SoundBrowser::m_CheckBox_Loop_OnCheckBox(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_Loop = event.IsChecked();
    this->m_SpinCtrl_LoopEnd->Enable(event.IsChecked());
    this->m_SpinCtrl_LoopStart->Enable(event.IsChecked());
    if (event.IsChecked())
    {
        this->m_SpinCtrl_LoopEnd->SetValue(this->m_LoadedAsset->m_LoopStart);
        this->m_SpinCtrl_LoopStart->SetValue(this->m_LoadedAsset->m_LoopEnd);
    }
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_SpinCtrl_LoopStart_OnSpinCtrl
    Handles pressing the loop start spin control
    @param The spin event
==============================*/

void Frame_SoundBrowser::m_SpinCtrl_LoopStart_OnSpinCtrl(wxSpinEvent& event)
{
    if ((uint32_t)event.GetValue() > this->m_LoadedAsset->m_LoopEnd)
    {
        this->m_SpinCtrl_LoopStart->SetValue(this->m_LoadedAsset->m_LoopEnd);
        event.Veto();
        return;
    }
    this->m_LoadedAsset->m_LoopStart = event.GetValue();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::m_SpinCtrl_LoopEnd_OnSpinCtrl
    Handles pressing the loop end spin control
    @param The spin event
==============================*/

void Frame_SoundBrowser::m_SpinCtrl_LoopEnd_OnSpinCtrl(wxSpinEvent& event)
{
    if ((uint32_t)event.GetValue() < this->m_LoadedAsset->m_LoopStart)
    {
        this->m_SpinCtrl_LoopStart->SetValue(this->m_LoadedAsset->m_LoopStart);
        event.Veto();
        return;
    }
    this->m_LoadedAsset->m_LoopEnd = event.GetValue();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_SoundBrowser::MarkAssetModified
    Mark the asset as modified
==============================*/

void Frame_SoundBrowser::MarkAssetModified()
{
    this->m_AssetModified = true;
    this->UpdateTitle();
}


/*==============================
    Frame_SoundBrowser::IsAssetModified
    Check the asset's modification status
    @return Whether the asset was recently modified
==============================*/

bool Frame_SoundBrowser::IsAssetModified()
{
    return this->m_AssetModified;
}


/*==============================
    Frame_SoundBrowser::SaveChanges
    Save changes made to the file
==============================*/

void Frame_SoundBrowser::SaveChanges()
{
    wxFile file;
    bool refresh = false;
    std::vector<uint8_t> data;

    // Create the path to the file if it doesn't exist anymore
    if (!wxFileExists(this->m_AssetFilePath.GetFullPath()))
    {
        if (!wxDirExists(this->m_AssetFilePath.GetPath()))
            wxFileName::Mkdir(this->m_AssetFilePath.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        file.Create(this->m_AssetFilePath.GetFullPath());
        refresh = true;
    }

    // Open the file for writing
    file.Open(this->m_AssetFilePath.GetFullPath(), wxFile::write);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog(this, "Unable to open file for writing", "Error serializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    // Serialize the asset and write it
    data = this->m_LoadedAsset->Serialize();
    file.Write(data.data(), data.size());
    file.Close();
    this->m_AssetModified = false;

    // Update the title and the previews 
    this->UpdateTitle();
    this->m_Panel_Search->ReloadThumbnail(this->m_AssetFilePath);
    if (refresh)
        this->m_Panel_Search->ReloadDirectory();
}


/*==============================
    Frame_SoundBrowser::SaveChanges
    Updates the frame title to reflect the file's name and its
    modification status
==============================*/

void Frame_SoundBrowser::UpdateTitle()
{
    if (this->m_AssetModified)
        this->SetTitle(this->m_AssetFilePath.GetName() + "* - " + this->m_Title);
    else
        this->SetTitle(this->m_AssetFilePath.GetName() + " - " + this->m_Title);
}


/*==============================
    Frame_SoundBrowser::UpdateFilePath
    Updates the current asset's file path.
    Should only be used if the file file was renamed and thus
    it needs to point to a new path.
    @param The new path of the asset
==============================*/

void Frame_SoundBrowser::UpdateFilePath(wxFileName path)
{
    this->m_AssetFilePath = path;
    this->UpdateTitle();
}


/*==============================
    Frame_SoundBrowser::LoadAsset
    Loads a sound asset from a given path
    @param  The path of the asset
    @return The loaded asset, or NULL
==============================*/

P64Asset_Sound* Frame_SoundBrowser::LoadAsset(wxFileName path)
{
    wxFile file;
    std::vector<uint8_t> data;
    P64Asset_Sound* ret;

    // Warn the user that the file was changed before doing anything potentially destructive
    if (this->IsAssetModified())
    {
        wxMessageDialog dialog(this, "Unsaved changes will be lost. Continue?", "Warning", wxCENTER | wxYES | wxNO | wxNO_DEFAULT | wxICON_WARNING);
        if (dialog.ShowModal() == wxID_NO)
            return NULL;
    }

    // Set the path and open the file so we can extract its data
    this->m_AssetFilePath = path;
    file.Open(path.GetFullPath(), wxFile::read);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog(this, "Unable to open file for reading", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return NULL;
    }
    data.resize(path.GetSize().ToULong());
    file.Read(&data[0], data.capacity());
    file.Close();

    // Create the asset object by deserializing the bytes
    ret = P64Asset_Sound::Deserialize(data);
    if (ret == NULL)
        return NULL;
    if (this->m_LoadedAsset != NULL)
        delete this->m_LoadedAsset;
    this->m_LoadedAsset = ret;
    this->m_AssetModified = false;

    // Activate the window
    this->m_ToolBar_Preview->Enable();
    this->m_FilePicker_Source->Enable();
    this->m_Choice_SampleRate->Enable();
    this->m_CheckBox_Mono->Enable();
    this->m_CheckBox_Loop->Enable();
    this->m_SpinCtrl_LoopEnd->Enable(ret->m_Loop);
    this->m_SpinCtrl_LoopStart->Enable(ret->m_Loop);

    // Set the content's values
    this->m_FilePicker_Source->SetPath(ret->m_SourcePath.GetFullPath());
    switch (ret->m_SampleRate)
    {
        case 44100: this->m_Choice_SampleRate->Select(0); break;
        case 32000: this->m_Choice_SampleRate->Select(1); break;
        case 22050: this->m_Choice_SampleRate->Select(2); break;
        case 16000: this->m_Choice_SampleRate->Select(3); break;
        case 11025: this->m_Choice_SampleRate->Select(4); break;
        default:    this->m_Choice_SampleRate->Select(5); break;
    }
    this->m_CheckBox_Mono->SetValue(ret->m_ForceMono);
    this->m_CheckBox_Loop->SetValue(ret->m_Loop);
    this->m_SpinCtrl_LoopStart->SetValue(ret->m_LoopStart);
    this->m_SpinCtrl_LoopEnd->SetValue(ret->m_LoopEnd);
    // TODO: Set spin ctrl's maximum value based on the sample count in the loaded sound

    // Finish
    this->UpdateTitle();
    return ret;
}


/*==============================
    Frame_SoundBrowser::GetLoadedAssetPath
    Get the currently loaded asset's path
    @return The loaded asset's path
==============================*/

wxFileName Frame_SoundBrowser::GetLoadedAssetPath()
{
    return this->m_AssetFilePath;
}