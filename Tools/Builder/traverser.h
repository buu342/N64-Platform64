#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
#include <wx/dir.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <map>
#include <vector>

class Traverser : public wxDirTraverser
{
    protected:
        wxTreeCtrl*  m_Tree;
        wxTreeItemId m_CurNode;
        wxString     m_CurDir;
        std::vector<wxString> m_Extensions;
        std::map<wxTreeItemId, CompUnit*>* m_Map;

    public:
        wxDirTraverseResult OnFile(const wxString& filename);
        wxDirTraverseResult OnDir(const wxString& dirname);
        bool IsWhitelistedExtension(wxString ext);
        void DeleteFiles(wxTreeItemId node);
        Traverser(wxString path, wxTreeCtrl* tree, wxTreeItemId root, std::map<wxTreeItemId, CompUnit*>* map, std::vector<wxString> extensions);
        ~Traverser();
};