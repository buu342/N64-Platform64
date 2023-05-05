#include "helper.h"
#include <wx/file.h>
#ifndef LINUX
	#include <Windows.h>
#endif

time_t LastModTime(const char* path)
{
	struct stat finfo;
	if (!wxFileExists(path))
		return 0;
	#ifndef LINUX
		LARGE_INTEGER lt;
		WIN32_FILE_ATTRIBUTE_DATA fdata;
		GetFileAttributesExA(path, GetFileExInfoStandard, &fdata);
		lt.LowPart = fdata.ftLastWriteTime.dwLowDateTime;
		lt.HighPart = (long)fdata.ftLastWriteTime.dwHighDateTime;
		finfo.st_mtime = (time_t)(lt.QuadPart * 1e-7);
	#else
		stat(path, &finfo);
	#endif
	return finfo.st_mtime;
}

unsigned int GetMBits(const char* path)
{
	unsigned int v = 1;
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}