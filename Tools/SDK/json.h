#pragma once

typedef struct IUnknown IUnknown;

#include "main.h"


/*********************************
        Function Prototypes
*********************************/

int           json_createproject(wxString filepath);
int           json_loadproject(wxString filepath);
void          json_save(wxString filepath);