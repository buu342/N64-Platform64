#pragma once

typedef struct IUnknown IUnknown;

#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <vector>

class CompUnit
{
	private:
		wxFileName              m_File;
		std::vector<wxFileName> m_Dependencies;
		wxFileName              m_Output;

	public:
		wxFileName GetOutputPath(bool debug);
		bool ShouldRebuild(bool debug);
		void Compile(bool debug);
		CompUnit(wxTreeCtrl* tree, wxTreeItemId id);
		~CompUnit();
};

