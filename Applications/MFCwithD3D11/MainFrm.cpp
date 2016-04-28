//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "stdafx.h"

#include "MFCwithD3D11.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
//--------------------------------------------------------------------------------
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}
//--------------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}
//--------------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	return 0;
}
//--------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG
//--------------------------------------------------------------------------------
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// We are creating two splitter windows - one to split the screen into a left
	// and right side, and one to split the right side into a top and bottom.

	m_SplitterMain.CreateStatic( this, 1, 2 );
	m_SplitterMain.CreateView( 0, 0, RUNTIME_CLASS (CMFCwithD3D11View), CSize(800,800), pContext );

	m_SplitterSub.CreateStatic( &m_SplitterMain, 2, 1, WS_CHILD|WS_VISIBLE, m_SplitterMain.IdFromRowCol(0,1) );
	m_SplitterSub.CreateView( 0, 0, RUNTIME_CLASS (CMFCwithD3D11View), CSize(300,200), pContext );
	m_SplitterSub.CreateView( 1, 0, RUNTIME_CLASS (CMFCwithD3D11View), CSize(300,300), pContext );

	//// Viewing parameters for the larger left side window
	//Glyph3::Camera* pCamera = ((CMFCwithD3D11View*)m_SplitterMain.GetPane( 0, 0 ))->pCamera;
	//pCamera->Spatial().SetRotation( Glyph3::Vector3f( 0.65f, -0.75f, 0.0f ) );
	//pCamera->Spatial().SetTranslation( Glyph3::Vector3f( 30.0f, 30.0f, -30.0f ) );

	//// Viewing parameters for the top right side window
	//pCamera = ((CMFCwithD3D11View*)m_SplitterSub.GetPane( 0, 0 ))->pCamera;
	//pCamera->Spatial().SetRotation( Glyph3::Vector3f( 1.5f, 0.0, 0.0f ) );
	//pCamera->Spatial().SetTranslation( Glyph3::Vector3f( 0.0f, 30.0f, 0.0f ) );

	//// Viewing parameters for the bottom right side window
	//pCamera = ((CMFCwithD3D11View*)m_SplitterSub.GetPane( 1, 0 ))->pCamera;
	//pCamera->Spatial().SetRotation( Glyph3::Vector3f( 0.65f, 0.75f, 0.0f ) );
	//pCamera->Spatial().SetTranslation( Glyph3::Vector3f( -30.0f, 30.0f, -30.0f ) );

	return true;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}
//--------------------------------------------------------------------------------