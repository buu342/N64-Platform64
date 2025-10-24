#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>


/*=============================================================
                            Structs
=============================================================*/

typedef void* JSONFile;


/*=============================================================
                      Function Prototypes
=============================================================*/

JSONFile  json_createproject(wxString filepath);
JSONFile  json_load(wxString filepath);
void      json_save(JSONFile json, wxString filepath);
void      json_free(JSONFile json);