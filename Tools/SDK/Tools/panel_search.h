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
#include <wx/msgqueue.h>
#include <list>
#include <unordered_map>


/******************************
             Types
******************************/

typedef struct {
    wxFileName file;
    bool large;
} ThreadWork;

typedef struct {
    wxFileName file;
    wxIcon icon;
} ThreadResult;


/******************************
            Classes
******************************/

// Prototypes
class IconGeneratorThread;
class Panel_AssetDisplay;
class Panel_AssetDisplay_List;
class Panel_AssetDisplay_Grid; 

// Search panel
class Panel_Search : public wxPanel
{
    private:
        wxFileName m_MainFolder;
        
        wxBitmapButton* m_Button_NewAsset;
        wxBitmapButton* m_Button_NewFolder;
        wxBitmapToggleButton* m_ToggleButton_Search;
        wxBitmapButton* m_Button_ViewMode;
        Panel_AssetDisplay_List* m_Display_List;
        Panel_AssetDisplay_Grid* m_Display_Grid;
        Panel_AssetDisplay* m_Display_Current;

        void m_Button_Back_OnButtonClick(wxCommandEvent& event);
        void m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event);
        void m_Button_ViewMode_OnButtonClick(wxCommandEvent& event);
        void m_TextCtrl_Search_OnText(wxCommandEvent& event);

    protected:

    public:
        wxBitmapButton* m_Button_Back;
        wxFileName m_CurrFolder;
        wxString m_AssetType;
        wxString m_AssetExt;
        wxString m_AssetExt_NoAsterisk;

        void (*m_NewAssetFunc)(wxFrame*, wxFileName);
        void (*m_LoadAssetFunc)(wxFrame*, wxFileName);
        void (*m_RenameAssetFunc)(wxFrame*, wxFileName, wxFileName);
        wxTextCtrl* m_TextCtrl_Search;
        wxFrame* m_TargetFrame;

        void m_Button_NewAsset_OnButtonClick(wxCommandEvent& event);
        void m_Button_NewFolder_OnButtonClick(wxCommandEvent& event);

        void SetMainFolder(wxFileName path);
        void SetAssetType(wxString name, wxString type);
        void SetAssetGenerator(void (*function)(wxFrame*, wxFileName));
        void SetIconGenerator(wxIcon (*function)(bool, wxFileName));
        void SetLoadAssetFunc(void (*function)(wxFrame*, wxFileName));
        void SetRenameAssetFunc(void (*function)(wxFrame*, wxFileName, wxFileName));
        void SetTargetFrame(wxFrame* target);
        wxFileName GetMainFolder();

        Panel_Search(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);
        ~Panel_Search();
};

class Panel_AssetDisplay : public wxPanel
{
    private:

    protected:
        std::list<wxString> m_Folders;
        std::list<wxString> m_Assets;

        IconGeneratorThread* m_IconGeneratorThread;

        std::list<wxIcon> m_IconCache_LRU;
        std::unordered_map<wxString, std::list<wxIcon>::iterator> m_IconCache_Map;

        void ThreadEvent(wxThreadEvent& event);
        void StartThread_IconGenerator();
        void StopThread_IconGenerator();

    public:
        wxIcon (*m_IconGenFunc)(bool, wxFileName);
        wxMessageQueue<ThreadWork*> m_ThreadQueue;

        bool virtual LoadDirectory(wxFileName path, wxString filter = wxEmptyString);

        Panel_AssetDisplay(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_AssetDisplay();
};

class Panel_AssetDisplay_List : public Panel_AssetDisplay
{
    private:
        wxDataViewListCtrl* m_DataViewListCtrl_ObjectList;
        wxDataViewColumn* m_DataViewListColumn_Assets;
        wxDataViewColumn* m_DataViewListColumn_IsFolder;

        void m_DataViewListCtrl_ObjectList_OnItemActivated(wxDataViewEvent& event);
        void m_DataViewListCtrl_ObjectList_ContextMenu(wxDataViewEvent& event);
        void m_DataViewListCtrl_ObjectList_ItemEditingDone(wxDataViewEvent& event);
        void ThreadEvent(wxThreadEvent& event);

    protected:

    public:
        bool LoadDirectory(wxFileName path, wxString filter = wxEmptyString);
        
        Panel_AssetDisplay_List(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_AssetDisplay_List();
};

class Panel_AssetDisplay_Grid : public Panel_AssetDisplay
{
    private:

    protected:
        void ThreadEvent(wxThreadEvent& event);

    public:
        bool LoadDirectory(wxFileName path, wxString filter = wxEmptyString);

        Panel_AssetDisplay_Grid(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_AssetDisplay_Grid();
};

// Icon generator thread
class IconGeneratorThread : public wxThread
{
    private:
        wxIcon(*m_IconGenFunc)(bool, wxFileName);
        Panel_AssetDisplay* m_Parent;

    protected:

    public:
        IconGeneratorThread(Panel_AssetDisplay* parent, wxIcon(*func)(bool, wxFileName));
        ~IconGeneratorThread();

        virtual void* Entry() wxOVERRIDE;
};