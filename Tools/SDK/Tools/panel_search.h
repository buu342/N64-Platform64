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
#include <wx/wrapsizer.h>
#include <list>
#include <unordered_map>


/******************************
             Types
******************************/

typedef struct {
    wxFileName file;
    bool large;
    bool isnew;
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
class Panel_AssetDisplay_Grid_Item;

// Search panel
class Panel_Search : public wxPanel
{
    private:
        wxFileName m_MainFolder;
        wxFileName m_CurrFolder;
        wxFrame* m_TargetFrame;
        wxString m_AssetType;
        wxString m_AssetExt;
        wxString m_AssetExt_NoAsterisk;
        void (*m_NewAssetFunc)(wxFrame*, wxFileName);
        void (*m_LoadAssetFunc)(wxFrame*, wxFileName);
        void (*m_RenameAssetFunc)(wxFrame*, wxFileName, wxFileName);
        
        wxBitmapButton* m_Button_Back;
        wxBitmapButton* m_Button_NewAsset;
        wxBitmapButton* m_Button_NewFolder;
        wxBitmapToggleButton* m_ToggleButton_Search;
        wxBitmapButton* m_Button_ViewMode;
        wxTextCtrl* m_TextCtrl_Search;
        Panel_AssetDisplay_List* m_Display_List;
        Panel_AssetDisplay_Grid* m_Display_Grid;
        Panel_AssetDisplay* m_Display_Current;

        void m_Button_Back_OnButtonClick(wxCommandEvent& event);
        void m_Button_NewAsset_OnButtonClick(wxCommandEvent& event);
        void m_Button_NewFolder_OnButtonClick(wxCommandEvent& event);
        void m_ToggleButton_Search_OnToggleButton(wxCommandEvent& event);
        void m_Button_ViewMode_OnButtonClick(wxCommandEvent& event);
        void m_TextCtrl_Search_OnText(wxCommandEvent& event);

    protected:

    public:
        void SetMainFolder(wxFileName path);
        void SetAssetType(wxString name, wxString type);
        void SetAssetGenerator(void (*function)(wxFrame*, wxFileName));
        void SetIconGenerator(wxIcon (*function)(bool, wxFileName));
        void SetLoadAssetFunc(void (*function)(wxFrame*, wxFileName));
        void SetRenameAssetFunc(void (*function)(wxFrame*, wxFileName, wxFileName));
        void SetTargetFrame(wxFrame* target);
        void SetCurrentFolder(wxFileName path);
        void ClearSearchbox();

        wxFileName GetMainFolder();
        wxFileName GetCurrentFolder();
        wxString GetAssetType();
        wxString GetAssetExtension(bool asterisk=false);
        wxFrame* GetTargetFrame();

        void CreateNewAsset();
        void CreateNewFolder();
        void LoadAsset(wxFileName path);
        void ReloadThumbnail(wxFileName path);
        void RenameAsset(wxFileName oldpath, wxFileName newpath);

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
        wxMessageQueue<ThreadWork*> m_ThreadQueue;

        wxIcon (*m_IconGenFunc)(bool, wxFileName);

        void ThreadEvent(wxThreadEvent& event);
        void StartThread_IconGenerator();
        void StopThread_IconGenerator();

    public:
        std::list<std::tuple<wxString, wxIcon>> m_IconCache_LRU;
        std::unordered_map<wxString, std::list<std::tuple<wxString, wxIcon>>::iterator> m_IconCache_Map;
        
        virtual bool LoadDirectory(wxFileName path, wxString filter = wxEmptyString);
        virtual void  SelectItem(wxString itemname, bool isfolder, bool rename=false);
        
        void SetIconGenerator(wxIcon (*function)(bool, wxFileName));
        wxMessageQueue<ThreadWork*>* GetThreadQueue();

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
        void SelectItem(wxString itemname, bool isfolder, bool rename=false);
        
        Panel_AssetDisplay_List(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_AssetDisplay_List();
};

class Panel_AssetDisplay_Grid : public Panel_AssetDisplay
{
    private:
        wxScrolledWindow* m_Panel_Icons = NULL;
        wxWrapSizer* m_Sizer_Icons;
        Panel_AssetDisplay_Grid_Item* m_Selection;
        
        void CreateIconPanel(wxFileName filepath, bool isfolder);

        void m_Panel_Icons_OnSize(wxSizeEvent& event);
        void m_Panel_Icons_OnLeftDown(wxMouseEvent& event);
        void ThreadEvent(wxThreadEvent& event);

    protected:

    public:
        bool LoadDirectory(wxFileName path, wxString filter = wxEmptyString);
        void SelectItem(wxString itemname, bool isfolder, bool rename=false);
        void HighlightItem(Panel_AssetDisplay_Grid_Item* item);

        int GetItemCount();
        Panel_AssetDisplay_Grid_Item* GetItemAtPos(int pos);

        Panel_AssetDisplay_Grid(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
        ~Panel_AssetDisplay_Grid();
};

class Panel_AssetDisplay_Grid_Item : public wxPanel
{
    private:
        wxStaticBitmap* m_Bitmap_Icon;
        wxStaticText* m_StaticText_Name;
        wxTextCtrl* m_TextCtrl_NameEdit;
        bool m_IsFolder;

        void m_OnLeftDClick(wxMouseEvent& event);
        void m_OnLeftDown(wxMouseEvent& event);
        void m_Bitmap_Icon_OnLeftDClick(wxMouseEvent& event);
        void m_Bitmap_Icon_OnLeftDown(wxMouseEvent& event);
        void m_Icon_Name_OnLeftDown(wxMouseEvent& event);
        void m_Icon_TextCtrl_OnTextEnter(wxCommandEvent& event);

    protected:

    public:
        void SetFile(wxFileName filepath, bool isfolder);
        void SetIcon(wxIcon icon);

        wxString GetFileName();
        bool IsFolder();

        Panel_AssetDisplay_Grid_Item(wxWindow* parent);
        ~Panel_AssetDisplay_Grid_Item();
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