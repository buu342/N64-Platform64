#include "compunit.h"
#include "main.h"
#include "helper.h"
#include <wx/utils.h> 
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/dir.h>

CompUnit::CompUnit(wxTreeCtrl* tree, wxTreeItemId id)
{
	/*
	wxTextFile file;
	wxString path = "";
	wxString output = tree->GetItemText(id);
	wxTreeItemId curid = id;

	// First, generate the path
	while (tree->GetItemParent(curid) != NULL)
	{
		curid = tree->GetItemParent(curid);
		path.Prepend(tree->GetItemText(curid)+wxString("/"));
	}

	// Now set the file pointers
	output.Replace(".c", ".o");
	this->m_File.Assign(PROJECTPATH + path + tree->GetItemText(id));
	this->m_Output.Assign(OUTPUTPATH + "/" + path + output);

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
	*/
}

CompUnit::~CompUnit()
{

}

bool CompUnit::ShouldRebuild()
{
	/*
	wxStructStat stat_input;
	wxStructStat stat_output;
	stat_input.st_mtime = LastModTime(this->m_File.GetFullPath());
	stat_output.st_mtime = LastModTime(this->m_Output.GetFullPath());

	// Check if the object file exists
	if (!wxFileExists(this->m_Output.GetFullPath()))
	{
		wxLogVerbose(this->m_Output.GetFullName() + " does not exist");
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
	*/
}

wxFileName CompUnit::GetOutputName()
{
	return this->m_Output;
}

void CompUnit::Compile(bool debug)
{
	/*
	wxString command;
	wxExecuteEnv env;
	wxArrayString output;
	wxEnvVariableHashMap vars;
	wxString optimizer = "-g";
	wxString lcdefs = "-DDEBUG";

	// Set optimizer flags
	if (!debug)
	{
		optimizer = "-O2";
		lcdefs = "-D_FINALROM -DNDEBUG";
	}

	// Setup the environment
	vars["ROOT"] = wxString(LIBULTRAPATH);
	vars["gccdir"] = wxString(LIBULTRAPATH) + wxString("/gcc");
	vars["PATH"] = MIPSEFULLPATH + wxString(";") + LIBULTRAPATH + wxString("/usr/sbin;C:/WINDOWS/system32;");
	vars["gccsw"] = wxString("-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__");
	vars["n64align"] = "on";
	env.cwd = PROJECTPATH;
	env.env = vars;

	// Run GCC
	wxDir::Make(this->m_Output.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	command = MIPSEFULLPATH + "/exew32.exe gcc.exe "
			+ lcdefs + " "
			+ global_projectconfig.Flags_GCC + " "
			+ optimizer
			+ " -c " + this->m_File.GetFullPath()
			+ " -o " + this->m_Output.GetFullPath();
	wxLogVerbose("> " + command);
	wxExecute(command, output, wxEXEC_SYNC,  &env);

	// Output errors
	for (size_t i=0; i<output.size(); i++)
		wxLogError(output[i]);
	*/
}