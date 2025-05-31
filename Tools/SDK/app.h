#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include "main.h"


/*********************************
             Classes
*********************************/

class App : public wxApp
{
	private:
		Frame_Main* m_Frame = nullptr;
        
	protected:
    
	public:
		App();
		~App();
		virtual bool OnInit();
};