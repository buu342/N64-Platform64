/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include "app.h"
#include "json.h"
#include "Include/json.hpp"
#include <fstream>
#include <wx/msgdlg.h>
#include <wx/textfile.h>


/*********************************
             Globals
*********************************/

nlohmann::json g_project;


/*==============================
    json_fromfilepath
    Gets a JSON object from a filepath
    @param  The filepath of the JSON file
    @return The JSON object or NULL
==============================*/

nlohmann::json json_fromfilepath(wxString filepath)
{
    try
    {
        if (wxFileExists(filepath))
        {
            std::ifstream stream(filepath.mb_str());
            if (stream.is_open())
                return nlohmann::json::parse(stream);
        }
        else
            wxMessageBox(wxString("JSON file at '"+filepath+"' does not exist."), wxString("Error opening JSON"), wxICON_ERROR);
    }
    catch (nlohmann::json::exception e)
    {
        wxMessageBox(wxString(e.what()), wxString("Error parsing JSON"), wxICON_ERROR);
    }
    return NULL;
}


/*==============================
    json_createproject
    Create a new P64 project JSON file
    @param  The filepath of the JSON file to create
    @return 1 if successful, 0 if not
==============================*/

int json_createproject(wxString filepath)
{
    try
    {
        g_project = {};
        g_project["Platform64"] = {};
        g_project["Platform64"]["Version"] = PROJECT_VERSION;
        json_save(filepath);
        return 1;
    }
    catch (nlohmann::json::exception e)
    {
        wxMessageBox(wxString(e.what()), wxString("Error creating JSON"), wxICON_ERROR);
        return 0;
    }
}


/*==============================
    json_loadproject
    Load a P64 project from a JSON file
    @param  The filepath of the JSON file
    @return 1 if successful, 0 if not
==============================*/

int json_loadproject(wxString filepath)
{
    g_project = json_fromfilepath(filepath);
    return g_project == NULL ? 0 : 1;
}


/*==============================
    json_save
    TODO
==============================*/

void json_save(wxString filepath)
{
    wxTextFile out_proj(filepath);
    if (!out_proj.Exists())
        out_proj.Create();
    out_proj.Clear();
    out_proj.AddLine(wxString::FromUTF8(g_project.dump(4)));
    out_proj.Write();
    out_proj.Close();
}