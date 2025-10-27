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

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* Sizer_Main;
    Sizer_Main = new wxBoxSizer(wxVERTICAL);

    this->m_Splitter_Vertical = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    this->m_Splitter_Vertical->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_VerticalOnIdle), NULL, this);

    this->m_Panel_Search = new Panel_Search(this->m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    this->m_Panel_Search->SetAssetType(CONTENT_NAME, CONTENT_EXTENSION);
    this->m_Panel_Search->SetTargetFrame(this);
    this->m_Panel_Search->SetAssetGenerator(AssetGenerator);
    this->m_Panel_Search->SetLoadAssetFunc(AssetLoad);
    this->m_Panel_Search->SetRenameAssetFunc(AssetRename);
    this->m_Panel_Search->SetIconGenerator(IconGenerator);
    this->m_Panel_Search->SetMainFolder(((Frame_Main*)this->GetParent())->GetAssetsPath() + CONTENT_FOLDER + wxFileName::GetPathSeparator());

    this->m_Panel_Edit = new wxPanel(this->m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* Sizer_Edit;
    Sizer_Edit = new wxBoxSizer(wxVERTICAL);

    this->m_Splitter_Horizontal = new wxSplitterWindow(this->m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_Horizontal->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_SoundBrowser::m_Splitter_HorizontalOnIdle), NULL, this);

    this->m_Panel_Waveform = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    this->m_Panel_Config = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME|wxTAB_TRAVERSAL);
    this->m_Panel_Config->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    wxFlexGridSizer* Sizer_Config;
    Sizer_Config = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_Config->AddGrowableCol(1);
    Sizer_Config->SetFlexibleDirection(wxBOTH);
    Sizer_Config->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxStaticText* StaticText_Path;
    StaticText_Path = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("Sound path:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_Path->Wrap(-1);
    Sizer_Config->Add(StaticText_Path, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    this->m_FilePicker_Source = new wxFilePickerCtrl(this->m_Panel_Config, wxID_ANY, wxEmptyString, _("Select a file"), _("*.wav"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_OPEN|wxFLP_USE_TEXTCTRL);
    this->m_FilePicker_Source->Enable(false);
    Sizer_Config->Add(this->m_FilePicker_Source, 0, wxALL|wxEXPAND, 5);

    wxStaticText* StaticText_SampleRate;
    StaticText_SampleRate = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("Sample Rate:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_SampleRate->Wrap(-1);
    Sizer_Config->Add(StaticText_SampleRate, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxString SampleRateChoices[] = { _("44100 Hz"), _("32000 Hz"), _("22050 Hz"), _("16000 Hz"), _("11025 Hz") };
    int SampleRateNChoices = sizeof(SampleRateChoices)/sizeof(wxString);
    this->m_Choice_SampleRate = new wxChoice(this->m_Panel_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, SampleRateNChoices, SampleRateChoices, 0);
    this->m_Choice_SampleRate->SetSelection(0);
    this->m_Choice_SampleRate->Enable(false);
    Sizer_Config->Add(this->m_Choice_SampleRate, 0, wxALL, 5);

    this->m_CheckBox_Mono = new wxCheckBox(this->m_Panel_Config, wxID_ANY, _("Force Mono"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_CheckBox_Mono->SetValue(true);
    this->m_CheckBox_Mono->Enable(false);
    Sizer_Config->Add(this->m_CheckBox_Mono, 0, wxALL, 5);

    Sizer_Config->Add(0, 0, 1, wxEXPAND, 5);
    this->m_CheckBox_Loop = new wxCheckBox(this->m_Panel_Config, wxID_ANY, _("Loop"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_CheckBox_Loop->Enable(false);

    Sizer_Config->Add(this->m_CheckBox_Loop, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

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
    this->m_SpinCtrl_LoopStart->Enable(false);

    Sizer_LoopPoints->Add(this->m_SpinCtrl_LoopStart, 0, wxALL | wxEXPAND, 5);

    wxStaticText* StaticText_LoopEnd;
    StaticText_LoopEnd = new wxStaticText(this->m_Panel_Config, wxID_ANY, _("End:"), wxDefaultPosition, wxDefaultSize, 0);
    StaticText_LoopEnd->Wrap(-1);
    Sizer_LoopPoints->Add(StaticText_LoopEnd, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    this->m_SpinCtrl_LoopEnd = new wxSpinCtrl(this->m_Panel_Config, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
    this->m_SpinCtrl_LoopEnd->Enable(false);
    Sizer_LoopPoints->Add(this->m_SpinCtrl_LoopEnd, 0, wxALL | wxEXPAND, 5);

    Sizer_Config->Add(Sizer_LoopPoints, 1, wxEXPAND, 5);
    this->m_Panel_Config->SetSizer(Sizer_Config);
    this->m_Panel_Config->Layout();
    Sizer_Config->Fit(this->m_Panel_Config);
    this->m_Splitter_Horizontal->SplitHorizontally(this->m_Panel_Waveform, this->m_Panel_Config, 0);
    Sizer_Edit->Add(this->m_Splitter_Horizontal, 1, wxEXPAND, 5);

    this->m_Panel_Edit->SetSizer(Sizer_Edit);
    this->m_Panel_Edit->Layout();
    Sizer_Edit->Fit(m_Panel_Edit);
    this->m_Splitter_Vertical->SplitVertically(this->m_Panel_Search, this->m_Panel_Edit, 219);
    Sizer_Main->Add(this->m_Splitter_Vertical, 1, wxEXPAND, 5);

    this->SetSizer(Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);
}


/*==============================
    Frame_SoundBrowser (Destructor)
    Cleans up the class before deletion
==============================*/

Frame_SoundBrowser::~Frame_SoundBrowser()
{

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