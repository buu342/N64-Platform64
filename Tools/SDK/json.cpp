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


/*==============================
    json_fromfilepath
    Gets a JSON object from a filepath
    @param  The filepath of the JSON file
    @return The JSON object or NULL
==============================*/

static nlohmann::json json_fromfilepath(wxString filepath)
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
    @return The JSON file, or NULL
==============================*/

JSONFile json_createproject(wxString filepath)
{
    nlohmann::json* j = new nlohmann::json();
    try
    {
        (*j) = {};
        (*j)["Platform64"] = {};
        (*j)["Platform64"]["Version"] = PROJECT_VERSION;
        json_save(j, filepath);
        return j;
    }
    catch (nlohmann::json::exception e)
    {
        wxMessageBox(wxString(e.what()), wxString("Error creating JSON"), wxICON_ERROR);
        return NULL;
    }
}


/*==============================
    json_load
    Load a json file from a given file path
    @param  The path to the JSON file to load
    @return The loaded JSON file
==============================*/

JSONFile json_load(wxString filepath)
{
    nlohmann::json* j = new nlohmann::json(json_fromfilepath(filepath));
    return j;
}


/*==============================
    json_save
    Saves a json file to a given file path
    @param The JSON file to save
    @param The path to save the JSON file to
==============================*/

void json_save(JSONFile file, wxString filepath)
{
    wxTextFile out_proj(filepath);
    if (!out_proj.Exists())
        out_proj.Create();
    out_proj.Clear();
    out_proj.AddLine(wxString::FromUTF8(((nlohmann::json*)file)->dump(4)));
    out_proj.Write();
    out_proj.Close();
}