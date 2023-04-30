#include "traverser.h"
#include <wx/artprov.h>

Traverser::Traverser(wxString path, wxTreeCtrl* tree, wxTreeItemId root, std::map<wxTreeItemId, CompUnit*>* map)
{
    this->m_CurDir = path;
    this->m_ProjectTree = tree;
    this->m_CurNode = root;
    this->m_Map = map;
}

Traverser::~Traverser()
{

}

bool IsWhitelistedExtension(wxString ext)
{
    return (ext == "c" || ext == "h");
}

wxDirTraverseResult Traverser::OnFile(const wxString& filename)
{
    wxFileName name(filename);
    if (IsWhitelistedExtension(name.GetExt()))
    {
        if (name.GetExt() == "c")
        {
            wxTreeItemId id = this->m_ProjectTree->AppendItem(this->m_CurNode, name.GetFullName(), 1);
            this->m_Map->insert(std::pair<wxTreeItemId, CompUnit* >(id, new CompUnit(this->m_ProjectTree, id)));
        }
        else
            this->m_ProjectTree->AppendItem(this->m_CurNode, name.GetFullName(), 2);
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult Traverser::OnDir(const wxString& dirname)
{
	wxDir dir(dirname);
    wxTreeItemId oldnode = this->m_CurNode;

    // Check for errors
	if (!dir.IsOpened())
		return wxDIR_IGNORE;
    this->m_CurNode = this->m_ProjectTree->AppendItem(this->m_CurNode, wxFileName(dirname).GetName(), 0);
	
    // Traverse recursively
    Traverser traverser(dirname, this->m_ProjectTree, this->m_CurNode, this->m_Map);
	dir.Traverse(traverser);
    dir.Close();

    // Remove the node if it's empty
    if (this->m_ProjectTree->GetChildrenCount(this->m_CurNode) == 0)
    {
        this->m_ProjectTree->Delete(this->m_CurNode);
        this->m_CurNode = oldnode;
    }
    else
        this->m_CurNode = this->m_ProjectTree->GetItemParent(this->m_CurNode);

    // Done
    return wxDIR_IGNORE;
}