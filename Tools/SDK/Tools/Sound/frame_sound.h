#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/filename.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include "../panel_search.h"
#include "asset_sound.h"
#include "panel_sndview.h"


/*=============================================================
                            Classes
=============================================================*/

class Frame_SoundBrowser : public wxFrame
{
    private:
        wxString m_Title;
        wxFileName m_AssetFilePath;
        P64Asset_Sound* m_LoadedAsset;
        bool m_AssetModified;

        wxSplitterWindow* m_Splitter_Vertical;
        Panel_Search* m_Panel_Search;
        wxPanel* m_Panel_Edit;
        wxSplitterWindow* m_Splitter_Horizontal;
        wxPanel* m_Panel_Preview;
		wxToolBar* m_ToolBar_Preview;
		wxToolBarToolBase* m_Tool_Save;
		wxToolBarToolBase* m_Tool_FlashcartUpload;
        Panel_SndView* m_ScrolledWin_Preview;
        wxPanel* m_Panel_Config;
        wxNotebook* m_Notebook_Config;
        wxPanel* m_Panel_Basic;
        wxPanel* m_Panel_Advanced;
        wxFilePickerCtrl* m_FilePicker_Source;
        wxChoice* m_Choice_SampleRate;
        wxCheckBox* m_CheckBox_Mono;
        wxCheckBox* m_CheckBox_Loop;
        wxSpinCtrl* m_SpinCtrl_LoopStart;
        wxSpinCtrl* m_SpinCtrl_LoopEnd;
        wxSpinCtrl* m_SpinCtrl_CodebookEntryCount;
        wxSpinCtrl* m_SpinCtrl_CodebookSampleSize;
        wxSpinCtrl* m_SpinCtrl_ClusterIterations;

        void OnClose(wxCloseEvent& event);
        void m_Panel_Edit_OnChar(wxKeyEvent& event);
        void m_Tool_Save_OnToolClicked(wxCommandEvent& event);
        void m_Tool_FlashcartUpload_OnToolClicked(wxCommandEvent& event);
        void m_Splitter_VerticalOnIdle(wxIdleEvent&);
        void m_Splitter_HorizontalOnIdle(wxIdleEvent&);
        void m_FilePicker_Source_OnFileChanged(wxFileDirPickerEvent& event);
        void m_Choice_SampleRate_OnChoice(wxCommandEvent& event);
        void m_CheckBox_Mono_OnCheckBox(wxCommandEvent& event);
        void m_CheckBox_Loop_OnCheckBox(wxCommandEvent& event);
        void m_SpinCtrl_LoopStart_OnSpinCtrl(wxSpinEvent& event);
        void m_SpinCtrl_LoopEnd_OnSpinCtrl(wxSpinEvent& event);

        void MarkAssetModified();
        void SaveChanges();

    protected:

	public:
        void UpdateTitle();
        void UpdateFilePath(wxFileName path);
        P64Asset_Sound* LoadAsset(wxFileName path);

        bool IsAssetModified();
        wxFileName GetLoadedAssetPath();

		Frame_SoundBrowser(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 480), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
		~Frame_SoundBrowser();
};