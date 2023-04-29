#pragma once

typedef struct IUnknown IUnknown;

#include <wx/dir.h>
#include <wx/treectrl.h>
#include <wx/filename.h>

class Traverser : public wxDirTraverser
{
    private:
        wxTreeCtrl* m_ProjectTree;
        wxTreeItemId m_CurNode;
        wxString m_CurDir;

    public:
        wxDirTraverseResult OnFile(const wxString& filename);
        wxDirTraverseResult OnDir(const wxString& dirname);
        Traverser(wxString path, wxTreeCtrl* tree, wxTreeItemId root);
        ~Traverser();
};