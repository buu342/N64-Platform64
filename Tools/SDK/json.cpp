/***************************************************************
                            json.cpp

JSON handling functions. This file was created to isolate the 
bulky JSON header only library. 
***************************************************************/

#include "json.h"
#include "Include/json.hpp"
#include <fstream>
#include <wx/msgdlg.h>


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
    json_loadproject
    Load a list of categories from a JSON file
    @param  The filepath of the JSON file
    @return 1 if successful, 0 if not
==============================*/

int json_loadproject(wxString filepath)
{
    nlohmann::json pagejson = json_fromfilepath(filepath);
    return pagejson == NULL ? 0 : 1;
}


/*==============================
    json_save
    TODO
==============================*/

void json_save(wxString filepath)
{

}