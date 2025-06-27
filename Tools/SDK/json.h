#pragma once

typedef struct IUnknown IUnknown;

typedef void* JSONFile;

#include "main.h"


/*********************************
        Function Prototypes
*********************************/

JSONFile  json_createproject(wxString filepath);
JSONFile  json_load(wxString filepath);
void      json_save(JSONFile json, wxString filepath);