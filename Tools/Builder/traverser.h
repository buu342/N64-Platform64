#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
#include <wx/dir.h>
#include <wx/treelist.h>
#include <wx/filename.h>
#include <map>
#include <vector>

class Traverser : public wxDirTraverser
{
    protected:
        wxTreeListCtrl*  m_Tree;
        wxTreeListItem   m_CurNode;
        wxString         m_CurDir;
        std::vector<wxString> m_Extensions;
        std::map<wxTreeListItem, CompUnit*>* m_Map;

    public:
        wxDirTraverseResult OnFile(const wxString& filename);
        wxDirTraverseResult OnDir(const wxString& dirname);
        bool IsWhitelistedExtension(wxString ext);
        void DeleteFiles(wxTreeListItem node);
        Traverser(wxString path, wxTreeListCtrl* tree, wxTreeListItem root, std::map<wxTreeListItem, CompUnit*>* map, std::vector<wxString> extensions);
        ~Traverser();
};