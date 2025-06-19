/***************************************************************
                        frame_image.cpp

TODO
***************************************************************/

#include "frame_image.h"
#include "../../resource.h"
#include "../../main.h"

#define CONTENT_FOLDER     wxString("Images")
#define CONTENT_NAME       wxString("Image")
#define CONTENT_EXTENSION  wxString("*.p64_img")

static wxIcon IconGenerator(bool large)
{
    return Icon_Texture;
}

static void AssetGenerator(wxFileName path)
{
    return;
}

static void AssetLoad(wxFileName path)
{
    return;
}

Frame_ImageBrowser::Frame_ImageBrowser(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer(wxVERTICAL);

    m_Splitter_Vertical = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_Splitter_Vertical->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_VerticalOnIdle), NULL, this);

    this->m_Panel_Search = new Panel_Search(m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    this->m_Panel_Search->Search_SetAssetType(CONTENT_NAME, CONTENT_EXTENSION);
    this->m_Panel_Search->Search_IconGenerator(IconGenerator);
    this->m_Panel_Search->Search_NewAssetGenerator(AssetGenerator);
    this->m_Panel_Search->Search_LoadAssetFunc(AssetLoad);
    this->m_Panel_Search->Search_SetFolder(((Frame_Main*)this->GetParent())->GetAssetsPath() + CONTENT_FOLDER + wxFileName::GetPathSeparator());
    m_Panel_Edit = new wxPanel(m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Edit;
    m_Sizer_Edit = new wxBoxSizer(wxVERTICAL);

    m_Splitter_Horizontal = new wxSplitterWindow(m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_Splitter_Horizontal->SetSashGravity(1);
    m_Splitter_Horizontal->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_HorizontalOnIdle), NULL, this);

    m_Panel_Preview = new wxPanel(m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_Panel_Preview->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME));

    wxBoxSizer* m_Sizer_Preview;
    m_Sizer_Preview = new wxBoxSizer(wxVERTICAL);

    m_ToolBar_Preview = new wxToolBar(m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);
    m_Tool_Alpha = m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_ToggleAlpha, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of alpha"), wxEmptyString, NULL);
    
    m_Tool_Tiling = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleTiling, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image tiling"), wxEmptyString, NULL);

    m_Tool_Filtering = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleFilter, wxNullBitmap, wxITEM_NORMAL, _("Toggle simulation of the N64's 3-point bilinear filtering"), wxEmptyString, NULL);

    m_Tool_PalettePreview = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_Palette, wxNullBitmap, wxITEM_NORMAL, _("Swap the palette for preview"), wxEmptyString, NULL);

    m_Tool_Statistics = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleStatistics, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image statistics"), wxEmptyString, NULL);

    m_ToolBar_Preview->AddSeparator();

    m_Tool_ZoomIn = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomIn, wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), wxEmptyString, NULL);

    m_Tool_ZoomOut = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomOut, wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), wxEmptyString, NULL);

    m_Tool_ZoomNone = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomFit, wxNullBitmap, wxITEM_NORMAL, _("No zoom"), wxEmptyString, NULL);

    m_ToolBar_Preview->AddSeparator();

    m_Tool_FlashcartUpload = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_USBUpload, wxNullBitmap, wxITEM_NORMAL, _("Upload texture to flashcart"), wxEmptyString, NULL);

    m_ToolBar_Preview->Realize();

    m_Sizer_Preview->Add(m_ToolBar_Preview, 0, wxEXPAND, 5);

    m_ScrolledWin_Preview = new wxScrolledWindow(m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL);
    m_ScrolledWin_Preview->SetScrollRate(5, 5);
    m_Sizer_Preview->Add(m_ScrolledWin_Preview, 1, wxEXPAND, 5);


    m_Panel_Preview->SetSizer(m_Sizer_Preview);
    m_Panel_Preview->Layout();
    m_Sizer_Preview->Fit(m_Panel_Preview);
    m_Panel_Config = new wxPanel(m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Config;
    m_Sizer_Config = new wxBoxSizer(wxVERTICAL);

    m_notebook1 = new wxNotebook(m_Panel_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_Panel_ImageData = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageData;
    m_Sizer_ImageData = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageData->AddGrowableCol(1);
    m_Sizer_ImageData->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageData->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Image = new wxStaticText(m_Panel_ImageData, wxID_ANY, _("Image path:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Image->Wrap(-1);
    m_Sizer_ImageData->Add(m_StaticText_Image, 0, wxALL, 5);

    m_FilePicker_Image = new wxFilePickerCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE);
    m_FilePicker_Image->SetToolTip(_("The path for the texture image to load"));

    m_Sizer_ImageData->Add(m_FilePicker_Image, 0, wxALL|wxEXPAND, 5);

    m_StaticText_Resize = new wxStaticText(m_Panel_ImageData, wxID_ANY, _("Resize:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Resize->Wrap(-1);
    m_Sizer_ImageData->Add(m_StaticText_Resize, 0, wxALL, 5);


    m_Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_ResizeNone = new wxRadioButton(m_Panel_ImageData, wxID_ANY, _("No change"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_ResizeNone->SetValue(true);
    m_RadioBtn_ResizeNone->SetToolTip(_("Do not resize the image"));

    m_Sizer_ImageData->Add(m_RadioBtn_ResizeNone, 0, wxALL, 5);


    m_Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_ResizeTwo = new wxRadioButton(m_Panel_ImageData, wxID_ANY, _("Nearest power of two"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_ResizeTwo->SetToolTip(_("Resize the image so that its size goes to the nearest power of two (16, 32, 64, etc...)"));

    m_Sizer_ImageData->Add(m_RadioBtn_ResizeTwo, 0, wxALL, 5);


    m_Sizer_ImageData->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_ResizeCustom = new wxRadioButton(m_Panel_ImageData, wxID_ANY, _("Custom size:"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_ResizeCustom->SetToolTip(_("Set a custom resize size for the image"));

    m_Sizer_ImageData->Add(m_RadioBtn_ResizeCustom, 0, wxALL, 5);

    wxGridSizer* m_Sizer_ResizeCustom;
    m_Sizer_ResizeCustom = new wxGridSizer(0, 2, 0, 0);

    m_TextCtrl_ResizeW = new wxTextCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), 0);
    m_TextCtrl_ResizeW->Enable(false);
    m_TextCtrl_ResizeW->SetToolTip(_("Size of image width (in pixels)"));

    m_Sizer_ResizeCustom->Add(m_TextCtrl_ResizeW, 0, wxALL, 5);

    m_TextCtrl_ResizeH = new wxTextCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), 0);
    m_TextCtrl_ResizeH->Enable(false);
    m_TextCtrl_ResizeH->SetToolTip(_("Size of image height (in pixels)"));

    m_Sizer_ResizeCustom->Add(m_TextCtrl_ResizeH, 0, wxALL, 5);


    m_Sizer_ImageData->Add(m_Sizer_ResizeCustom, 1, wxEXPAND, 5);

    m_StaticText_Align = new wxStaticText(m_Panel_ImageData, wxID_ANY, _("Resize alignment:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Align->Wrap(-1);
    m_Sizer_ImageData->Add(m_StaticText_Align, 0, wxALL, 5);

    wxString m_Choice_AlignChoices[] = { _("Top left"), _("Top middle"), _("Top right"), _("Middle left"), _("Center"), _("Middle right"), _("Bottom left"), _("Bottom middle"), _("Bottom right") };
    int m_Choice_AlignNChoices = sizeof(m_Choice_AlignChoices) / sizeof(wxString);
    m_Choice_Align = new wxChoice(m_Panel_ImageData, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_AlignNChoices, m_Choice_AlignChoices, 0);
    m_Choice_Align->SetSelection(4);
    m_Choice_Align->Enable(false);
    m_Choice_Align->SetToolTip(_("Set where to align the image to when resizing"));

    m_Sizer_ImageData->Add(m_Choice_Align, 0, wxALL|wxEXPAND, 5);

    m_StaticText_ResizeFill = new wxStaticText(m_Panel_ImageData, wxID_ANY, _("Fill space with:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_ResizeFill->Wrap(-1);
    m_Sizer_ImageData->Add(m_StaticText_ResizeFill, 0, wxALL, 5);

    wxString m_Choice_ResizeFillChoices[] = { _("Invisible color"), _("Edge colors"), _("Repeated texture") };
    int m_Choice_ResizeFillNChoices = sizeof(m_Choice_ResizeFillChoices) / sizeof(wxString);
    m_Choice_ResizeFill = new wxChoice(m_Panel_ImageData, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_ResizeFillNChoices, m_Choice_ResizeFillChoices, 0);
    m_Choice_ResizeFill->SetSelection(0);
    m_Choice_ResizeFill->Enable(false);
    m_Choice_ResizeFill->SetToolTip(_("When resizing larger than the base size, what color should be used to fill the new space?"));

    m_Sizer_ImageData->Add(m_Choice_ResizeFill, 0, wxALL|wxEXPAND, 5);


    m_Panel_ImageData->SetSizer(m_Sizer_ImageData);
    m_Panel_ImageData->Layout();
    m_Sizer_ImageData->Fit(m_Panel_ImageData);
    m_notebook1->AddPage(m_Panel_ImageData, _("Image"), false);
    m_Panel_ImageLoading = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageLoading;
    m_Sizer_ImageLoading = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageLoading->AddGrowableCol(1);
    m_Sizer_ImageLoading->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageLoading->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Format = new wxStaticText(m_Panel_ImageLoading, wxID_ANY, _("Format:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Format->Wrap(-1);
    m_Sizer_ImageLoading->Add(m_StaticText_Format, 0, wxALL, 5);

    wxString m_Choice_FormatChoices[] = { _("32-Bit RGBA"), _("16-Bit RGBA"), _("16-Bit IA"), _("8-Bit IA"), _("4-Bit IA"), _("8-Bit I"), _("4-Bit I"), _("8-Bit CI"), _("4-Bit CI") };
    int m_Choice_FormatNChoices = sizeof(m_Choice_FormatChoices) / sizeof(wxString);
    m_Choice_Format = new wxChoice(m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_FormatNChoices, m_Choice_FormatChoices, 0);
    m_Choice_Format->SetSelection(1);
    m_Choice_Format->SetToolTip(_("Image loading format. Different formats allow for larger image sizes at the expense of color."));

    m_Sizer_ImageLoading->Add(m_Choice_Format, 0, wxALL|wxEXPAND, 5);

    m_StaticText_TilingMode = new wxStaticText(m_Panel_ImageLoading, wxID_ANY, _("Tiling mode:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_TilingMode->Wrap(-1);
    m_Sizer_ImageLoading->Add(m_StaticText_TilingMode, 0, wxALL, 5);

    wxGridSizer* m_Sizer_TilingMode;
    m_Sizer_TilingMode = new wxGridSizer(0, 2, 0, 0);

    wxString m_Choice_TilingXChoices[] = { _("Mirror"), _("Wrap"), _("Clamp") };
    int m_Choice_TilingXNChoices = sizeof(m_Choice_TilingXChoices) / sizeof(wxString);
    m_Choice_TilingX = new wxChoice(m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_TilingXNChoices, m_Choice_TilingXChoices, 0);
    m_Choice_TilingX->SetSelection(0);
    m_Choice_TilingX->SetToolTip(_("Horizontal tiling mode"));

    m_Sizer_TilingMode->Add(m_Choice_TilingX, 0, wxALL|wxEXPAND, 5);

    wxString m_Choice_TilingYChoices[] = { _("Mirror"), _("Wrap"), _("Clamp") };
    int m_Choice_TilingYNChoices = sizeof(m_Choice_TilingYChoices) / sizeof(wxString);
    m_Choice_TilingY = new wxChoice(m_Panel_ImageLoading, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_TilingYNChoices, m_Choice_TilingYChoices, 0);
    m_Choice_TilingY->SetSelection(0);
    m_Choice_TilingY->SetToolTip(_("Vertical tiling mode"));

    m_Sizer_TilingMode->Add(m_Choice_TilingY, 0, wxALL|wxEXPAND, 5);


    m_Sizer_ImageLoading->Add(m_Sizer_TilingMode, 1, wxEXPAND, 5);

    m_StaticText_MaskPos = new wxStaticText(m_Panel_ImageLoading, wxID_ANY, _("Mask start:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_MaskPos->Wrap(-1);
    m_Sizer_ImageLoading->Add(m_StaticText_MaskPos, 0, wxALL, 5);

    wxGridSizer* m_Sizer_MaskPos;
    m_Sizer_MaskPos = new wxGridSizer(0, 2, 0, 0);

    m_TextCtrl_MaskPosW = new wxTextCtrl(m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_TextCtrl_MaskPosW->SetToolTip(_("X coordinate start of the image mask"));

    m_Sizer_MaskPos->Add(m_TextCtrl_MaskPosW, 0, wxALL, 5);

    m_TextCtrl_MaskPosH = new wxTextCtrl(m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), 0);
    m_TextCtrl_MaskPosH->SetToolTip(_("Y coordinate start of the image mask"));

    m_Sizer_MaskPos->Add(m_TextCtrl_MaskPosH, 0, wxALL, 5);


    m_Sizer_ImageLoading->Add(m_Sizer_MaskPos, 1, wxEXPAND, 5);

    m_Checkbox_Mipmaps = new wxCheckBox(m_Panel_ImageLoading, wxID_ANY, _("Generate mipmaps"), wxDefaultPosition, wxDefaultSize, 0);
    m_Checkbox_Mipmaps->SetToolTip(_("Generate mipmaps for this texture (requires the image to only take up half of TMEM!)"));

    m_Sizer_ImageLoading->Add(m_Checkbox_Mipmaps, 0, wxALL, 5);


    m_Panel_ImageLoading->SetSizer(m_Sizer_ImageLoading);
    m_Panel_ImageLoading->Layout();
    m_Sizer_ImageLoading->Fit(m_Panel_ImageLoading);
    m_notebook1->AddPage(m_Panel_ImageLoading, _("Loading"), false);
    m_Panel_ImageColors = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageColors;
    m_Sizer_ImageColors = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageColors->AddGrowableCol(1);
    m_Sizer_ImageColors->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageColors->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Quantization = new wxStaticText(m_Panel_ImageColors, wxID_ANY, _("Quantization:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Quantization->Wrap(-1);
    m_Sizer_ImageColors->Add(m_StaticText_Quantization, 0, wxALL, 5);

    wxString m_Choice_QuantizationChoices[] = { _("None"), _("Median cut") };
    int m_Choice_QuantizationNChoices = sizeof(m_Choice_QuantizationChoices) / sizeof(wxString);
    m_Choice_Quantization = new wxChoice(m_Panel_ImageColors, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_QuantizationNChoices, m_Choice_QuantizationChoices, 0);
    m_Choice_Quantization->SetSelection(1);
    m_Choice_Quantization->SetToolTip(_("What algorithm to use to reduce the colors"));

    m_Sizer_ImageColors->Add(m_Choice_Quantization, 0, wxALL|wxEXPAND, 5);

    m_StaticText_AlphaChoice = new wxStaticText(m_Panel_ImageColors, wxID_ANY, _("Alpha:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_AlphaChoice->Wrap(-1);
    m_Sizer_ImageColors->Add(m_StaticText_AlphaChoice, 0, wxALL, 5);


    m_Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_AlphaNone = new wxRadioButton(m_Panel_ImageColors, wxID_ANY, _("No Alpha"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_AlphaNone->SetToolTip(_("Do not use any alpha"));

    m_Sizer_ImageColors->Add(m_RadioBtn_AlphaNone, 0, wxALL, 5);


    m_Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_AlphaMask = new wxRadioButton(m_Panel_ImageColors, wxID_ANY, _("Alpha mask"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_AlphaMask->SetValue(true);
    m_RadioBtn_AlphaMask->SetToolTip(_("Use the image's built-in alpha mask (such as PNG transparency)"));

    m_Sizer_ImageColors->Add(m_RadioBtn_AlphaMask, 0, wxALL, 5);


    m_Sizer_ImageColors->Add(0, 0, 1, wxEXPAND, 5);

    m_RadioBtn_AlphaColor = new wxRadioButton(m_Panel_ImageColors, wxID_ANY, _("Remove color:"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_AlphaColor->SetToolTip(_("Treat a specific color in the image as the alpha color"));

    m_Sizer_ImageColors->Add(m_RadioBtn_AlphaColor, 0, wxALL, 5);

    wxFlexGridSizer* m_Sizer_AlphaColor;
    m_Sizer_AlphaColor = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_AlphaColor->AddGrowableCol(0);
    m_Sizer_AlphaColor->SetFlexibleDirection(wxBOTH);
    m_Sizer_AlphaColor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_ColourPicker_AlphaColor = new wxColourPickerCtrl(m_Panel_ImageColors, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
    m_ColourPicker_AlphaColor->Enable(false);
    m_ColourPicker_AlphaColor->SetToolTip(_("Color picker for the alpha color"));

    m_Sizer_AlphaColor->Add(m_ColourPicker_AlphaColor, 0, wxALL|wxEXPAND, 5);

    m_BitmapButton_Pipette = new wxBitmapButton(m_Panel_ImageColors, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0);
    m_BitmapButton_Pipette->Enable(false);
    m_BitmapButton_Pipette->SetToolTip(_("Pick the color by clicking on the image"));

    m_Sizer_AlphaColor->Add(m_BitmapButton_Pipette, 0, wxALL, 5);


    m_Sizer_ImageColors->Add(m_Sizer_AlphaColor, 1, wxEXPAND, 5);

    m_RadioBtn_AlphaExternal = new wxRadioButton(m_Panel_ImageColors, wxID_ANY, _("External mask:"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_AlphaExternal->SetToolTip(_("Use an external image for the alpha mask (requires the mask be the same size as the source image)"));

    m_Sizer_ImageColors->Add(m_RadioBtn_AlphaExternal, 0, wxALL, 5);

    m_FilePicker_Alpha = new wxFilePickerCtrl(m_Panel_ImageColors, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE);
    m_FilePicker_Alpha->Enable(false);
    m_FilePicker_Alpha->SetToolTip(_("Filepath for the external alpha mask"));

    m_Sizer_ImageColors->Add(m_FilePicker_Alpha, 0, wxALL|wxEXPAND, 5);

    m_Button_Palette = new wxButton(m_Panel_ImageColors, wxID_ANY, _("Configure Palette"), wxDefaultPosition, wxDefaultSize, 0);
    m_Button_Palette->Enable(false);
    m_Button_Palette->SetToolTip(_("Configure Color Index palette"));

    m_Sizer_ImageColors->Add(m_Button_Palette, 0, wxALL, 5);


    m_Panel_ImageColors->SetSizer(m_Sizer_ImageColors);
    m_Panel_ImageColors->Layout();
    m_Sizer_ImageColors->Fit(m_Panel_ImageColors);
    m_notebook1->AddPage(m_Panel_ImageColors, _("Colors"), true);

    m_Sizer_Config->Add(m_notebook1, 1, wxEXPAND | wxALL, 5);


    m_Panel_Config->SetSizer(m_Sizer_Config);
    m_Panel_Config->Layout();
    m_Sizer_Config->Fit(m_Panel_Config);
    m_Splitter_Horizontal->SplitHorizontally(m_Panel_Preview, m_Panel_Config, 0);
    m_Sizer_Edit->Add(m_Splitter_Horizontal, 1, wxEXPAND, 5);


    m_Panel_Edit->SetSizer(m_Sizer_Edit);
    m_Panel_Edit->Layout();
    m_Sizer_Edit->Fit(m_Panel_Edit);
    m_Splitter_Vertical->SplitVertically(m_Panel_Search, m_Panel_Edit, 0);
    m_Sizer_Main->Add(m_Splitter_Vertical, 1, wxEXPAND, 5);


    this->SetSizer(m_Sizer_Main);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(m_Tool_Alpha->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked));
    this->Connect(m_Tool_Tiling->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked));
    this->Connect(m_Tool_Filtering->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked));
    this->Connect(m_Tool_PalettePreview->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked));
    this->Connect(m_Tool_Statistics->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked));
    this->Connect(m_Tool_ZoomIn->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked));
    this->Connect(m_Tool_ZoomOut->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked));
    this->Connect(m_Tool_ZoomNone->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked));
    this->Connect(m_Tool_FlashcartUpload->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked));
    m_FilePicker_Image->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(Frame_ImageBrowser::m_FilePicker_Image_OnFileChanged), NULL, this);
    m_RadioBtn_ResizeNone->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton), NULL, this);
    m_RadioBtn_ResizeTwo->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton), NULL, this);
    m_RadioBtn_ResizeCustom->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton), NULL, this);
    m_TextCtrl_ResizeW->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText), NULL, this);
    m_TextCtrl_ResizeH->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText), NULL, this);
    m_Choice_Align->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Align_OnChoice), NULL, this);
    m_Choice_ResizeFill->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice), NULL, this);
    m_Choice_Format->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Format_OnChoice), NULL, this);
    m_Choice_TilingX->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_TilingX_OnChoice), NULL, this);
    m_Choice_TilingY->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_TilingY_OnChoice), NULL, this);
    m_TextCtrl_MaskPosW->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText), NULL, this);
    m_TextCtrl_MaskPosH->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Frame_ImageBrowser::m_TextCtrl_MaskPosH_OnText), NULL, this);
    m_Checkbox_Mipmaps->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox), NULL, this);
    m_Choice_Quantization->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_Choice_Quantization_OnChoice), NULL, this);
    m_RadioBtn_AlphaNone->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton), NULL, this);
    m_RadioBtn_AlphaMask->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton), NULL, this);
    m_RadioBtn_AlphaColor->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton), NULL, this);
    m_ColourPicker_AlphaColor->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged), NULL, this);
    m_BitmapButton_Pipette->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick), NULL, this);
    m_RadioBtn_AlphaExternal->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton), NULL, this);
    m_FilePicker_Alpha->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged), NULL, this);
    m_Button_Palette->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Button_Palette_OnButtonClick), NULL, this);
}

Frame_ImageBrowser::~Frame_ImageBrowser()
{
    
}

void Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_FilePicker_Image_OnFileChanged(wxFileDirPickerEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_Align_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_Format_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_TilingX_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_TilingY_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_TextCtrl_MaskPosH_OnText(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_Choice_Quantization_OnChoice(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged(wxColourPickerEvent& event)
{

}

void Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton(wxCommandEvent& event)
{

}

void Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged(wxFileDirPickerEvent& event)
{

}

void Frame_ImageBrowser::m_Button_Palette_OnButtonClick(wxCommandEvent& event)
{

}