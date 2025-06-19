#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/clrpicker.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/frame.h>
#include "../panel_search.h"
#include "panel_imgview.h"

class Frame_ImageBrowser : public wxFrame
{
	private:

	protected:
        wxSplitterWindow* m_Splitter_Vertical;
        Panel_Search* m_Panel_Search;
        wxPanel* m_Panel_Edit;
        wxSplitterWindow* m_Splitter_Horizontal;
        wxPanel* m_Panel_Preview;
        wxToolBar* m_ToolBar_Preview;
        wxToolBarToolBase* m_Tool_Alpha;
        wxToolBarToolBase* m_Tool_Tiling;
        wxToolBarToolBase* m_Tool_Filtering;
        wxToolBarToolBase* m_Tool_PalettePreview;
        wxToolBarToolBase* m_Tool_Statistics;
        wxToolBarToolBase* m_Tool_ZoomIn;
        wxToolBarToolBase* m_Tool_ZoomOut;
        wxToolBarToolBase* m_Tool_ZoomNone;
        wxToolBarToolBase* m_Tool_FlashcartUpload;
        wxScrolledWindow* m_ScrolledWin_Preview;
        wxPanel* m_Panel_Config;
        wxNotebook* m_notebook1;
        wxPanel* m_Panel_ImageData;
        wxStaticText* m_StaticText_Image;
        wxFilePickerCtrl* m_FilePicker_Image;
        wxStaticText* m_StaticText_Resize;
        wxRadioButton* m_RadioBtn_ResizeNone;
        wxRadioButton* m_RadioBtn_ResizeTwo;
        wxRadioButton* m_RadioBtn_ResizeCustom;
        wxTextCtrl* m_TextCtrl_ResizeW;
        wxTextCtrl* m_TextCtrl_ResizeH;
        wxStaticText* m_StaticText_Align;
        wxChoice* m_Choice_Align;
        wxStaticText* m_StaticText_ResizeFill;
        wxChoice* m_Choice_ResizeFill;
        wxPanel* m_Panel_ImageLoading;
        wxStaticText* m_StaticText_Format;
        wxChoice* m_Choice_Format;
        wxStaticText* m_StaticText_TilingMode;
        wxChoice* m_Choice_TilingX;
        wxChoice* m_Choice_TilingY;
        wxStaticText* m_StaticText_MaskPos;
        wxTextCtrl* m_TextCtrl_MaskPosW;
        wxTextCtrl* m_TextCtrl_MaskPosH;
        wxCheckBox* m_Checkbox_Mipmaps;
        wxPanel* m_Panel_ImageColors;
        wxStaticText* m_StaticText_Quantization;
        wxChoice* m_Choice_Quantization;
        wxStaticText* m_StaticText_AlphaChoice;
        wxRadioButton* m_RadioBtn_AlphaNone;
        wxRadioButton* m_RadioBtn_AlphaMask;
        wxRadioButton* m_RadioBtn_AlphaColor;
        wxColourPickerCtrl* m_ColourPicker_AlphaColor;
        wxBitmapButton* m_BitmapButton_Pipette;
        wxRadioButton* m_RadioBtn_AlphaExternal;
        wxFilePickerCtrl* m_FilePicker_Alpha;
        wxButton* m_Button_Palette;

        // Virtual event handlers, override them in your derived class
        void m_Tool_Alpha_OnToolClicked(wxCommandEvent& event);
        void m_Tool_Tiling_OnToolClicked(wxCommandEvent& event);
        void m_Tool_Filtering_OnToolClicked(wxCommandEvent& event);
        void m_Tool_PalettePreview_OnToolClicked(wxCommandEvent& event);
        void m_Tool_Statistics_OnToolClicked(wxCommandEvent& event);
        void m_Tool_ZoomIn_OnToolClicked(wxCommandEvent& event);
        void m_Tool_ZoomOut_OnToolClicked(wxCommandEvent& event);
        void m_Tool_ZoomNone_OnToolClicked(wxCommandEvent& event);
        void m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event);
        void m_FilePicker_Image_OnFileChanged(wxFileDirPickerEvent& event);
        void m_RadioBtn_ResizeNone_OnRadioButton(wxCommandEvent& event);
        void m_RadioBtn_ResizeTwo_OnRadioButton(wxCommandEvent& event);
        void m_RadioBtn_ResizeCustom_OnRadioButton(wxCommandEvent& event);
        void m_TextCtrl_ResizeW_OnText(wxCommandEvent& event);
        void m_TextCtrl_ResizeH_OnText(wxCommandEvent& event);
        void m_Choice_Align_OnChoice(wxCommandEvent& event);
        void m_Choice_ResizeFill_OnChoice(wxCommandEvent& event);
        void m_Choice_Format_OnChoice(wxCommandEvent& event);
        void m_Choice_TilingX_OnChoice(wxCommandEvent& event);
        void m_Choice_TilingY_OnChoice(wxCommandEvent& event);
        void m_TextCtrl_MaskPosW_OnText(wxCommandEvent& event);
        void m_TextCtrl_MaskPosH_OnText(wxCommandEvent& event);
        void m_Checkbox_Mipmaps_OnCheckBox(wxCommandEvent& event);
        void m_Choice_Quantization_OnChoice(wxCommandEvent& event);
        void m_RadioBtn_AlphaNone_OnRadioButton(wxCommandEvent& event);
        void m_RadioBtn_AlphaMask_OnRadioButton(wxCommandEvent& event);
        void m_RadioBtn_AlphaColor_OnRadioButton(wxCommandEvent& event);
        void m_ColourPicker_AlphaColor_OnColourChanged(wxColourPickerEvent& event);
        void m_BitmapButton_Pipette_OnButtonClick(wxCommandEvent& event);
        void m_RadioBtn_AlphaExternal_OnRadioButton(wxCommandEvent& event);
        void m_FilePicker_Alpha_OnFileChanged(wxFileDirPickerEvent& event);
        void m_Button_Palette_OnButtonClick(wxCommandEvent& event);

	public:
		Frame_ImageBrowser( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~Frame_ImageBrowser();

		void m_Splitter_VerticalOnIdle( wxIdleEvent& )
		{
			m_Splitter_Vertical->SetSashPosition( 0 );
			m_Splitter_Vertical->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_VerticalOnIdle ), NULL, this );
		}

		void m_Splitter_HorizontalOnIdle( wxIdleEvent& )
		{
			m_Splitter_Horizontal->SetSashPosition( 0 );
			m_Splitter_Horizontal->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Frame_ImageBrowser::m_Splitter_HorizontalOnIdle ), NULL, this );
		}

};

