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
// CMFCwithD3D11View
//
// The view performs the bulk of the work to visualize a given scene.  This is
// accomplished with a few primary objects from the Hieroglyph 3 framework:
//
// 1. Swap Chain / ResourcePtr: Provide a resource to render into that is linked
//                              to the Win32 window.  This allows a rendered 
//                              output to be presented in the output window.
//
// 2. Camera / render views: The camera defines where the scene will be rendered
//                           from for this view.  The actual rendering is carried
//                           out by render views, which are set as a portion of 
//                           the camera object.
//
// These objects will be used to interact with the Scene object that is part of
// the document, and produce a rendered version of it in this view's client area.
//--------------------------------------------------------------------------------

#pragma once
#include "MFCwithD3D11Doc.h"

//--------------------------------------------------------------------------------
class CMFCwithD3D11View : public CView
{
protected: // create from serialization only
	CMFCwithD3D11View();
	DECLARE_DYNCREATE(CMFCwithD3D11View)

// Attributes
public:
	CMFCwithD3D11Doc* GetDocument() const;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMFCwithD3D11View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	Glyph3::Camera*					pCamera;
	int								SwapChain;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in MFCwithD3D11View.cpp
inline CMFCwithD3D11Doc* CMFCwithD3D11View::GetDocument() const
   { return reinterpret_cast<CMFCwithD3D11Doc*>(m_pDocument); }
#endif
//--------------------------------------------------------------------------------
