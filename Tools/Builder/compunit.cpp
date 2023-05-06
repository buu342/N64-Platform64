#include "compunit.h"
#include "main.h"
#include "helper.h"
#include "preferences.h"
#include <wx/utils.h> 
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/dir.h>

CompUnit::CompUnit(wxTreeCtrl* tree, wxTreeItemId id)
{
	wxTextFile file;
	wxString path = "";
	wxString output = tree->GetItemText(id);
	wxTreeItemId curid = id;

	// First, generate the path
	while (tree->GetItemParent(curid) != NULL)
	{
		curid = tree->GetItemParent(curid);
		path.Prepend(tree->GetItemText(curid)+"/");
	}

	// Now set the file paths
	output.Replace(".c", ".o");
	this->m_File.Assign(global_projectconfig.ProjectPath + "/" + path + tree->GetItemText(id));
	if (global_programconfig.Use_Build)
		this->m_Output.Assign(global_projectconfig.BuildFolder + "/" + path + output);
	else
		this->m_Output.Assign(global_projectconfig.ProjectPath + "/" + path + output);

	// Finally, we need to read the file and check its dependencies
	file.Open(this->m_File.GetFullPath());
	while (!file.Eof())
	{
		wxString str = file.GetNextLine();
		if (str.Contains("#include"))
		{
			wxString token;
			wxStringTokenizer tokenizer(str, "\"");
			tokenizer.GetNextToken();
			token = tokenizer.GetNextToken();
			if (token != "")
			{
				wxFileName finalpath(token);
				finalpath.Normalize(wxPATH_NORM_ALL, this->m_File.GetPath());
				this->m_Dependencies.push_back(finalpath);
			}
		}
	}
	file.Close();
}

CompUnit::~CompUnit()
{

}

wxFileName CompUnit::GetOutputPath(bool debug)
{
	if (debug && global_programconfig.SeparateDebug)
		return this->m_Output.GetPath() + "/" + this->m_Output.GetName() + "_d." + this->m_Output.GetExt();
	else
		return this->m_Output;
}

bool CompUnit::ShouldRebuild(bool debug)
{
	wxStructStat stat_input;
	wxStructStat stat_output;
	wxFileName outputpath = this->GetOutputPath(debug);
	stat_input.st_mtime = LastModTime(this->m_File.GetFullPath());
	stat_output.st_mtime = LastModTime(outputpath.GetFullPath());

	// Check if the object file exists
	if (!wxFileExists(outputpath.GetFullPath()))
	{
		wxLogVerbose(outputpath.GetFullName() + " does not exist");
		return true;
	}

	// Check if the modification date of the C file superscedes the object file
	if (wxDateTime(stat_input.st_mtime).IsLaterThan(wxDateTime(stat_output.st_mtime)))
	{
		wxLogVerbose(this->m_File.GetFullName() + " was modified");
		return true;
	}

	// Check if header files superscene the object file
	for (std::vector<wxFileName>::iterator it = this->m_Dependencies.begin(); it != this->m_Dependencies.end(); ++it)
	{
		stat_input.st_mtime = LastModTime((*it).GetFullPath());
		if (wxDateTime(stat_input.st_mtime).IsLaterThan(wxDateTime(stat_output.st_mtime)))
		{
			wxLogVerbose((*it).GetFullName() + " was modified");
			return true;
		}
	}
	return false;
}

void CompUnit::Compile(bool debug)
{
	wxString command;
	wxArrayString output;
	wxString optimizer = "-g";
	wxString lcdefs = "-DDEBUG";
	wxString exew32 = global_programconfig.Use_EXEW32 ? "exew32.exe " : "";

	// Set optimizer flags
	if (!debug)
	{
		optimizer = "-O2";
		lcdefs = "-D_FINALROM -DNDEBUG";
	}

	// Run GCC
	wxDir::Make(this->GetOutputPath(debug).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	command = global_programconfig.Path_Toolkit + "/" + exew32 + "gcc.exe "
			+ lcdefs + " "
			+ global_projectconfig.Flags_GCC + " "
			+ optimizer
			+ " -c " + this->m_File.GetFullPath()
			+ " -o " + this->GetOutputPath(debug).GetFullPath();
	wxLogVerbose("> " + command);
	wxExecute(command, output, wxEXEC_SYNC,  &GetProgramEnvironment());

	// Output errors
	for (size_t i=0; i<output.size(); i++)
		wxLogError(output[i]);
}