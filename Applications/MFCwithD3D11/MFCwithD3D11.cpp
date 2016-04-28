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
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCwithD3D11.h"
#include "MainFrm.h"

#include "MFCwithD3D11Doc.h"
#include "MFCwithD3D11View.h"

#include "EvtFrameStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMFCwithD3D11App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMFCwithD3D11App::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------
CMFCwithD3D11App::CMFCwithD3D11App()
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MFCwithD3D11.AppID.NoVersion"));
}
//--------------------------------------------------------------------------------
// The one and only CMFCwithD3D11App object
CMFCwithD3D11App theApp;
//--------------------------------------------------------------------------------
BOOL CMFCwithD3D11App::InitInstance()
{
	pTimer = new Glyph3::Timer();
	pTimer->Update();

	// Open the log file for use by the engine/application in general.

	Glyph3::Log::Get().Open();

	// Create the event manager here since it must be the first engine component
	// instantiated.

	pEventMgr = new Glyph3::EventManager();


	pRenderer = new Glyph3::RendererDX11();

	if ( !pRenderer->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) ) {
		Glyph3::Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !pRenderer->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) ) {
			::MessageBox( 0, L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	pRenderer->MultiThreadingConfig.SetConfiguration( false );



	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCwithD3D11Doc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMFCwithD3D11View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand


	return TRUE;
}
//--------------------------------------------------------------------------------
int CMFCwithD3D11App::ExitInstance()
{
	pRenderer->Shutdown();
	if ( pRenderer ) {
		delete pRenderer;
	}

	if ( pTimer != NULL ) {
		delete pTimer;
	}

	if ( pEventMgr ) {
		delete pEventMgr;
	}

	Glyph3::Log::Get().Close();

	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}
//--------------------------------------------------------------------------------
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};
//--------------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}
//--------------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
//--------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------
void CMFCwithD3D11App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
//--------------------------------------------------------------------------------
BOOL CMFCwithD3D11App::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	CWinApp::OnIdle(lCount);

	// Update the timer and then update the current scene with the elapsed time.
	
	pTimer->Update();

	pEventMgr->ProcessEvent( Glyph3::EvtFrameStartPtr( new Glyph3::EvtFrameStart( pTimer->Elapsed() ) ) );

	Glyph3::Scene* pScene = CMFCwithD3D11Doc::GetDoc()->pScene;

	if ( pScene != NULL ) {
		pScene->Update( pTimer->Elapsed() );
	}

	// Update the views to reflect the new state of the scene.
	AfxGetMainWnd()->Invalidate(false); 

	return TRUE;
}
//--------------------------------------------------------------------------------