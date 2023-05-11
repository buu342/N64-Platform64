#pragma once

typedef struct IUnknown IUnknown;

#include "compunit.h"
#include <sys/stat.h>
#include <wx/utils.h>
#include <wx/treelist.h>
#include <wx/filename.h>
#include <map>
#include <vector>

time_t LastModTime(const char* path);
unsigned int GetMBits(const char* path);
wxExecuteEnv GetProgramEnvironment();
wxString RelativeProjectPath(wxFileName file);