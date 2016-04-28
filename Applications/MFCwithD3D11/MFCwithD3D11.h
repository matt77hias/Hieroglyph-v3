//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// CMFCwithD3D11App
//
// This is the basic application object for an SDI MFC application.  It is 
// responsible for the message pump, as well as creating the frame window that
// will be used in this program.  In addition, it will create and manage the basic
// Hieroglyph 3 elements needed as the basis for the other classes.
//--------------------------------------------------------------------------------
#pragma once

#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
//--------------------------------------------------------------------------------
class CMFCwithD3D11App : public CWinApp
{
public:
	CMFCwithD3D11App();

	Glyph3::Timer*			pTimer;    // These are the most basic elements that
	Glyph3::RendererDX11*	pRenderer; // are required for the Hieroglyph 3 
	Glyph3::EventManager*	pEventMgr; // components to function properly.

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};
//--------------------------------------------------------------------------------
extern CMFCwithD3D11App theApp;
//--------------------------------------------------------------------------------