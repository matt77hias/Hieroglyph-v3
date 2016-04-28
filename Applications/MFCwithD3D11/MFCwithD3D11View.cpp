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
#include "MFCwithD3D11Doc.h"
#include "MFCwithD3D11View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//--------------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMFCwithD3D11View, CView)

BEGIN_MESSAGE_MAP(CMFCwithD3D11View, CView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------
CMFCwithD3D11View::CMFCwithD3D11View()
{
	pCamera = 0;
	SwapChain = -1;
}
//--------------------------------------------------------------------------------
CMFCwithD3D11View::~CMFCwithD3D11View()
{
}
//--------------------------------------------------------------------------------
BOOL CMFCwithD3D11View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}
//--------------------------------------------------------------------------------
void CMFCwithD3D11View::OnDraw(CDC* /*pDC*/)
{
	CMFCwithD3D11Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	pCamera->SetScene( pDoc->pScene );
	pCamera->RenderFrame( Glyph3::RendererDX11::Get() );

	Glyph3::RendererDX11::Get()->Present( this->GetSafeHwnd(), SwapChain, 1, 0 );
}
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void CMFCwithD3D11View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCwithD3D11View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCwithD3D11Doc* CMFCwithD3D11View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCwithD3D11Doc)));
	return (CMFCwithD3D11Doc*)m_pDocument;
}
#endif //_DEBUG
//--------------------------------------------------------------------------------
void CMFCwithD3D11View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	RECT client;
	this->GetClientRect( &client );

	Glyph3::RendererDX11* pRenderer = Glyph3::RendererDX11::Get();

	LONG width = client.right - client.left;
	LONG height = client.bottom - client.top;

	// Create a swap chain for the window.
	Glyph3::SwapChainConfigDX11 Config;
	Config.SetWidth( width );
	Config.SetHeight( height );
	Config.SetOutputWindow( this->GetSafeHwnd() );
	
	SwapChain = pRenderer->CreateSwapChain( &Config );


	// Acquire the swap chain's render target reference to use in the render views.

	Glyph3::ResourcePtr BackBuffer = pRenderer->GetSwapChainResource( SwapChain );


	// Create and configure the render views for use in our camera.

	Glyph3::ViewPerspective* pPerspectiveView = new Glyph3::ViewPerspective( *pRenderer, BackBuffer );
	pPerspectiveView->SetBackColor( Glyph3::Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );
	
	Glyph3::ViewTextOverlay* pTextOverlayView = new Glyph3::ViewTextOverlay( *pRenderer, BackBuffer );


	// Create and configure the camera for this view.

	pCamera = new Glyph3::FirstPersonCamera();
	pCamera->Spatial().SetRotation( Glyph3::Vector3f( 0.65f, -0.75f, 0.0f ) );
	pCamera->Spatial().SetTranslation( Glyph3::Vector3f( 30.0f, 30.0f, -30.0f ) );
	pCamera->SetCameraView( pPerspectiveView );
	pCamera->SetOverlayView( pTextOverlayView );
	pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(width) / static_cast<float>(height), static_cast<float>( GLYPH_PI ) / 4.0f );

	GetDocument()->pScene->AddCamera( pCamera );


	// Create the parameters for use with this effect

	Glyph3::Vector4f LightParams = Glyph3::Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	Glyph3::VectorParameterDX11* pLightColor = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	pLightColor->InitializeParameterData( &LightParams );

	Glyph3::Vector4f LightPosition = Glyph3::Vector4f( 20.0f, 20.0f, -20.0f, 0.0f );
	Glyph3::VectorParameterDX11* pLightPosition = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	pLightPosition->InitializeParameterData( &LightPosition );
}
//--------------------------------------------------------------------------------
BOOL CMFCwithD3D11View::OnEraseBkgnd(CDC* pDC)
{
	// Always return false in order to avoid any flickering of the rendered scene.

	return FALSE;
}
//--------------------------------------------------------------------------------
void CMFCwithD3D11View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if ( cx < 1 ) cx = 1;
	if ( cy < 1 ) cy = 1;

	// Resize our rendering window state if the handle matches
	Glyph3::RendererDX11::Get()->ResizeSwapChain( SwapChain, cx, cy );

	// Update the projection matrix of our camera
	if ( pCamera != 0 ) {
		
		pCamera->SetAspectRatio( static_cast<float>(cx) / static_cast<float>(cy) );
		
		// Update the render views being used to render the scene
		Glyph3::SceneRenderTask* pRenderView = pCamera->GetCameraView();

		if ( pRenderView != 0 ) {
			pRenderView->Resize( cx, cy );
		}	
	}
}
//--------------------------------------------------------------------------------