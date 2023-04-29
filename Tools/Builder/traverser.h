#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
#include <wx/dir.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <map>

class Traverser : public wxDirTraverser
{
    private:
        wxTreeCtrl*  m_ProjectTree;
        wxTreeItemId m_CurNode;
        wxString     m_CurDir;
        std::map<wxTreeItemId, CompUnit*>* m_Map;

    public:
        wxDirTraverseResult OnFile(const wxString& filename);
        wxDirTraverseResult OnDir(const wxString& dirname);
        Traverser(wxString path, wxTreeCtrl* tree, wxTreeItemId root, std::map<wxTreeItemId, CompUnit*>* map);
        ~Traverser();
};