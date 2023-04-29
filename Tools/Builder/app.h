#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include "main.h"

class App : public wxApp
{
	private:
		Main* m_ProgramFrame = nullptr;

	public:
		App();
		~App();
		virtual bool OnInit();
};
