/***************************************************************
                        frame_image.cpp

This class implements the image editor panel
***************************************************************/

#include "frame_image.h"
#include "../../resource.h"
#include "../../main.h"


/*=============================================================
                            Macros
=============================================================*/

#define VERSION_P64TEX 1

#define CONTENT_FOLDER     wxString("Images")
#define CONTENT_NAME       wxString("Image")
#define CONTENT_EXTENSION  wxString("*.p64_img")


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
    wxFile file;
    std::vector<uint8_t> data;
    P64Asset_Image* img;
    wxIcon ret = large ? Icon_MissingLarge : Icon_MissingSmall;

    // Open the file and get its bytes
    data.resize(path.GetSize().ToULong());
    file.Open(path.GetFullPath(), wxFile::read);
    if (!file.IsOpened())
        return ret;
    file.Read(&data[0], data.capacity());
    file.Close();

    // Read the asset file
    img = P64Asset_Image::Deserialize(data);
    if (img == NULL)
        return ret;

    // Grab the thumbnail
    if (large && img->m_Thumbnail.IsValidLarge())
        ret = img->m_Thumbnail.m_IconLarge;
    else if (!large && img->m_Thumbnail.IsValidSmall())
        ret = img->m_Thumbnail.m_IconSmall;

    // Finish
    delete img;
    return ret;
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
    P64Asset_Image asset;
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
    Frame_ImageBrowser* realframe = (Frame_ImageBrowser*)frame;

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
    Frame_ImageBrowser* realframe = (Frame_ImageBrowser*)frame;
    if (realframe->GetLoadedAssetPath() == oldpath)
        realframe->UpdateFilePath(newpath);
}


/*=============================================================
               Image Editor Frame Implementation
=============================================================*/

/*==============================
    Frame_ImageBrowser (Constructor)
    Initializes the class
    @param The parent window
    @param The frame's ID (default wxID_ANY)
    @param The frame's title (default wxEmptyString)
    @param The frame's position (default wxDefaultPosition)
    @param The frame's size (default 640x480)
    @param The frame's style (default wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
==============================*/

Frame_ImageBrowser::Frame_ImageBrowser(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    // Initialize attributes to their defaults
    this->m_Title = title;
    this->m_LoadedAsset = NULL;
    this->m_AssetModified = false;
    this->m_UsingPipette = false;
    this->m_AssetFilePath = wxFileName("");

    // Initialize the frame
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    // Create the main sizer
    wxBoxSizer* Sizer_Main;
    Sizer_Main = new wxBoxSizer(wxVERTICAL);

    // Split the editor panel vertically
    this->m_Splitter_Vertical = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    this->m_Splitter_Vertical->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_VerticalOnIdle), NULL, this);
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
    this->m_Panel_Edit = new wxPanel(this->m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
    
    // Create the sizer for the settings editor
    wxBoxSizer* Sizer_Edit;
    Sizer_Edit = new wxBoxSizer(wxVERTICAL);

    // Split the settings editor panel horizontally
    this->m_Splitter_Horizontal = new wxSplitterWindow(this->m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    this->m_Splitter_Horizontal->SetSashGravity(1);
    this->m_Splitter_Horizontal->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    this->m_Splitter_Horizontal->SetMinimumPaneSize(1);

    // Add the preview panel and sizer
    this->m_Panel_Preview = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    this->m_Panel_Preview->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME));
    wxBoxSizer* Sizer_Preview;
    Sizer_Preview = new wxBoxSizer(wxVERTICAL);

    // Setup the preview toolbar
    this->m_ToolBar_Preview = new wxToolBar(this->m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);
    this->m_ToolBar_Preview->Disable();
    this->m_Tool_Save = this->m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_Save, wxNullBitmap, wxITEM_NORMAL, _("Save changes"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->AddSeparator();
    this->m_Tool_Alpha = this->m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_ToggleAlpha, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of alpha"), wxEmptyString, NULL);
    this->m_Tool_Tiling = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleTiling, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image tiling"), wxEmptyString, NULL);
    this->m_Tool_Filtering = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleFilter, wxNullBitmap, wxITEM_NORMAL, _("Toggle simulation of the N64's 3-point bilinear filtering"), wxEmptyString, NULL);
    //this->m_Tool_PalettePreview = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_Palette, wxNullBitmap, wxITEM_NORMAL, _("Swap the palette for preview"), wxEmptyString, NULL);
    this->m_Tool_Statistics = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleStatistics, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image statistics"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->AddSeparator();
    this->m_Tool_ZoomIn = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomIn, wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), wxEmptyString, NULL);
    this->m_Tool_ZoomOut = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomOut, wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), wxEmptyString, NULL);
    this->m_Tool_ZoomNone = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomFit, wxNullBitmap, wxITEM_NORMAL, _("No zoom"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->AddSeparator();
    this->m_Tool_FlashcartUpload = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_USBUpload, wxNullBitmap, wxITEM_NORMAL, _("Upload texture to flashcart"), wxEmptyString, NULL);
    this->m_ToolBar_Preview->Realize();
    Sizer_Preview->Add(this->m_ToolBar_Preview, 0, wxEXPAND, 5);

    // Create the preview window
    this->m_ScrolledWin_Preview = new Panel_ImgView(this->m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWin_Preview->SetScrollRate(5, 5);
    Sizer_Preview->Add(this->m_ScrolledWin_Preview, 1, wxEXPAND, 5);
    this->m_Panel_Preview->SetSizer(Sizer_Preview);
    this->m_Panel_Preview->Layout();
    Sizer_Preview->Fit(this->m_Panel_Preview);

    // Create the configuration panel
    this->m_Panel_Config = new wxPanel(this->m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* Sizer_Config;
    Sizer_Config = new wxBoxSizer(wxVERTICAL);
    this->m_Notebook_Config = new wxNotebook(this->m_Panel_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    this->m_Notebook_Config->Disable();

    // Create the Image Data notebook page
    this->m_Panel_ImageData = new wxPanel(this->m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* Sizer_ImageData;
    Sizer_ImageData = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_ImageData->AddGrowableCol(1);
    Sizer_ImageData->SetFlexibleDirection(wxBOTH);
    Sizer_ImageData->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Image path file picker
    this->m_StaticText_Image = new wxStaticText(this->m_Panel_ImageData, wxID_ANY, _("Image path:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_Image->Wrap(-1);
    Sizer_ImageData->Add(this->m_StaticText_Image, 0, wxALL, 5);
    this->m_FilePicker_Image = new wxFilePickerCtrl(this->m_Panel_ImageData, wxID_ANY, wxEmptyString, _("Select a file"), _("Image files|*.bmp;*.gif;*.jpg;*.png"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    this->m_FilePicker_Image->SetToolTip(_("The path for the texture image to load"));
    Sizer_ImageData->Add(this->m_FilePicker_Image, 0, wxALL|wxEXPAND, 5);

    // Image resize radios and inputs
    this->m_StaticText_Resize = new wxStaticText(this->m_Panel_ImageData, wxID_ANY, _("Resize:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_Resize->Wrap(-1);
    Sizer_ImageData->Add(this->m_StaticText_Resize, 0, wxALL, 5);
    Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_ResizeNone = new wxRadioButton(this->m_Panel_ImageData, wxID_ANY, _("No change"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_ResizeNone->SetValue(true);
    this->m_RadioBtn_ResizeNone->SetToolTip(_("Do not resize the image"));
    Sizer_ImageData->Add(this->m_RadioBtn_ResizeNone, 0, wxALL, 5);
    Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_ResizeTwo = new wxRadioButton(this->m_Panel_ImageData, wxID_ANY, _("Nearest power of two"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_ResizeTwo->SetToolTip(_("Resize the image so that its size goes to the nearest power of two (16, 32, 64, etc...)"));
    Sizer_ImageData->Add(this->m_RadioBtn_ResizeTwo, 0, wxALL, 5);
    Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_ResizeCustom = new wxRadioButton(this->m_Panel_ImageData, wxID_ANY, _("Custom size:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_ResizeCustom->SetToolTip(_("Set a custom resize size for the image"));
    Sizer_ImageData->Add(this->m_RadioBtn_ResizeCustom, 0, wxALL, 5);
    wxGridSizer* Sizer_ResizeCustom;
    Sizer_ResizeCustom = new wxGridSizer(0, 2, 0, 0);
    this->m_TextCtrl_ResizeW = new wxTextCtrl(this->m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_ResizeW->Disable();
    this->m_TextCtrl_ResizeW->SetToolTip(_("Size of image width (in pixels)"));
    Sizer_ResizeCustom->Add(this->m_TextCtrl_ResizeW, 0, wxALL, 5);
    this->m_TextCtrl_ResizeH = new wxTextCtrl(this->m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_ResizeH->Disable();
    this->m_TextCtrl_ResizeH->SetToolTip(_("Size of image height (in pixels)"));
    Sizer_ResizeCustom->Add(this->m_TextCtrl_ResizeH, 0, wxALL, 5);
    Sizer_ImageData->Add(Sizer_ResizeCustom, 1, wxEXPAND, 5);

    // Resize alignment selection
    this->m_StaticText_Align = new wxStaticText(this->m_Panel_ImageData, wxID_ANY, _("Resize alignment:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_Align->Wrap(-1);
    Sizer_ImageData->Add(this->m_StaticText_Align, 0, wxALL, 5);
    wxString Choice_AlignChoices[] = { _("Top left"), _("Top middle"), _("Top right"), _("Middle left"), _("Center"), _("Middle right"), _("Bottom left"), _("Bottom middle"), _("Bottom right") };
    int Choice_AlignNChoices = sizeof(Choice_AlignChoices)/sizeof(wxString);
    this->m_Choice_Align = new wxChoice(this->m_Panel_ImageData, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_AlignNChoices, Choice_AlignChoices, 0);
    this->m_Choice_Align->SetSelection(4);
    this->m_Choice_Align->Disable();
    this->m_Choice_Align->SetToolTip(_("Set where to align the image to when resizing"));
    Sizer_ImageData->Add(this->m_Choice_Align, 0, wxALL|wxEXPAND, 5);

    // Resize fill selection
    this->m_StaticText_ResizeFill = new wxStaticText(this->m_Panel_ImageData, wxID_ANY, _("Fill space with:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_ResizeFill->Wrap(-1);
    Sizer_ImageData->Add(this->m_StaticText_ResizeFill, 0, wxALL, 5);
    wxString Choice_ResizeFillChoices[] = {_("Invisible color"), _("Edge colors"), _("Repeated texture"), _("Mirrored texture")};
    int Choice_ResizeFillNChoices = sizeof(Choice_ResizeFillChoices)/sizeof(wxString);
    this->m_Choice_ResizeFill = new wxChoice(this->m_Panel_ImageData, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_ResizeFillNChoices, Choice_ResizeFillChoices, 0);
    this->m_Choice_ResizeFill->SetSelection(0);
    this->m_Choice_ResizeFill->Disable();
    this->m_Choice_ResizeFill->SetToolTip(_("When resizing larger than the base size, what color should be used to fill the new space?"));
    Sizer_ImageData->Add(this->m_Choice_ResizeFill, 0, wxALL|wxEXPAND, 5);

    // Finalize the image notebook page layout
    this->m_Panel_ImageData->SetSizer(Sizer_ImageData);
    this->m_Panel_ImageData->Layout();
    Sizer_ImageData->Fit(this->m_Panel_ImageData);
    this->m_Notebook_Config->AddPage(this->m_Panel_ImageData, _("Image"), true);

    // Create the image loading notebook page
    this->m_Panel_ImageLoading = new wxPanel(this->m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* Sizer_ImageLoading;
    Sizer_ImageLoading = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_ImageLoading->AddGrowableCol(1);
    Sizer_ImageLoading->SetFlexibleDirection(wxBOTH);
    Sizer_ImageLoading->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Create the format selection
    this->m_StaticText_Format = new wxStaticText(this->m_Panel_ImageLoading, wxID_ANY, _("Format:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_Format->Wrap(-1);
    Sizer_ImageLoading->Add(this->m_StaticText_Format, 0, wxALL, 5);
    wxString Choice_FormatChoices[] = { _("32-Bit RGBA"), _("16-Bit RGBA"), _("16-Bit IA"), _("8-Bit IA"), _("4-Bit IA"), _("8-Bit I"), _("4-Bit I")/*, _("8-Bit CI"), _("4-Bit CI")*/ };
    int Choice_FormatNChoices = sizeof(Choice_FormatChoices)/sizeof(wxString);
    this->m_Choice_Format = new wxChoice(this->m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_FormatNChoices, Choice_FormatChoices, 0);
    this->m_Choice_Format->SetSelection(1);
    this->m_Choice_Format->SetToolTip(_("Image loading format. Different formats allow for larger image sizes at the expense of color."));
    Sizer_ImageLoading->Add(this->m_Choice_Format, 0, wxALL|wxEXPAND, 5);

    // Create the tiling mode selections
    this->m_StaticText_TilingMode = new wxStaticText(this->m_Panel_ImageLoading, wxID_ANY, _("Tiling mode:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_TilingMode->Wrap(-1);
    Sizer_ImageLoading->Add(this->m_StaticText_TilingMode, 0, wxALL, 5);
    wxGridSizer* Sizer_TilingMode;
    Sizer_TilingMode = new wxGridSizer(0, 2, 0, 0);
    wxString Choice_TilingXChoices[] = { _("Mirror"), _("Wrap"), _("Clamp") };
    int Choice_TilingXNChoices = sizeof(Choice_TilingXChoices)/sizeof(wxString);
    this->m_Choice_TilingX = new wxChoice(this->m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_TilingXNChoices, Choice_TilingXChoices, 0);
    this->m_Choice_TilingX->SetSelection(0);
    this->m_Choice_TilingX->SetToolTip(_("Horizontal tiling mode"));
    Sizer_TilingMode->Add(this->m_Choice_TilingX, 0, wxALL|wxEXPAND, 5);
    wxString Choice_TilingYChoices[] = { _("Mirror"), _("Wrap"), _("Clamp") };
    int Choice_TilingYNChoices = sizeof(Choice_TilingYChoices)/sizeof(wxString);
    this->m_Choice_TilingY = new wxChoice(this->m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_TilingYNChoices, Choice_TilingYChoices, 0);
    this->m_Choice_TilingY->SetSelection(0);
    this->m_Choice_TilingY->SetToolTip(_("Vertical tiling mode"));
    Sizer_TilingMode->Add(this->m_Choice_TilingY, 0, wxALL|wxEXPAND, 5);
    Sizer_ImageLoading->Add(Sizer_TilingMode, 1, wxEXPAND, 5);

    // Create the mask position inputs
    this->m_StaticText_MaskPos = new wxStaticText(this->m_Panel_ImageLoading, wxID_ANY, _("Mask start:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_MaskPos->Wrap(-1);
    Sizer_ImageLoading->Add(this->m_StaticText_MaskPos, 0, wxALL, 5);
    wxGridSizer* Sizer_MaskPos;
    Sizer_MaskPos = new wxGridSizer(0, 2, 0, 0);
    this->m_TextCtrl_MaskPosW = new wxTextCtrl(this->m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_MaskPosW->SetToolTip(_("X coordinate start of the image mask"));
    Sizer_MaskPos->Add(this->m_TextCtrl_MaskPosW, 0, wxALL, 5);
    this->m_TextCtrl_MaskPosH = new wxTextCtrl(this->m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_MaskPosH->SetToolTip(_("Y coordinate start of the image mask"));
    Sizer_MaskPos->Add(this->m_TextCtrl_MaskPosH, 0, wxALL, 5);
    Sizer_ImageLoading->Add(Sizer_MaskPos, 1, wxEXPAND, 5);

    // Create the generate mipmaps checkbox
    this->m_Checkbox_Mipmaps = new wxCheckBox(this->m_Panel_ImageLoading, wxID_ANY, _("Generate mipmaps"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Checkbox_Mipmaps->SetToolTip(_("Generate mipmaps for this texture (requires the image to only take up half of TMEM!)"));
    Sizer_ImageLoading->Add(this->m_Checkbox_Mipmaps, 0, wxALL, 5);

    // Finalize the image loading notebook page layout
    this->m_Panel_ImageLoading->SetSizer(Sizer_ImageLoading);
    this->m_Panel_ImageLoading->Layout();
    Sizer_ImageLoading->Fit(this->m_Panel_ImageLoading);
    this->m_Notebook_Config->AddPage(this->m_Panel_ImageLoading, _("Loading"), false);
    this->m_Panel_ImageColors = new wxPanel(this->m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
   
    // Create the image colors notebook page
    wxFlexGridSizer* Sizer_ImageColors;
    Sizer_ImageColors = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_ImageColors->AddGrowableCol(1);
    Sizer_ImageColors->SetFlexibleDirection(wxBOTH);
    Sizer_ImageColors->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Create the dithering algorithm selection
    this->m_StaticText_Dithering = new wxStaticText(this->m_Panel_ImageColors, wxID_ANY, _("Dithering:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_Dithering->Wrap(-1);
    Sizer_ImageColors->Add(this->m_StaticText_Dithering, 0, wxALL, 5);
    wxString Choice_DitheringChoices[] = { _("None"), _("Ordered Dithering"), _("Floyd-Steinberg") };
    int Choice_DitheringNChoices = sizeof(Choice_DitheringChoices)/sizeof(wxString);
    this->m_Choice_Dithering = new wxChoice(this->m_Panel_ImageColors, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choice_DitheringNChoices, Choice_DitheringChoices, 0);
    this->m_Choice_Dithering->SetSelection(1);
    this->m_Choice_Dithering->SetToolTip(_("What algorithm to use to reduce the colors"));
    Sizer_ImageColors->Add(this->m_Choice_Dithering, 0, wxALL|wxEXPAND, 5);

    // Create the alpha mode radios and inputs
    this->m_StaticText_AlphaChoice = new wxStaticText(this->m_Panel_ImageColors, wxID_ANY, _("Alpha:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_AlphaChoice->Wrap(-1);
    Sizer_ImageColors->Add(this->m_StaticText_AlphaChoice, 0, wxALL, 5);
    Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_AlphaNone = new wxRadioButton(this->m_Panel_ImageColors, wxID_ANY, _("No Alpha"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_AlphaNone->SetToolTip(_("Do not use any alpha"));
    Sizer_ImageColors->Add(this->m_RadioBtn_AlphaNone, 0, wxALL, 5);
    Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_AlphaMask = new wxRadioButton(this->m_Panel_ImageColors, wxID_ANY, _("Alpha mask"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_AlphaMask->SetValue(true);
    this->m_RadioBtn_AlphaMask->SetToolTip(_("Use the image's built-in alpha mask (such as PNG transparency)"));
    Sizer_ImageColors->Add(this->m_RadioBtn_AlphaMask, 0, wxALL, 5);
    Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);
    this->m_RadioBtn_AlphaColor = new wxRadioButton(this->m_Panel_ImageColors, wxID_ANY, _("Remove color:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_AlphaColor->SetToolTip(_("Treat a specific color in the unmodified image as the alpha color"));
    Sizer_ImageColors->Add(this->m_RadioBtn_AlphaColor, 0, wxALL, 5);
    wxFlexGridSizer* Sizer_AlphaColor;
    Sizer_AlphaColor = new wxFlexGridSizer(0, 2, 0, 0);
    Sizer_AlphaColor->AddGrowableCol(0);
    Sizer_AlphaColor->SetFlexibleDirection(wxBOTH);
    Sizer_AlphaColor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_ColourPicker_AlphaColor = new wxColourPickerCtrl(this->m_Panel_ImageColors, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
    this->m_ColourPicker_AlphaColor->Disable();
    this->m_ColourPicker_AlphaColor->SetToolTip(_("Color picker for the alpha color"));
    Sizer_AlphaColor->Add(this->m_ColourPicker_AlphaColor, 0, wxALL|wxEXPAND, 5);
    this->m_BitmapButton_Pipette = new wxBitmapButton(this->m_Panel_ImageColors, wxID_ANY, Icon_Pipette, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0);
    this->m_BitmapButton_Pipette->Disable();
    this->m_BitmapButton_Pipette->SetToolTip(_("Pick the color by clicking on the image"));
    Sizer_AlphaColor->Add(this->m_BitmapButton_Pipette, 0, wxALL, 5);
    Sizer_ImageColors->Add(Sizer_AlphaColor, 1, wxEXPAND, 5);
    this->m_RadioBtn_AlphaExternal = new wxRadioButton(this->m_Panel_ImageColors, wxID_ANY, _("External mask:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_RadioBtn_AlphaExternal->SetToolTip(_("Use an external image for the alpha mask (requires the mask be the same size as the source image)"));
    Sizer_ImageColors->Add(this->m_RadioBtn_AlphaExternal, 0, wxALL, 5);
    this->m_FilePicker_Alpha = new wxFilePickerCtrl(this->m_Panel_ImageColors, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    this->m_FilePicker_Alpha->Disable();
    this->m_FilePicker_Alpha->SetToolTip(_("Filepath for the external alpha mask"));
    Sizer_ImageColors->Add(this->m_FilePicker_Alpha, 0, wxALL|wxEXPAND, 5);

    // Create the palette configuration button
    this->m_Button_Palette = new wxButton(this->m_Panel_ImageColors, wxID_ANY, _("Configure Palette"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_Palette->Disable();
    this->m_Button_Palette->SetToolTip(_("Configure Color Index palette"));
    Sizer_ImageColors->Add(this->m_Button_Palette, 0, wxALL, 5);

    // Finalize the image colors notebook page layout
    this->m_Panel_ImageColors->SetSizer(Sizer_ImageColors);
    this->m_Panel_ImageColors->Layout();
    Sizer_ImageColors->Fit(this->m_Panel_ImageColors);
    this->m_Notebook_Config->AddPage(this->m_Panel_ImageColors, _("Colors"), false);
    Sizer_Config->Add(this->m_Notebook_Config, 1, wxEXPAND | wxALL, 5);

    // Finalize the layout
    this->m_Panel_Config->SetSizer(Sizer_Config);
    this->m_Panel_Config->Layout();
    Sizer_Config->Fit(this->m_Panel_Config);
    this->m_Splitter_Horizontal->SplitHorizontally(this->m_Panel_Preview, this->m_Panel_Config, 0);
    Sizer_Edit->Add(this->m_Splitter_Horizontal, 1, wxEXPAND, 5);
    this->m_Panel_Edit->SetSizer(Sizer_Edit);
    this->m_Panel_Edit->Layout();
    Sizer_Edit->Fit(this->m_Panel_Edit);
    this->m_Splitter_Vertical->SplitVertically(this->m_Panel_Search, this->m_Panel_Edit, 0);
    Sizer_Main->Add(this->m_Splitter_Vertical, 1, wxEXPAND, 5);
    this->SetSizer(Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect events
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Frame_ImageBrowser::OnClose));
    this->m_Splitter_Vertical->Connect(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, wxSplitterEventHandler(Frame_ImageBrowser::m_Splitter_Vertical_DClick), NULL, this);
    this->m_Splitter_Horizontal->Connect(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, wxSplitterEventHandler(Frame_ImageBrowser::m_Splitter_Horizontal_DClick), NULL, this);
    this->m_Panel_Edit->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(Frame_ImageBrowser::m_Panel_Edit_OnChar), NULL, this);
    this->Connect(this->m_Tool_Save->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Save_OnToolClicked));
    this->Connect(this->m_Tool_Alpha->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked));
    this->Connect(this->m_Tool_Tiling->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked));
    this->Connect(this->m_Tool_Filtering->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked));
    //this->Connect(this->m_Tool_PalettePreview->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked));
    this->Connect(this->m_Tool_Statistics->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked));
    this->Connect(this->m_Tool_ZoomIn->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked));
    this->Connect(this->m_Tool_ZoomOut->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked));
    this->Connect(this->m_Tool_ZoomNone->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked));
    this->Connect(this->m_Tool_FlashcartUpload->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked));
    this->m_ScrolledWin_Preview->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseLeftDown), NULL, this);
    this->m_ScrolledWin_Preview->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseWheel), NULL, this);
    this->m_FilePicker_Image->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(Frame_ImageBrowser::m_FilePicker_Image_OnFileChanged), NULL, this);
    this->m_RadioBtn_ResizeNone->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton), NULL, this);
    this->m_RadioBtn_ResizeTwo->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton), NULL, this);
    this->m_RadioBtn_ResizeCustom->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton), NULL, this);
    this->m_TextCtrl_ResizeW->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText), NULL, this);
    this->m_TextCtrl_ResizeH->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText), NULL, this);
    this->m_Choice_Align->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Align_OnChoice), NULL, this);
    this->m_Choice_ResizeFill->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice), NULL, this);
    this->m_Choice_Format->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Format_OnChoice), NULL, this);
    this->m_Choice_TilingX->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_TilingX_OnChoice), NULL, this);
    this->m_Choice_TilingY->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_TilingY_OnChoice), NULL, this);
    this->m_TextCtrl_MaskPosW->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText), NULL, this);
    this->m_TextCtrl_MaskPosH->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_MaskPosH_OnText), NULL, this);
    this->m_Checkbox_Mipmaps->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox), NULL, this);
    this->m_Choice_Dithering->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Dithering_OnChoice), NULL, this);
    this->m_RadioBtn_AlphaNone->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton), NULL, this);
    this->m_RadioBtn_AlphaMask->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton), NULL, this);
    this->m_RadioBtn_AlphaColor->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton), NULL, this);
    this->m_ColourPicker_AlphaColor->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged), NULL, this);
    this->m_BitmapButton_Pipette->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick), NULL, this);
    this->m_RadioBtn_AlphaExternal->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton), NULL, this);
    this->m_FilePicker_Alpha->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged), NULL, this);
    this->m_Button_Palette->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Button_Palette_OnButtonClick), NULL, this);

    // Hide unimplemented things
    this->m_Button_Palette->Hide();
    this->m_StaticText_MaskPos->Hide();
    this->m_TextCtrl_MaskPosW->Hide();
    this->m_TextCtrl_MaskPosH->Hide();
    //this->m_Tool_PalettePreview->Hide();
}


/*==============================
    Frame_ImageBrowser (Destructor)
    Cleans up the class before deletion
==============================*/

Frame_ImageBrowser::~Frame_ImageBrowser()
{
    if (this->m_LoadedAsset != NULL)
        delete this->m_LoadedAsset;
}


/*==============================
    Frame_ImageBrowser::OnClose
    Handles the user closing the window
    @param The Close event
==============================*/

void Frame_ImageBrowser::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && this->m_AssetModified)
    {
        if (wxMessageBox("Unsaved changes will be lost. Continue?", this->m_Title, wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Veto();
            return;
        }
    }
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Panel_Edit_OnChar
    Handles the user inputting keys
    @param The Key event
==============================*/

void Frame_ImageBrowser::m_Panel_Edit_OnChar(wxKeyEvent& event)
{
    wxChar uc = event.GetKeyCode();
    if (wxGetKeyState(WXK_CONTROL))
    {
        switch (event.GetKeyCode())
        {
            case 'S':
                this->SaveChanges();
                break;
            case '+':
                this->m_ScrolledWin_Preview->ZoomIn();
                break;
            case '-':
                this->m_ScrolledWin_Preview->ZoomOut();
                break;
        }
    }
    else
    {
        switch (event.GetKeyCode())
        {
            case WXK_ESCAPE:
                if (this->m_UsingPipette)
                {
                    wxWindow::SetCursor(wxNullCursor);
                    wxSetCursor(wxNullCursor);
                    this->m_UsingPipette = false;
                }
                break;
        }
    }
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Splitter_VerticalOnIdle
    Handles the splitter's idle event.
    Used to set its initial position properly.
    @param The Idle event
==============================*/

void Frame_ImageBrowser::m_Splitter_VerticalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Vertical->SetSashPosition(0);
    this->m_Splitter_Vertical->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_VerticalOnIdle), NULL, this);
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Splitter_HorizontalOnIdle
    Handles the splitter's idle event.
    Used to set its initial position properly.
    @param The Idle event
==============================*/

void Frame_ImageBrowser::m_Splitter_HorizontalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Horizontal->SetSashPosition(this->m_Panel_Edit->GetSize().y - this->m_Panel_Config->GetBestSize().y);
    this->m_Splitter_Horizontal->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Splitter_Vertical_DClick
    Handles double clicking the splitter
    @param The splitter event
==============================*/

void Frame_ImageBrowser::m_Splitter_Vertical_DClick(wxSplitterEvent& event)
{
    event.Veto();
}


/*==============================
    Frame_ImageBrowser::m_Splitter_Horizontal_DClick
    Handles double clicking the splitter
    @param The splitter event
==============================*/

void Frame_ImageBrowser::m_Splitter_Horizontal_DClick(wxSplitterEvent& event)
{
    event.Veto();
}


/*==============================
    Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseWheel
    Handles using the mousewheel in the preview window
    @param The mouse event
==============================*/

void Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseWheel(wxMouseEvent& event)
{
    if (event.ControlDown())
    {
        if (event.GetWheelRotation() > 0)
            this->m_ScrolledWin_Preview->ZoomIn();
        else
            this->m_ScrolledWin_Preview->ZoomOut();
    }
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseLeftDown
    Handles using the left mouse button in the preview window
    @param The mouse event
==============================*/

void Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseLeftDown(wxMouseEvent& event)
{
    if (this->m_UsingPipette)
    {
        if (this->m_LoadedAsset != NULL)
        {
            wxColor c;
            wxWindowDC dc(this->m_ScrolledWin_Preview);
            wxPoint p = event.GetPosition();
            dc.GetPixel(p.x, p.y, &c);
            this->m_ColourPicker_AlphaColor->SetColour(c);
            this->m_LoadedAsset->m_AlphaColor = c;
            this->MarkAssetModified();
        }

        wxWindow::SetCursor(wxNullCursor);
        wxSetCursor(wxNullCursor);
        this->m_UsingPipette = false;
    }
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_Save_OnToolClicked
    Handles pressing the save toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_Save_OnToolClicked(wxCommandEvent& event)
{
    this->SaveChanges();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked
    Handles pressing the alpha display toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleAlphaDisplay();
    if (this->m_LoadedAsset != NULL)
        this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked
    Handles pressing the tiling display toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked(wxCommandEvent& event)
{
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked
    Handles pressing the filtering preview toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleFilterDisplay();
    if (this->m_LoadedAsset != NULL)
        this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked
    Handles pressing the palette preview toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked(wxCommandEvent& event)
{
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked
    Handles pressing the statistics display toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleStatisticsDisplay();
    this->m_ScrolledWin_Preview->RefreshDrawing();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked
    Handles pressing the zoom in toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomIn();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked
    Handles pressing the zoom out toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomOut();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked
    Handles pressing the zoom reset toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomReset();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked
    Handles pressing the flashcart upload toolbar button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event)
{
    wxMessageDialog dialog(this, "This feature is not yet available.", "Whoops", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_WARNING);
    dialog.ShowModal();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_FilePicker_Image_OnFileChanged
    Handles changing the image file path
    @param The FileDirPicker event
==============================*/

void Frame_ImageBrowser::m_FilePicker_Image_OnFileChanged(wxFileDirPickerEvent& event)
{
    bool foundbitmap = false;
    bool externalfile = false;
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }

    // Try to load the image either from an absolute path or from a relative path
    if (wxFileExists(event.GetPath())) // Check absolute path first
    {
        foundbitmap = this->m_LoadedAsset->m_Image.LoadFile(event.GetPath());
        externalfile = true;
    }
    else if (wxFileExists(this->m_Panel_Search->GetMainFolder().GetFullPath() + wxFileName::GetPathSeparator() + event.GetPath()))
        foundbitmap = this->m_LoadedAsset->m_Image.LoadFile(this->m_Panel_Search->GetMainFolder().GetFullPath() + wxFileName::GetPathSeparator() + event.GetPath());

    // Check a valid image loaded
    if (foundbitmap)
    {
        // If not relative to our main folder, make a copy of the image and make it relative
        if (externalfile)
        {
            wxFileName relative;
            wxString fileext = wxFileName(event.GetPath()).GetExt();
            wxString assetpath = this->m_AssetFilePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
            wxString assetname = this->m_AssetFilePath.GetName();
            wxString copyfullpath =  assetpath + assetname + "." + fileext;
            wxCopyFile(event.GetPath(), copyfullpath, true);
            relative = wxFileName(copyfullpath);
            relative.MakeRelativeTo(this->m_Panel_Search->GetMainFolder().GetFullPath());
            this->m_LoadedAsset->m_SourcePath = relative;
            this->m_FilePicker_Image->SetPath(relative.GetFullPath());
        }
        else
        {
            this->m_FilePicker_Image->SetPath(event.GetPath());
            this->m_LoadedAsset->m_SourcePath = event.GetPath();
        }
        this->m_TextCtrl_ResizeW->SetValue(wxString::Format("%d", this->m_LoadedAsset->m_Image.GetWidth()));
        this->m_TextCtrl_ResizeH->SetValue(wxString::Format("%d", this->m_LoadedAsset->m_Image.GetHeight()));
    }
    else
    {
        this->m_LoadedAsset->m_SourcePath = event.GetPath();
        this->m_LoadedAsset->m_Image = wxImage();
    }
    this->m_ScrolledWin_Preview->ZoomReset();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton
    Handles changing the resize radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_NONE;
    this->m_TextCtrl_ResizeW->Disable();
    this->m_TextCtrl_ResizeH->Disable();
    this->m_Choice_Align->Disable();
    this->m_Choice_ResizeFill->Disable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton
    Handles changing the resize radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_POWER2;
    this->m_TextCtrl_ResizeW->Disable();
    this->m_TextCtrl_ResizeH->Disable();
    this->m_Choice_Align->Enable();
    this->m_Choice_ResizeFill->Enable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton
    Handles changing the resize radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_CUSTOM;
    this->m_TextCtrl_ResizeW->Enable();
    this->m_TextCtrl_ResizeH->Enable();
    this->m_Choice_Align->Enable();
    this->m_Choice_ResizeFill->Enable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText
    Handles input into the image width text control
    @param The command event
==============================*/

void Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_CustomSize.x);
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText
    Handles input into the image height text control
    @param The command event
==============================*/

void Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_CustomSize.y);
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_Align_OnChoice
    Handles changing the alignment choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_Align_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_Alignment = (P64Img_Alignment)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice
    Handles changing the resize fill choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_ResizeFill = (P64Img_Fill)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_Format_OnChoice
    Handles changing the image format choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_Format_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_ImageFormat = (P64Img_Format)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_TilingX_OnChoice
    Handles changing the horizontal tiling choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_TilingX_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_TilingX = (P64Img_Tiling)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_TilingY_OnChoice
    Handles changing the vertical tiling choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_TilingY_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_TilingY = (P64Img_Tiling)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText
    Handles changing the mask X position text input
    @param The command event
==============================*/

void Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_MaskStart.x);
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText
    Handles changing the mask Y position text input
    @param The command event
==============================*/

void Frame_ImageBrowser::m_TextCtrl_MaskPosH_OnText(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_MaskStart.y);
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox
    Handles pressing the mipmaps checkbox
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }

    // TODO: Check if we can generate Mipmaps to begin with (requires texture occupy <= 2048 bytes)
    this->m_LoadedAsset->m_UseMipmaps = event.IsChecked();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Choice_Dithering_OnChoice
    Handles changing the dithering choice box
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Choice_Dithering_OnChoice(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_Dithering = (P64Img_DitheringMode)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton
    Handles changing the alpha radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_AlphaMode = ALPHA_NONE;
    this->m_ColourPicker_AlphaColor->Disable();
    this->m_BitmapButton_Pipette->Disable();
    this->m_FilePicker_Alpha->Disable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton
    Handles changing the alpha radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_AlphaMode = ALPHA_MASK;
    this->m_ColourPicker_AlphaColor->Disable();
    this->m_BitmapButton_Pipette->Disable();
    this->m_FilePicker_Alpha->Disable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton
    Handles changing the alpha radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_AlphaMode = ALPHA_CUSTOM;
    this->m_ColourPicker_AlphaColor->Enable();
    this->m_BitmapButton_Pipette->Enable();
    this->m_FilePicker_Alpha->Disable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged
    Handles changing the alpha color in the color picker
    @param The ColourPicker event
==============================*/

void Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged(wxColourPickerEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_AlphaColor = event.GetColour();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick
    Handles pressing the pipette button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick(wxCommandEvent& event)
{
    wxSetCursor(Cursor_Pipette); // Works on Linux
    wxWindow::SetCursor(Cursor_Pipette); // Works on Windows
    this->m_UsingPipette = true;
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton
    Handles changing the alpha radio button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton(wxCommandEvent& event)
{
    if (this->m_LoadedAsset == NULL)
    {
        event.Skip();
        return;
    }
    this->m_LoadedAsset->m_AlphaMode = ALPHA_EXTERNALMASK;
    this->m_ColourPicker_AlphaColor->Disable();
    this->m_BitmapButton_Pipette->Disable();
    this->m_FilePicker_Alpha->Enable();
    this->MarkAssetModified();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged
    Handles changing the alpha image file path
    @param The FileDirPicker event
==============================*/

void Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged(wxFileDirPickerEvent& event)
{
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::m_Button_Palette_OnButtonClick
    Handles pressing the palette button
    @param The command event
==============================*/

void Frame_ImageBrowser::m_Button_Palette_OnButtonClick(wxCommandEvent& event)
{
    wxMessageDialog dialog(this, "This feature is not yet available.", "Whoops", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_WARNING);
    dialog.ShowModal();
    event.Skip();
}


/*==============================
    Frame_ImageBrowser::MarkAssetModified
    Mark the asset as modified and regenerate its preview
==============================*/

void Frame_ImageBrowser::MarkAssetModified()
{
    if (this->m_LoadedAsset == NULL)
        return;
    this->m_AssetModified = true;
    this->UpdateTitle();
    this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
}


/*==============================
    Frame_ImageBrowser::SaveChanges
    Save changes made to the file
==============================*/

void Frame_ImageBrowser::SaveChanges()
{
    wxFile file;
    bool refresh = false;
    std::vector<uint8_t> data;

    // Check we have an asset loaded to begin with
    if (this->m_LoadedAsset == NULL)
        return;

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
    Frame_ImageBrowser::UpdateTitle
    Updates the frame title to reflect the file's name and its
    modification status
==============================*/

void Frame_ImageBrowser::UpdateTitle()
{
    if (this->m_AssetModified)
        this->SetTitle(this->m_AssetFilePath.GetName() + "* - " + this->m_Title);
    else
        this->SetTitle(this->m_AssetFilePath.GetName() + " - " + this->m_Title);
}


/*==============================
    Frame_ImageBrowser::UpdateFilePath
    Updates the current asset's file path.
    Should only be used if the file file was renamed and thus
    it needs to point to a new path.
    @param The new path of the asset
==============================*/

void Frame_ImageBrowser::UpdateFilePath(wxFileName path)
{
    this->m_AssetFilePath = path;
    this->UpdateTitle();
}


/*==============================
    Frame_ImageBrowser::LoadAsset
    Loads an image asset from a given path
    @param  The path of the asset
    @return The loaded asset, or NULL
==============================*/

P64Asset_Image* Frame_ImageBrowser::LoadAsset(wxFileName path)
{
    wxFile file;
    std::vector<uint8_t> data;
    P64Asset_Image* ret;

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
    ret = P64Asset_Image::Deserialize(data);
    if (ret == NULL)
        return NULL;
    if (this->m_LoadedAsset != NULL)
        delete this->m_LoadedAsset;
    this->m_LoadedAsset = ret;
    this->m_AssetModified = false;

    // Set asset content
    this->m_FilePicker_Image->SetPath(ret->m_SourcePath.GetFullPath());
    if (wxFileExists(path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + ret->m_SourcePath.GetName() + "." + ret->m_SourcePath.GetExt()))
        ret->m_Image.LoadFile(path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + ret->m_SourcePath.GetName() + "." + ret->m_SourcePath.GetExt());
    ret->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->SetAsset(ret);

    // Activate the window
    this->m_ToolBar_Preview->Enable();
    this->m_Notebook_Config->Enable();

    // Set panel item values based on asset data
    switch (ret->m_ResizeMode)
    {
        case RESIZETYPE_NONE:
            this->m_RadioBtn_ResizeNone->SetValue(true);
            if (ret->m_Image.IsOk())
            {
                this->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", ret->m_Image.GetWidth()));
                this->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", ret->m_Image.GetHeight()));
            }
            else
            {
                this->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", 0));
                this->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", 0));
            }
            this->m_TextCtrl_ResizeW->Disable();
            this->m_TextCtrl_ResizeH->Disable();
            this->m_Choice_Align->Disable();
            this->m_Choice_ResizeFill->Disable();
            break;
        case RESIZETYPE_POWER2:
            this->m_RadioBtn_ResizeTwo->SetValue(true);
            if (ret->m_Image.IsOk())
            {
                this->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", ret->m_Image.GetWidth()));
                this->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", ret->m_Image.GetHeight()));
            }
            else
            {
                this->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", 0));
                this->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", 0));
            }
            this->m_TextCtrl_ResizeW->Disable();
            this->m_TextCtrl_ResizeH->Disable();
            this->m_Choice_Align->Enable();
            this->m_Choice_ResizeFill->Enable();
            break;
        case RESIZETYPE_CUSTOM:
            this->m_RadioBtn_ResizeCustom->SetValue(true);
            this->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", ret->m_CustomSize.x));
            this->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", ret->m_CustomSize.y));
            this->m_TextCtrl_ResizeW->Enable();
            this->m_TextCtrl_ResizeH->Enable();
            this->m_Choice_Align->Enable();
            this->m_Choice_ResizeFill->Enable();
            break;
    }
    this->m_Choice_Align->SetSelection(ret->m_Alignment);
    this->m_Choice_ResizeFill->SetSelection(ret->m_ResizeFill);
    this->m_Choice_TilingX->SetSelection(ret->m_TilingX);
    this->m_Choice_TilingY->SetSelection(ret->m_TilingY);
    this->m_Choice_Format->SetSelection(ret->m_ImageFormat);
    this->m_TextCtrl_MaskPosW->ChangeValue(wxString::Format("%d", ret->m_MaskStart.x));
    this->m_TextCtrl_MaskPosH->ChangeValue(wxString::Format("%d", ret->m_MaskStart.y));
    this->m_Checkbox_Mipmaps->SetValue(ret->m_UseMipmaps);
    this->m_Choice_Dithering->SetSelection(ret->m_Dithering);
    switch (ret->m_AlphaMode)
    {
        case ALPHA_NONE:
            this->m_RadioBtn_AlphaNone->SetValue(true);
            this->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            this->m_FilePicker_Alpha->SetPath("");
            this->m_ColourPicker_AlphaColor->Disable();
            this->m_BitmapButton_Pipette->Disable();
            this->m_FilePicker_Alpha->Disable();
            break;
        case ALPHA_MASK:
            this->m_RadioBtn_AlphaMask->SetValue(true);
            this->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            this->m_FilePicker_Alpha->SetPath("");
            this->m_ColourPicker_AlphaColor->Disable();
            this->m_BitmapButton_Pipette->Disable();
            this->m_FilePicker_Alpha->Disable();
            break;
        case ALPHA_CUSTOM:
            this->m_RadioBtn_AlphaColor->SetValue(true);
            this->m_ColourPicker_AlphaColor->SetColour(ret->m_AlphaColor);
            this->m_FilePicker_Alpha->SetPath("");
            this->m_ColourPicker_AlphaColor->Enable();
            this->m_BitmapButton_Pipette->Enable();
            this->m_FilePicker_Alpha->Disable();
            break;
        case ALPHA_EXTERNALMASK:
            this->m_RadioBtn_AlphaColor->SetValue(true);
            this->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            this->m_FilePicker_Alpha->SetPath(ret->m_AlphaPath.GetFullPath());
            this->m_ColourPicker_AlphaColor->Disable();
            this->m_BitmapButton_Pipette->Disable();
            this->m_FilePicker_Alpha->Enable();
            break;
    }

    // Finalize
    this->UpdateTitle();
    this->m_ScrolledWin_Preview->ZoomReset();
    return ret;
}


/*==============================
    Frame_ImageBrowser::IsAssetModified
    Check the asset's modification status
    @return Whether the asset was recently modified
==============================*/

bool Frame_ImageBrowser::IsAssetModified()
{
    return this->m_AssetModified;
}


/*==============================
    Frame_ImageBrowser::GetLoadedAsset
    Get the currently loaded asset
    @return The loaded asset, or NULL
==============================*/

P64Asset_Image* Frame_ImageBrowser::GetLoadedAsset()
{
    return this->m_LoadedAsset;
}


/*==============================
    Frame_ImageBrowser::GetLoadedAssetPath
    Get the currently loaded asset's path
    @return The loaded asset's path
==============================*/

wxFileName Frame_ImageBrowser::GetLoadedAssetPath()
{
    return this->m_AssetFilePath;
}