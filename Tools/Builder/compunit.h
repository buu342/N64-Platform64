#pragma once

typedef struct IUnknown IUnknown;

#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/treelist.h>
#include <wx/filename.h>
#include <vector>

class CompUnit
{
	private:
		wxFileName              m_File;
		std::vector<wxFileName> m_Dependencies;
		wxFileName              m_Output;
		wxFileName              m_Segment;

	public:
		wxFileName GetFilePath();
		wxFileName GetOutputPath(bool debug);
		wxFileName GetSegment();
		void SetSegment(wxString segment);
		bool ShouldRebuild(bool debug);
		void Compile(bool debug);
		CompUnit(wxTreeListCtrl* tree, wxTreeListItem id);
		~CompUnit();
};

