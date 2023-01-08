
// MotionGUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMotionGUIApp:
// See MotionGUI.cpp for the implementation of this class
//

class CMotionGUIApp : public CWinApp
{
public:
	CMotionGUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMotionGUIApp theApp;