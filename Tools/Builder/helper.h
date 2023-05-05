#pragma once

typedef struct IUnknown IUnknown;

#include <sys/stat.h>

time_t LastModTime(const char* path);
unsigned int GetMBits(const char* path);