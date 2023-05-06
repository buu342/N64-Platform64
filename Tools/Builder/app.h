#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include "main.h"
#include "preferences.h"

#define PROGRAM_NAME  "Builder"

// Icons
extern wxIcon   iconbm_prog;
extern wxBitmap iconbm_c;
extern wxBitmap iconbm_h;
extern wxIcon   iconbm_config;

class App : public wxApp
{
	private:
		Main* m_ProgramFrame;

	public:
		App();
		~App();
		virtual bool OnInit();
};