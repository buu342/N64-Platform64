/***************************************************************
                        frame_image.cpp

TODO
***************************************************************/

#include "frame_image.h"
#include "../../resource.h"
#include "../../main.h"

#define VERSION_P64TEX 1

#define CONTENT_FOLDER     wxString("Images")
#define CONTENT_NAME       wxString("Image")
#define CONTENT_EXTENSION  wxString("*.p64_img")

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

static void AssetGenerator(wxFrame* frame, wxFileName path)
{
    wxFile file;
    P64Asset_Image asset;
    std::vector<uint8_t> data = asset.Serialize();
    file.Create(path.GetFullPath());
    file.Open(path.GetFullPath(), wxFile::write);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog((Frame_ImageBrowser*)frame, "Unable to open file for writing", "Error serializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }
    file.Write(data.data(), data.size());
    file.Close();
}

static void AssetLoad(wxFrame* frame, wxFileName path)
{
    wxFile file;
    Frame_ImageBrowser* realframe = (Frame_ImageBrowser*)frame;
    std::vector<uint8_t> data;
    P64Asset_Image* oldasset = realframe->m_LoadedAsset;
    P64Asset_Image* curasset;

    if (realframe->m_AssetModified)
    {
        wxMessageDialog dialog((Frame_ImageBrowser*)frame, "Unsaved changes will be lost. Continue?", "Warning", wxCENTER | wxYES | wxNO | wxNO_DEFAULT | wxICON_WARNING);
        if (dialog.ShowModal() == wxID_NO)
            return;
    }

    // Open the file and get its bytes
    realframe->m_AssetFilePath = path;
    data.resize(path.GetSize().ToULong());
    file.Open(path.GetFullPath(), wxFile::read);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog(realframe, "Unable to open file for reading", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }
    file.Read(&data[0], data.capacity());
    file.Close();

    // Create the asset object by deserializing the bytes
    curasset = P64Asset_Image::Deserialize(data);
    if (curasset == NULL)
        return;
    if (oldasset != NULL)
        delete oldasset;
    realframe->m_LoadedAsset = curasset;

    // Set asset content
    realframe->m_FilePicker_Image->SetPath(curasset->m_SourcePath.GetFullPath());
    if (wxFileExists(path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + curasset->m_SourcePath.GetName() + "." + curasset->m_SourcePath.GetExt()))
        curasset->m_Image.LoadFile(path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + curasset->m_SourcePath.GetName() + "." + curasset->m_SourcePath.GetExt());
    curasset->RegenerateFinal(realframe->m_ScrolledWin_Preview->GetAlphaDisplay(), realframe->m_ScrolledWin_Preview->GetFilterDisplay(), realframe->m_ScrolledWin_Preview->GetZoom());
    realframe->m_ScrolledWin_Preview->SetAsset(curasset);

    // Activate the window
    realframe->m_ToolBar_Preview->Enable(true);
    realframe->m_Notebook_Config->Enable(true);

    // Set panel item values based on asset data
    switch (curasset->m_ResizeMode)
    {
        case RESIZETYPE_NONE: 
            realframe->m_RadioBtn_ResizeNone->SetValue(true);
            if (curasset->m_Image.IsOk())
            {
                realframe->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", curasset->m_Image.GetWidth()));
                realframe->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", curasset->m_Image.GetHeight()));
            }
            else
            {
                realframe->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", 0));
                realframe->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", 0));
            }
            realframe->m_TextCtrl_ResizeW->Enable(false);
            realframe->m_TextCtrl_ResizeH->Enable(false);
            realframe->m_Choice_Align->Enable(false);
            realframe->m_Choice_ResizeFill->Enable(false);
            break;
        case RESIZETYPE_POWER2: 
            realframe->m_RadioBtn_ResizeTwo->SetValue(true);
            if (curasset->m_Image.IsOk())
            {
                realframe->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", curasset->m_Image.GetWidth()));
                realframe->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", curasset->m_Image.GetHeight()));
            }
            else
            {
                realframe->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", 0));
                realframe->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", 0));
            }
            realframe->m_TextCtrl_ResizeW->Enable(false);
            realframe->m_TextCtrl_ResizeH->Enable(false);
            realframe->m_Choice_Align->Enable(true);
            realframe->m_Choice_ResizeFill->Enable(true);
            break;
        case RESIZETYPE_CUSTOM: 
            realframe->m_RadioBtn_ResizeCustom->SetValue(true);
            realframe->m_TextCtrl_ResizeW->ChangeValue(wxString::Format("%d", curasset->m_CustomSize.x));
            realframe->m_TextCtrl_ResizeH->ChangeValue(wxString::Format("%d", curasset->m_CustomSize.y));
            realframe->m_TextCtrl_ResizeW->Enable(true);
            realframe->m_TextCtrl_ResizeH->Enable(true);
            realframe->m_Choice_Align->Enable(true);
            realframe->m_Choice_ResizeFill->Enable(true);
            break;
    }
    realframe->m_Choice_Align->SetSelection(curasset->m_Alignment);
    realframe->m_Choice_ResizeFill->SetSelection(curasset->m_ResizeFill);
    realframe->m_Choice_TilingX->SetSelection(curasset->m_TilingX);
    realframe->m_Choice_TilingY->SetSelection(curasset->m_TilingY);
    realframe->m_Choice_Format->SetSelection(curasset->m_ImageFormat);
    realframe->m_TextCtrl_MaskPosW->ChangeValue(wxString::Format("%d", curasset->m_MaskStart.x));
    realframe->m_TextCtrl_MaskPosH->ChangeValue(wxString::Format("%d", curasset->m_MaskStart.y));
    realframe->m_Checkbox_Mipmaps->SetValue(curasset->m_UseMipmaps);
    realframe->m_Choice_Dithering->SetSelection(curasset->m_Dithering);
    switch (curasset->m_AlphaMode)
    {
        case ALPHA_NONE:
            realframe->m_RadioBtn_AlphaNone->SetValue(true);
            realframe->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            realframe->m_FilePicker_Alpha->SetPath("");
            realframe->m_ColourPicker_AlphaColor->Enable(false);
            realframe->m_BitmapButton_Pipette->Enable(false);
            realframe->m_FilePicker_Alpha->Enable(false);
            break;
        case ALPHA_MASK:
            realframe->m_RadioBtn_AlphaMask->SetValue(true);
            realframe->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            realframe->m_FilePicker_Alpha->SetPath("");
            realframe->m_ColourPicker_AlphaColor->Enable(false);
            realframe->m_BitmapButton_Pipette->Enable(false);
            realframe->m_FilePicker_Alpha->Enable(false);
            break;
        case ALPHA_CUSTOM:
            realframe->m_RadioBtn_AlphaColor->SetValue(true);
            realframe->m_ColourPicker_AlphaColor->SetColour(curasset->m_AlphaColor);
            realframe->m_FilePicker_Alpha->SetPath("");
            realframe->m_ColourPicker_AlphaColor->Enable(true);
            realframe->m_BitmapButton_Pipette->Enable(true);
            realframe->m_FilePicker_Alpha->Enable(false);
            break;
        case ALPHA_EXTERNALMASK:
            realframe->m_RadioBtn_AlphaColor->SetValue(true);
            realframe->m_ColourPicker_AlphaColor->SetColour(*wxBLACK);
            realframe->m_FilePicker_Alpha->SetPath(curasset->m_AlphaPath.GetFullPath());
            realframe->m_ColourPicker_AlphaColor->Enable(false);
            realframe->m_BitmapButton_Pipette->Enable(false);
            realframe->m_FilePicker_Alpha->Enable(true);
            break;
    }

    // Finalize
    realframe->SetTitle(path.GetName() + " - " + realframe->m_Title);
    realframe->m_AssetModified = false;
    realframe->m_ScrolledWin_Preview->ZoomReset();
}

static void AssetRename(wxFrame* frame, wxFileName oldname, wxFileName newname)
{
    Frame_ImageBrowser* realframe = (Frame_ImageBrowser*)frame;
    if (realframe->m_AssetFilePath == oldname)
    {
        realframe->m_AssetFilePath = newname;
        if (realframe->m_AssetModified)
            realframe->SetTitle(realframe->m_AssetFilePath.GetName() + "* - " + realframe->m_Title);
        else
            realframe->SetTitle(realframe->m_AssetFilePath.GetName() + " - " + realframe->m_Title);
    }
}

Frame_ImageBrowser::Frame_ImageBrowser(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->m_Title = title;
    this->m_LoadedAsset = NULL;
    this->m_AssetModified = false;
    this->m_UsingPipette = false;
    this->m_AssetFilePath = wxFileName("");

    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer(wxVERTICAL);

    m_Splitter_Vertical = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_Splitter_Vertical->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_VerticalOnIdle), NULL, this);
    m_Splitter_Vertical->SetMinimumPaneSize(1);

    this->m_Panel_Search = new Panel_Search(m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    this->m_Panel_Search->Search_SetAssetType(CONTENT_NAME, CONTENT_EXTENSION);
    this->m_Panel_Search->Search_IconGenerator(IconGenerator);
    this->m_Panel_Search->Search_NewAssetGenerator(AssetGenerator);
    this->m_Panel_Search->Search_LoadAssetFunc(AssetLoad);
    this->m_Panel_Search->Search_RenameAssetFunc(AssetRename);
    this->m_Panel_Search->Search_SetFolder(((Frame_Main*)this->GetParent())->GetAssetsPath() + CONTENT_FOLDER + wxFileName::GetPathSeparator());
    this->m_Panel_Search->Search_SetTarget(this);

    m_Panel_Edit = new wxPanel(m_Splitter_Vertical, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxWANTS_CHARS);
    wxBoxSizer* m_Sizer_Edit;
    m_Sizer_Edit = new wxBoxSizer(wxVERTICAL);

    m_Splitter_Horizontal = new wxSplitterWindow(m_Panel_Edit, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
    m_Splitter_Horizontal->SetSashGravity(1);
    m_Splitter_Horizontal->Connect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    m_Splitter_Horizontal->SetMinimumPaneSize(1);

    m_Panel_Preview = new wxPanel(m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    m_Panel_Preview->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME));

    wxBoxSizer* m_Sizer_Preview;
    m_Sizer_Preview = new wxBoxSizer(wxVERTICAL);

    m_ToolBar_Preview = new wxToolBar(m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);
    m_ToolBar_Preview->Enable(false);
    m_Tool_Save = m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_Save, wxNullBitmap, wxITEM_NORMAL, _("Save changes"), wxEmptyString, NULL);
    m_ToolBar_Preview->AddSeparator();
    m_Tool_Alpha = m_ToolBar_Preview->AddTool(wxID_ANY, wxEmptyString, Icon_ToggleAlpha, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of alpha"), wxEmptyString, NULL);
    m_Tool_Tiling = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleTiling, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image tiling"), wxEmptyString, NULL);
    m_Tool_Filtering = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleFilter, wxNullBitmap, wxITEM_NORMAL, _("Toggle simulation of the N64's 3-point bilinear filtering"), wxEmptyString, NULL);
    //this->m_Tool_PalettePreview = this->m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_Palette, wxNullBitmap, wxITEM_NORMAL, _("Swap the palette for preview"), wxEmptyString, NULL);
    m_Tool_Statistics = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ToggleStatistics, wxNullBitmap, wxITEM_NORMAL, _("Toggle the display of image statistics"), wxEmptyString, NULL);
    m_ToolBar_Preview->AddSeparator();
    m_Tool_ZoomIn = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomIn, wxNullBitmap, wxITEM_NORMAL, _("Zoom in"), wxEmptyString, NULL);
    m_Tool_ZoomOut = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomOut, wxNullBitmap, wxITEM_NORMAL, _("Zoom out"), wxEmptyString, NULL);
    m_Tool_ZoomNone = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_ZoomFit, wxNullBitmap, wxITEM_NORMAL, _("No zoom"), wxEmptyString, NULL);
    m_ToolBar_Preview->AddSeparator();
    m_Tool_FlashcartUpload = m_ToolBar_Preview->AddTool(wxID_ANY, _("tool"), Icon_USBUpload, wxNullBitmap, wxITEM_NORMAL, _("Upload texture to flashcart"), wxEmptyString, NULL);
    m_ToolBar_Preview->Realize();

    m_Sizer_Preview->Add(m_ToolBar_Preview, 0, wxEXPAND, 5);

    m_ScrolledWin_Preview = new Panel_ImgView(m_Panel_Preview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL);
    m_ScrolledWin_Preview->SetScrollRate(5, 5);
    m_Sizer_Preview->Add(m_ScrolledWin_Preview, 1, wxEXPAND, 5);


    m_Panel_Preview->SetSizer(m_Sizer_Preview);
    m_Panel_Preview->Layout();
    m_Sizer_Preview->Fit(m_Panel_Preview);
    m_Panel_Config = new wxPanel(m_Splitter_Horizontal, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Config;
    m_Sizer_Config = new wxBoxSizer(wxVERTICAL);

    m_Notebook_Config = new wxNotebook(m_Panel_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_Notebook_Config->Enable(false);
    m_Panel_ImageData = new wxPanel(m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageData;
    m_Sizer_ImageData = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageData->AddGrowableCol(1);
    m_Sizer_ImageData->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageData->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Image = new wxStaticText(m_Panel_ImageData, wxID_ANY, _("Image path:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Image->Wrap(-1);
    m_Sizer_ImageData->Add(m_StaticText_Image, 0, wxALL, 5);

    m_FilePicker_Image = new wxFilePickerCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, _("Select a file"), _("Image files|*.bmp;*.gif;*.jpg;*.png"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    m_FilePicker_Image->SetToolTip(_("The path for the texture image to load"));
    //m_FilePicker_Image->SetDirectory(this->m_Panel_Search->GetMainFolder());

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

    m_TextCtrl_ResizeW = new wxTextCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    m_TextCtrl_ResizeW->Enable(false);
    m_TextCtrl_ResizeW->SetToolTip(_("Size of image width (in pixels)"));

    m_Sizer_ResizeCustom->Add(m_TextCtrl_ResizeW, 0, wxALL, 5);

    m_TextCtrl_ResizeH = new wxTextCtrl(m_Panel_ImageData, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
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

    wxString m_Choice_ResizeFillChoices[] = {_("Invisible color"), _("Edge colors"), _("Repeated texture"), _("Mirrored texture")};
    int m_Choice_ResizeFillNChoices = sizeof(m_Choice_ResizeFillChoices) / sizeof(wxString);
    m_Choice_ResizeFill = new wxChoice(m_Panel_ImageData, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_ResizeFillNChoices, m_Choice_ResizeFillChoices, 0);
    m_Choice_ResizeFill->SetSelection(0);
    m_Choice_ResizeFill->Enable(false);
    m_Choice_ResizeFill->SetToolTip(_("When resizing larger than the base size, what color should be used to fill the new space?"));

    m_Sizer_ImageData->Add(m_Choice_ResizeFill, 0, wxALL|wxEXPAND, 5);


    m_Panel_ImageData->SetSizer(m_Sizer_ImageData);
    m_Panel_ImageData->Layout();
    m_Sizer_ImageData->Fit(m_Panel_ImageData);
    m_Notebook_Config->AddPage(m_Panel_ImageData, _("Image"), true);
    m_Panel_ImageLoading = new wxPanel(m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageLoading;
    m_Sizer_ImageLoading = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageLoading->AddGrowableCol(1);
    m_Sizer_ImageLoading->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageLoading->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Format = new wxStaticText(m_Panel_ImageLoading, wxID_ANY, _("Format:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Format->Wrap(-1);
    m_Sizer_ImageLoading->Add(m_StaticText_Format, 0, wxALL, 5);

    wxString m_Choice_FormatChoices[] = { _("32-Bit RGBA"), _("16-Bit RGBA"), _("16-Bit IA"), _("8-Bit IA"), _("4-Bit IA"), _("8-Bit I"), _("4-Bit I")/*, _("8-Bit CI"), _("4-Bit CI")*/ };
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

    this->m_StaticText_MaskPos = new wxStaticText(this->m_Panel_ImageLoading, wxID_ANY, _("Mask start:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_StaticText_MaskPos->Wrap(-1);
    m_Sizer_ImageLoading->Add(this->m_StaticText_MaskPos, 0, wxALL, 5);
    wxGridSizer* m_Sizer_MaskPos;
    m_Sizer_MaskPos = new wxGridSizer(0, 2, 0, 0);
    this->m_TextCtrl_MaskPosW = new wxTextCtrl(this->m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_MaskPosW->SetToolTip(_("X coordinate start of the image mask"));
    m_Sizer_MaskPos->Add(this->m_TextCtrl_MaskPosW, 0, wxALL, 5);
    this->m_TextCtrl_MaskPosH = new wxTextCtrl(this->m_Panel_ImageLoading, wxID_ANY, _("0"), wxDefaultPosition, wxSize(-1,-1), wxEXPAND, wxTextValidator(wxFILTER_NUMERIC));
    this->m_TextCtrl_MaskPosH->SetToolTip(_("Y coordinate start of the image mask"));
    m_Sizer_MaskPos->Add(this->m_TextCtrl_MaskPosH, 0, wxALL, 5);
    m_Sizer_ImageLoading->Add(m_Sizer_MaskPos, 1, wxEXPAND, 5);

    m_Checkbox_Mipmaps = new wxCheckBox(m_Panel_ImageLoading, wxID_ANY, _("Generate mipmaps"), wxDefaultPosition, wxDefaultSize, 0);
    m_Checkbox_Mipmaps->SetToolTip(_("Generate mipmaps for this texture (requires the image to only take up half of TMEM!)"));

    m_Sizer_ImageLoading->Add(m_Checkbox_Mipmaps, 0, wxALL, 5);


    m_Panel_ImageLoading->SetSizer(m_Sizer_ImageLoading);
    m_Panel_ImageLoading->Layout();
    m_Sizer_ImageLoading->Fit(m_Panel_ImageLoading);
    m_Notebook_Config->AddPage(m_Panel_ImageLoading, _("Loading"), false);
    m_Panel_ImageColors = new wxPanel(m_Notebook_Config, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ImageColors;
    m_Sizer_ImageColors = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ImageColors->AddGrowableCol(1);
    m_Sizer_ImageColors->SetFlexibleDirection(wxBOTH);
    m_Sizer_ImageColors->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_StaticText_Dithering = new wxStaticText(m_Panel_ImageColors, wxID_ANY, _("Dithering:"), wxDefaultPosition, wxDefaultSize, 0);
    m_StaticText_Dithering->Wrap(-1);
    m_Sizer_ImageColors->Add(m_StaticText_Dithering, 0, wxALL, 5);

    wxString m_Choice_DitheringChoices[] = { _("None"), _("Ordered Dithering"), _("Floyd-Steinberg") };
    int m_Choice_DitheringNChoices = sizeof(m_Choice_DitheringChoices) / sizeof(wxString);
    m_Choice_Dithering = new wxChoice(m_Panel_ImageColors, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Choice_DitheringNChoices, m_Choice_DitheringChoices, 0);
    m_Choice_Dithering->SetSelection(1);
    m_Choice_Dithering->SetToolTip(_("What algorithm to use to reduce the colors"));

    m_Sizer_ImageColors->Add(m_Choice_Dithering, 0, wxALL|wxEXPAND, 5);

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
    m_RadioBtn_AlphaColor->SetToolTip(_("Treat a specific color in the unmodified image as the alpha color"));

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

    m_BitmapButton_Pipette = new wxBitmapButton(m_Panel_ImageColors, wxID_ANY, Icon_Pipette, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0);
    m_BitmapButton_Pipette->Enable(false);
    m_BitmapButton_Pipette->SetToolTip(_("Pick the color by clicking on the image"));

    m_Sizer_AlphaColor->Add(m_BitmapButton_Pipette, 0, wxALL, 5);


    m_Sizer_ImageColors->Add(m_Sizer_AlphaColor, 1, wxEXPAND, 5);

    m_RadioBtn_AlphaExternal = new wxRadioButton(m_Panel_ImageColors, wxID_ANY, _("External mask:"), wxDefaultPosition, wxDefaultSize, 0);
    m_RadioBtn_AlphaExternal->SetToolTip(_("Use an external image for the alpha mask (requires the mask be the same size as the source image)"));

    m_Sizer_ImageColors->Add(m_RadioBtn_AlphaExternal, 0, wxALL, 5);

    m_FilePicker_Alpha = new wxFilePickerCtrl(m_Panel_ImageColors, wxID_ANY, wxEmptyString, _("Select a file"), _("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    m_FilePicker_Alpha->Enable(false);
    m_FilePicker_Alpha->SetToolTip(_("Filepath for the external alpha mask"));

    m_Sizer_ImageColors->Add(m_FilePicker_Alpha, 0, wxALL|wxEXPAND, 5);

    this->m_Button_Palette = new wxButton(this->m_Panel_ImageColors, wxID_ANY, _("Configure Palette"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_Palette->Enable(false);
    this->m_Button_Palette->SetToolTip(_("Configure Color Index palette"));
    m_Sizer_ImageColors->Add(this->m_Button_Palette, 0, wxALL, 5);

    m_Panel_ImageColors->SetSizer(m_Sizer_ImageColors);
    m_Panel_ImageColors->Layout();
    m_Sizer_ImageColors->Fit(m_Panel_ImageColors);
    m_Notebook_Config->AddPage(m_Panel_ImageColors, _("Colors"), false);

    m_Sizer_Config->Add(m_Notebook_Config, 1, wxEXPAND | wxALL, 5);


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
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Frame_ImageBrowser::OnClose));
    this->m_Splitter_Vertical->Connect(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, wxSplitterEventHandler(Frame_ImageBrowser::m_Splitter_Vertical_DClick), NULL, this);
    this->m_Splitter_Horizontal->Connect(wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, wxSplitterEventHandler(Frame_ImageBrowser::m_Splitter_Horizontal_DClick), NULL, this);
    this->m_Panel_Edit->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(Frame_ImageBrowser::m_Panel_Edit_OnChar), NULL, this);
    this->Connect(m_Tool_Save->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Save_OnToolClicked));
    this->Connect(m_Tool_Alpha->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked));
    this->Connect(m_Tool_Tiling->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked));
    this->Connect(m_Tool_Filtering->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked));
    //this->Connect(m_Tool_PalettePreview->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked));
    this->Connect(m_Tool_Statistics->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked));
    this->Connect(m_Tool_ZoomIn->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked));
    this->Connect(m_Tool_ZoomOut->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked));
    this->Connect(m_Tool_ZoomNone->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked));
    this->Connect(m_Tool_FlashcartUpload->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked));
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

Frame_ImageBrowser::~Frame_ImageBrowser()
{
    
}

void Frame_ImageBrowser::MarkAssetModified()
{
    this->m_AssetModified = true;
    this->SetTitle(this->m_AssetFilePath.GetName() + "* - " + this->m_Title);
    if (this->m_LoadedAsset != NULL)
        this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
}

void Frame_ImageBrowser::SaveChanges()
{
    wxFile file;
    bool refresh = false;
    std::vector<uint8_t> data;
    if (!wxFileExists(this->m_AssetFilePath.GetFullPath()))
    {
        if (!wxDirExists(this->m_AssetFilePath.GetPath()))
            wxFileName::Mkdir(this->m_AssetFilePath.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        file.Create(this->m_AssetFilePath.GetFullPath());
        refresh = true;
    }
    file.Open(this->m_AssetFilePath.GetFullPath(), wxFile::write);
    if (!file.IsOpened())
    {
        wxMessageDialog dialog(this, "Unable to open file for writing", "Error serializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }
    data = this->m_LoadedAsset->Serialize();
    file.Write(data.data(), data.size());
    file.Close();
    this->SetTitle(this->m_AssetFilePath.GetName() + " - " + this->m_Title);
    this->m_AssetModified = false;

    // If the file got removed, then refresh the entire list, otherwise just the thumbnail
    if (refresh)
        this->m_Panel_Search->RefreshList();
    else
        this->m_Panel_Search->RefreshThumbnail(this->m_AssetFilePath.GetName());
}

void Frame_ImageBrowser::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && m_AssetModified)
    {
        if (wxMessageBox("Unsaved changes will be lost. Continue?", "Warning", wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Veto();
            return;
        }
    }
    event.Skip();
}

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
                wxWindow::SetCursor(wxNullCursor);
                wxSetCursor(wxNullCursor);
                this->m_UsingPipette = false;
                break;
        }
    }
    event.Skip();
}

void Frame_ImageBrowser::m_Splitter_VerticalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Vertical->SetSashPosition(0);
    this->m_Splitter_Vertical->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_VerticalOnIdle), NULL, this);
    event.Skip();
}

void Frame_ImageBrowser::m_Splitter_HorizontalOnIdle(wxIdleEvent& event)
{
    this->m_Splitter_Horizontal->SetSashPosition(this->m_Panel_Edit->GetSize().y - this->m_Panel_Config->GetBestSize().y);
    this->m_Splitter_Horizontal->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Frame_ImageBrowser::m_Splitter_HorizontalOnIdle), NULL, this);
    event.Skip();
}

void Frame_ImageBrowser::m_Splitter_Vertical_DClick(wxSplitterEvent& event)
{
    event.Veto();
}

void Frame_ImageBrowser::m_Splitter_Horizontal_DClick(wxSplitterEvent& event)
{
    event.Veto();
}

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

void Frame_ImageBrowser::m_ScrolledWin_Preview_OnMouseLeftDown(wxMouseEvent& event)
{
    if (this->m_UsingPipette)
    {
        wxColor c;
        wxWindowDC dc(this->m_ScrolledWin_Preview);   
        wxPoint p = event.GetPosition();
        dc.GetPixel(p.x, p.y, &c);
        this->m_ColourPicker_AlphaColor->SetColour(c);
        this->m_LoadedAsset->m_AlphaColor = c;
        this->MarkAssetModified();

        wxWindow::SetCursor(wxNullCursor);
        wxSetCursor(wxNullCursor);
        this->m_UsingPipette = false;
    }
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_Save_OnToolClicked(wxCommandEvent& event)
{
    this->SaveChanges();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_Alpha_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleAlphaDisplay();
    if (this->m_LoadedAsset != NULL)
        this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_Tiling_OnToolClicked(wxCommandEvent& event)
{
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_Filtering_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleFilterDisplay();
    if (this->m_LoadedAsset != NULL)
        this->m_LoadedAsset->RegenerateFinal(this->m_ScrolledWin_Preview->GetAlphaDisplay(), this->m_ScrolledWin_Preview->GetFilterDisplay(), this->m_ScrolledWin_Preview->GetZoom());
    this->m_ScrolledWin_Preview->ReloadAsset();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_PalettePreview_OnToolClicked(wxCommandEvent& event)
{
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_Statistics_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ToggleStatisticsDisplay();
    this->m_ScrolledWin_Preview->RefreshDrawing();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_ZoomIn_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomIn();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_ZoomOut_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomOut();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_ZoomNone_OnToolClicked(wxCommandEvent& event)
{
    this->m_ScrolledWin_Preview->ZoomReset();
    event.Skip();
}

void Frame_ImageBrowser::m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event)
{
    wxMessageDialog dialog(this, "This feature is not yet available.", "Whoops", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_WARNING);
    dialog.ShowModal();
    event.Skip();
}

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
    if (wxFileExists(event.GetPath()))
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

void Frame_ImageBrowser::m_RadioBtn_ResizeNone_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_NONE;
    this->m_TextCtrl_ResizeW->Enable(false);
    this->m_TextCtrl_ResizeH->Enable(false);
    this->m_Choice_Align->Enable(false);
    this->m_Choice_ResizeFill->Enable(false);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_ResizeTwo_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_POWER2;
    this->m_TextCtrl_ResizeW->Enable(false);
    this->m_TextCtrl_ResizeH->Enable(false);
    this->m_Choice_Align->Enable(true);
    this->m_Choice_ResizeFill->Enable(true);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_ResizeCustom_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ResizeMode = RESIZETYPE_CUSTOM;
    this->m_TextCtrl_ResizeW->Enable(true);
    this->m_TextCtrl_ResizeH->Enable(true);
    this->m_Choice_Align->Enable(true);
    this->m_Choice_ResizeFill->Enable(true);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_TextCtrl_ResizeW_OnText(wxCommandEvent& event)
{
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_CustomSize.x);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_TextCtrl_ResizeH_OnText(wxCommandEvent& event)
{
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_CustomSize.y);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_Align_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_Alignment = (P64Img_Alignment)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_ResizeFill_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ResizeFill = (P64Img_Fill)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_Format_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_ImageFormat = (P64Img_Format)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_TilingX_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_TilingX = (P64Img_Tiling)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_TilingY_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_TilingY = (P64Img_Tiling)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_TextCtrl_MaskPosW_OnText(wxCommandEvent& event)
{
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_MaskStart.x);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_TextCtrl_MaskPosH_OnText(wxCommandEvent& event)
{
    event.GetString().ToUInt((unsigned int*)&this->m_LoadedAsset->m_MaskStart.y);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Checkbox_Mipmaps_OnCheckBox(wxCommandEvent& event)
{
    // TODO: Check if we can generate Mipmaps to begin with (requires texture occupy <= 2048 bytes)
    this->m_LoadedAsset->m_UseMipmaps = event.IsChecked();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_Choice_Dithering_OnChoice(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_Dithering = (P64Img_DitheringMode)event.GetSelection();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_AlphaNone_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_AlphaMode = ALPHA_NONE;
    this->m_ColourPicker_AlphaColor->Enable(false);
    this->m_BitmapButton_Pipette->Enable(false);
    this->m_FilePicker_Alpha->Enable(false);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_AlphaMask_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_AlphaMode = ALPHA_MASK;
    this->m_ColourPicker_AlphaColor->Enable(false);
    this->m_BitmapButton_Pipette->Enable(false);
    this->m_FilePicker_Alpha->Enable(false);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_AlphaColor_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_AlphaMode = ALPHA_CUSTOM;
    this->m_ColourPicker_AlphaColor->Enable(true);
    this->m_BitmapButton_Pipette->Enable(true);
    this->m_FilePicker_Alpha->Enable(false);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_ColourPicker_AlphaColor_OnColourChanged(wxColourPickerEvent& event)
{
    this->m_LoadedAsset->m_AlphaColor = event.GetColour();
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_BitmapButton_Pipette_OnButtonClick(wxCommandEvent& event)
{
    wxSetCursor(Cursor_Pipette); // Works on Linux
    wxWindow::SetCursor(Cursor_Pipette); // Works on Windows
    this->m_UsingPipette = true;
    event.Skip();
}

void Frame_ImageBrowser::m_RadioBtn_AlphaExternal_OnRadioButton(wxCommandEvent& event)
{
    this->m_LoadedAsset->m_AlphaMode = ALPHA_EXTERNALMASK;
    this->m_ColourPicker_AlphaColor->Enable(false);
    this->m_BitmapButton_Pipette->Enable(false);
    this->m_FilePicker_Alpha->Enable(true);
    this->MarkAssetModified();
    event.Skip();
}

void Frame_ImageBrowser::m_FilePicker_Alpha_OnFileChanged(wxFileDirPickerEvent& event)
{
    event.Skip();
}

void Frame_ImageBrowser::m_Button_Palette_OnButtonClick(wxCommandEvent& event)
{
    wxMessageDialog dialog(this, "This feature is not yet available.", "Whoops", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_WARNING);
    dialog.ShowModal();
    event.Skip();
}