#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bmpbuttn.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/dir.h>

class Panel_Search : public wxPanel
{
    private:
        wxFileName m_CurrFolder;
        wxString m_AssetExt;
        wxString m_AssetType;
        wxFrame* m_Target;
        wxIcon (*m_IconGenFunc)(bool, wxFileName);
        void (*m_NewAssetFunc)(wxFrame*, wxFileName);
        void (*m_LoadAssetFunc)(wxFrame*, wxFileName);
        void (*m_RenameAssetFunc)(wxFrame*, wxFileName, wxFileName);

    protected:
        wxFileName m_MainFolder;
        wxBitmapButton* m_Button_Back;
        wxBitmapButton* m_Button_NewAsset;
        wxBitmapButton* m_Button_NewFolder;
        wxBitmapToggleButton* m_ToggleButton_Search;
        wxBitmapButton* m_Button_ViewMode;
        wxTextCtrl* m_TextCtrl_Search;
        wxDataViewListCtrl* m_DataViewListCtrl_ObjectList;
        wxDataViewColumn* m_DataViewListColumn_Assets;
        wxDataViewListCtrl* m_DataViewListCtrl_ObjectGrid;

        void m_Button_Back_OnButtonClick(wxCommandEvent& event);
        void m_Button_NewAsset_OnButtonClick(wxCommandEvent& event);
        void m_Button_NewFolder_OnButtonClick(wxCommandEvent& event);
        void m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event);
        void m_Button_ViewMode_OnButtonClick(wxCommandEvent& event);
        void m_TextCtrl_Search_OnText(wxCommandEvent& event);
        void m_DataViewListCtrl_ObjectList_OnItemActivated(wxDataViewEvent& event);
        void m_DataViewListCtrl_ObjectList_ItemEditingDone(wxDataViewEvent& event);
        void m_DataViewListCtrl_ObjectList_ContextMenu(wxDataViewEvent& event);

    public:
        Panel_Search(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);
        ~Panel_Search();
        void Search_SetFolder(wxFileName path);
        void Search_SetAssetType(wxString name, wxString type);
        void Search_IconGenerator(wxIcon (*function)(bool, wxFileName));
        void Search_NewAssetGenerator(void (*function)(wxFrame*, wxFileName));
        void Search_LoadAssetFunc(void (*function)(wxFrame*, wxFileName));
        void Search_RenameAssetFunc(void (*function)(wxFrame*, wxFileName, wxFileName));
        void Search_SetTarget(wxFrame* target);
        bool LoadAssetsInDir(wxFileName path, wxString filter=wxEmptyString);
        void RefreshList();
        void RefreshThumbnail(wxString assetname);
        void SelectItem(wxString name, bool isfolder, bool editname=false);

        wxFileName GetMainFolder();
};
