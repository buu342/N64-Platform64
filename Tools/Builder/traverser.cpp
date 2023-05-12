#include "traverser.h"
#include "preferences.h"
#include "helper.h"
#include <wx/artprov.h>
#include <iterator>

Traverser::Traverser(wxString path, wxTreeListCtrl* tree, wxTreeListItem root, std::map<wxTreeListItem, CompUnit*>* map, std::vector<wxString> extensions)
{
    this->m_CurDir = path;
    this->m_Tree = tree;
    this->m_CurNode = root;
    this->m_Map = map;
    this->m_Extensions = extensions;
}

Traverser::~Traverser()
{

}

bool Traverser::IsWhitelistedExtension(wxString ext)
{
    for (std::vector<wxString>::iterator it = this->m_Extensions.begin(); it != this->m_Extensions.end(); ++it)
        if (ext == (*it))
            return true;
    return false;
}

wxDirTraverseResult Traverser::OnFile(const wxString& filename)
{
    wxFileName name(filename);
    if (this->IsWhitelistedExtension(name.GetExt()))
    {
        if (name.GetExt() == "c")
        {
            wxTreeListItem id = this->m_Tree->AppendItem(this->m_CurNode, name.GetFullName(), 1);
            if (this->m_Map != NULL)
            {
                CompUnit* unit = new CompUnit(this->m_Tree, id);
                this->m_Map->insert(std::pair<wxTreeListItem, CompUnit* >(id, unit));
                this->m_Tree->SetItemText(id, 1, unit->GetSegment().GetName());
            }
        }
        else
            this->m_Tree->AppendItem(this->m_CurNode, name.GetFullName(), 2);
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult Traverser::OnDir(const wxString& dirname)
{
	wxDir dir(dirname);
    wxTreeListItem oldnode = this->m_CurNode;

    // Check for errors
	if (!dir.IsOpened())
		return wxDIR_IGNORE;
    this->m_CurNode = this->m_Tree->AppendItem(this->m_CurNode, wxFileName(dirname).GetName(), 0);
	
    // Traverse recursively
    Traverser traverser(dirname, this->m_Tree, this->m_CurNode, this->m_Map, this->m_Extensions);
	dir.Traverse(traverser);
    dir.Close();

    // Remove the node if it's empty
    if (!this->m_Tree->GetFirstChild(this->m_CurNode).IsOk())
    {
        this->m_Tree->DeleteItem(this->m_CurNode);
        this->m_CurNode = oldnode;
    }
    else
        this->m_CurNode = this->m_Tree->GetItemParent(this->m_CurNode);

    // Done
    return wxDIR_IGNORE;
}

void Traverser::DeleteFiles(wxTreeListItem node)
{
    wxString name = this->m_Tree->GetItemText(node);
    wxTreeListItem child = this->m_Tree->GetFirstChild(node);

    // Recursively find the child node
    while (child.IsOk())
    {
        this->DeleteFiles(child);
        child = this->m_Tree->GetNextItem(child);
    }

    // Do the delete
    child = node;
    if (this->IsWhitelistedExtension(((wxFileName)name).GetExt()))
    {
        while (this->m_Tree->GetItemParent(child) != NULL)
        {
            child = this->m_Tree->GetItemParent(child);
            name.Prepend(this->m_Tree->GetItemText(child) + "/");
        }
        wxRemoveFile(((wxFileName)name).GetFullPath());
    }
}