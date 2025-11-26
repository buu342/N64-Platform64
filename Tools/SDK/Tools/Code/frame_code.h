#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/stc/stc.h>
#include <wx/sizer.h>
#include <wx/aui/auibook.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/aui/aui.h>

/*=============================================================
                            Classes
=============================================================*/

class Frame_CodeEditor : public wxFrame
{
    private:

    protected:
        wxToolBar* m_ToolBar;
        wxToolBarToolBase* m_Tool_Save;
        wxToolBarToolBase* m_Tool_Cut;
        wxToolBarToolBase* m_Tool_Copy;
        wxToolBarToolBase* m_Tool_Paste;
        wxToolBarToolBase* m_Tool_Compile;
        wxToolBarToolBase* m_Tool_CompileDebug;
        wxToolBarToolBase* m_Tool_Clean;
        wxToolBarToolBase* m_Tool_UploadUSB;
        wxMenuBar* m_Menubar;
        wxPanel* m_Panel_Search;
        wxAuiNotebook* m_Notebook_Editor;
        wxPanel* m_Panel_Page;
        wxStyledTextCtrl* m_Editor;
        wxAuiNotebook* m_AUINotebook_Output;
        wxPanel* m_Panel_Compiler;
        wxRichTextCtrl* m_RichText_Compiler;
        wxPanel* m_Panel_Log;
        wxRichTextCtrl* m_RichText_Log;
        wxPanel* m_Panel_Flashcart;
        wxRichTextCtrl* m_RichText_Flashcart;
        wxTextCtrl* m_TextCtrl_Flashcart;
        wxButton* m_Button_Flashcart;
        wxStatusBar* m_StatusBar;

    public:
        wxAuiManager m_mgr;
        Frame_CodeEditor(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 480), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~Frame_CodeEditor();
};