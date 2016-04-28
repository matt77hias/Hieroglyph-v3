//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"
#include "EvtMouseWheel.h"

#include "ScriptManager.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "SamplerStateConfigDX11.h"
#include "Texture2dDX11.h"
#include "IParameterManager.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bAppInitialized = false;
	m_bAppShuttingDown = false;
}
//--------------------------------------------------------------------------------
void App::MessageLoop()
{
	//Application::MessageLoop();
    MSG msg;

	while (GetMessage( &msg, NULL, 0, 0 ))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}
//--------------------------------------------------------------------------------
LRESULT App::WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	//return( RenderApplication::WindowProc( hwnd, msg, wparam, lparam ) );

	switch( msg )
	{	
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				
				if ( m_bAppInitialized && !m_bAppShuttingDown ) {
					Update();
				}

				return 0;

			} break;
		case  WM_MOUSEMOVE:
			{
				// we do our own mouse move handling before letting the framework do it
				return( HandleMouseMove( hwnd, wparam, lparam ) );  

				// Now we let the framework handle the mouse move as it likes
				//return RenderApplication::WindowProc( hwnd, msg, wparam, lparam );
			} break;
		case  WM_MOUSEWHEEL:
			{
				// we do our own mouse move handling before letting the framework do it
				return( HandleMouseWheel( hwnd, wparam, lparam ) );  

				// Now we let the framework handle the mouse move as it likes
				//return RenderApplication::WindowProc( hwnd, msg, wparam, lparam );
			} break;
		case  WM_SIZE:
			{
				if ( m_bAppInitialized && !m_bAppShuttingDown ) {
					HandleResize( hwnd, wparam, lparam );  
					return RenderApplication::WindowProc( hwnd, msg, wparam, lparam );
				}

				// Now we let the framework handle the mouse move as it likes
				return 0;
			} break;
		default:
			{
				// If we haven't defined a handler for a message, then we pass it 
				// to our base class to be handled.
				return RenderApplication::WindowProc( hwnd, msg, wparam, lparam );
			}
	}
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 1024, 640, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	m_UIData.LMouseDown = false;
	m_UIData.RMouseDown = false;
	m_UIData.LastMouseX = -1;
	m_UIData.LastMouseY = -1;
	m_UIData.MouseDeltaX = 0;
	m_UIData.MouseDeltaY = 0;

	m_iImage = -1;
	m_iAlgorithm = 0;
	m_iSampler = 0;

    SetMultiThreadedMode( false );

	// Create render effects for each of the required compute shaders for all of
	// our various algorithms.

	m_pBruteForceGaussian = new RenderEffectDX11();
	m_pBruteForceGaussian->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianBruteForceCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableGaussianX = new RenderEffectDX11();
	m_pSeparableGaussianX->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableGaussianY = new RenderEffectDX11();
	m_pSeparableGaussianY->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pCachedGaussianX = new RenderEffectDX11();
	m_pCachedGaussianX->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianCachedCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pCachedGaussianY = new RenderEffectDX11();
	m_pCachedGaussianY->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"GaussianCachedCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) ) );


	m_pBruteForceBilateral = new RenderEffectDX11();
	m_pBruteForceBilateral->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"BilateralBruteForceCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableBilateralX = new RenderEffectDX11();
	m_pSeparableBilateralX->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"BilateralSeparableCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) ) );

	m_pSeparableBilateralY = new RenderEffectDX11();
	m_pSeparableBilateralY->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"BilateralSeparableCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) ) );


	// Create the material for use by the entity.
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );

	RenderEffectDX11* pEffect = new RenderEffectDX11();
	pEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"ImageViewerVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );
	pEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"ImageViewerPS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	// Enable the material to render the given view type, and set its effect.

	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;


	// Create the geometry that will fill the screen with our results - just a full
	// screen quad in this case.

	GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
	GeometryGeneratorDX11::GenerateFullScreenQuad( pGeometry );
	pGeometry->LoadToBuffers();


	// Here we load our desired texture.

	
	m_Texture[0] = m_pRenderer11->LoadTexture( L"Outcrop.png" );
	m_Texture[1] = m_pRenderer11->LoadTexture( L"fruit.png" );
	m_Texture[2] = m_pRenderer11->LoadTexture( L"Hex.png" );
	m_Texture[3] = m_pRenderer11->LoadTexture( L"EyeOfHorus.png" );
	m_Texture[4] = m_pRenderer11->LoadTexture( L"Tiles.png" );
	

	// Create the texture for output of the compute shader.
	
	Texture2dConfigDX11 FilteredConfig;
	FilteredConfig.SetColorBuffer( 640, 480 ); 
	FilteredConfig.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );

	m_Intermediate = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );
	m_Output = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );


	// Get references to the parameters that will be used in rendering.

	m_pColorMapParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ColorMap00" ) );
	m_pInputParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"InputMap" ) );
	m_pOutputParameter = m_pRenderer11->m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"OutputMap" ) );
	m_pWindowSizeParameter = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"WindowSize" ) );
	m_pImageSizeParameter = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ImageSize" ) );
	m_pViewingParamsParameter = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ViewingParams" ) );

	WindowSize = Vector4f( (float)m_iWidth, (float)m_iHeight, 0.0f, 0.0f );
	m_pWindowSizeParameter->InitializeParameterData( &WindowSize );

	ImageSize = Vector4f( 640.0f, 480.0f, 0.0f, 0.0f );
	m_pImageSizeParameter->InitializeParameterData( &ImageSize );

	ViewingParams = Vector4f( 0.5f, 0.5f, 1.0f, 1.0f );
	m_pViewingParamsParameter->InitializeParameterData( &ViewingParams );

	// Create a sampler with wrapping texture address mode.
	SamplerStateConfigDX11 SamplerConfig;
	m_Samplers[0] = RendererDX11::Get()->CreateSamplerState( &SamplerConfig );

	// Create a sampler with border texture address mode.
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.BorderColor[0] = 0.0f;
	SamplerConfig.BorderColor[1] = 0.0f;
	SamplerConfig.BorderColor[2] = 0.0f;
	SamplerConfig.BorderColor[3] = 0.0f;

	m_Samplers[1] = RendererDX11::Get()->CreateSamplerState( &SamplerConfig );

	SamplerParameterDX11* pSamplerParameter = 
		RendererDX11::Get()->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"LinearSampler" ) );
	pSamplerParameter->InitializeParameterData( &m_iSampler );

	// Since this one won't be changing, it can be initialized here once, and then
	// not updated again later.

	m_pColorMapParameter->InitializeParameterData( &m_Output->m_iResourceSRV );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -70.0f, 30.5f, -75.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry( pGeometry );
	m_pActor->GetBody()->Visual.SetMaterial( pMaterial );

	m_pScene->AddActor( m_pActor );

	SelectNextImage(); 

	// This will trigger a WM_PAINT which when handled will render the image as we have set it up.
	InvalidateRect(m_pWindow->GetHandle(), NULL, TRUE);

	m_bAppInitialized = true;
}
//--------------------------------------------------------------------------------
void App::SelectNextImage()
{
	// Select the next image index.
	m_iImage++;
	if ( m_iImage > 4 ) m_iImage = 0;

	// Get the description of the new image.
	Texture2dDX11* pTexture = m_pRenderer11->GetTexture2DByIndex( m_Texture[m_iImage]->m_iResource );
	D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

	// Update the rendering parameters for image size.
	ImageSize.x = (float)desc.Width;
	ImageSize.y = (float)desc.Height;
	m_pImageSizeParameter->InitializeParameterData( &ImageSize );

	// Resize the resources used for processing.
	m_pRenderer11->ResizeTexture( this->m_Intermediate, desc.Width, desc.Height );
	m_pRenderer11->ResizeTexture( this->m_Output, desc.Width, desc.Height );
}
//--------------------------------------------------------------------------------
void App::SelectNextAlgorithm()
{
	m_iAlgorithm++;
	if ( m_iAlgorithm > 4 ) m_iAlgorithm = 0;
}
//--------------------------------------------------------------------------------
void App::SelectNextSampler()
{
	m_iSampler++;
	if ( m_iSampler > 1 ) m_iSampler = 0;

	SamplerParameterDX11* pSamplerParameter = 
		RendererDX11::Get()->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"LinearSampler" ) );
	pSamplerParameter->InitializeParameterData( &m_Samplers[m_iSampler] );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Setup the text rendering for the onscreen UI.

	std::wstringstream out;
	out << L"Hieroglyph 3 : ImageProcessor\nFPS: " << m_pTimer->Framerate() << std::endl;
	out << L"Press I to select the next image." << std::endl;
	out << L"Press N to select the next algorithm." << std::endl;
	out << L"Press SPACE to select the next texture sampling mode." << std::endl;
	out << std::endl;
	out << L"Drag the left mouse button to pan." << std::endl;
	out << L"Drag the right mouse button up or down to zoom." << std::endl;
	out << L"Mouse wheel can also be used to zoom." << std::endl;
	out << std::endl;
	out << L"Current Window Size: " << WindowSize.x << L", " << WindowSize.y << std::endl;
	out << L"Current Image Size: " << ImageSize.x << L", " << ImageSize.y << std::endl;
	out << L"Current Algorithm: " << std::endl;


	// Perform the filtering with the compute shader.  The assumption in this case
	// is that the texture is 640x480 - if there is a different size then the 
	// dispatch call can be modified accordingly.

	// Brute force Gaussian
	if ( m_iAlgorithm == 0 )
	{
		UINT DispatchX = (UINT)( ceil( ImageSize.x / 32.0f ) );
		UINT DispatchY = (UINT)( ceil( ImageSize.y / 32.0f ) );

		out << L"Brute Force Gaussian" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceGaussian, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();
	}

	// Separable Gaussian
	if ( m_iAlgorithm == 1 )
	{
		UINT DispatchX = (UINT)( ceil( ImageSize.x / 640.0f ) );
		UINT DispatchY = (UINT)( ImageSize.y );
		
		out << L"Separable Gaussian" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianX, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();

		DispatchX = (UINT)( ImageSize.x );
		DispatchY = (UINT)( ceil( ImageSize.y / 480.0f ) );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianY, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();
	}

	// Cached Gaussian
	if ( m_iAlgorithm == 2 )
	{
		UINT DispatchX = (UINT)( ceil( ImageSize.x / 640.0f ) );
		UINT DispatchY = (UINT)( ImageSize.y );

		out << L"Cached Separable Gaussian" << std::endl;
		
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianX, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();

		DispatchX = (UINT)( ImageSize.x );
		DispatchY = (UINT)( ceil( ImageSize.y / 480.0f ) );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianY, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();
	}

	// Brute force Bilateral
	if ( m_iAlgorithm == 3 )
	{
		UINT DispatchX = (UINT)( ceil( ImageSize.x / 32.0f ) );
		UINT DispatchY = (UINT)( ceil( ImageSize.y / 32.0f ) );

		out << L"Brute Force Bilateral" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceBilateral, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();
	}

	// Separable Bilateral
	if ( m_iAlgorithm == 4 )
	{
		UINT DispatchX = (UINT)( ceil( ImageSize.x / 640.0f ) );
		UINT DispatchY = (UINT)( ImageSize.y );

		out << L"Separable Bilateral" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableBilateralX, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();

		DispatchX = (UINT)( ImageSize.x );
		DispatchY = (UINT)( ceil( ImageSize.y / 480.0f ) );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableBilateralY, DispatchX, DispatchY, 1, m_pRenderer11->m_pParamMgr );

		m_pRenderer11->pImmPipeline->ClearPipelineResources();
		m_pRenderer11->pImmPipeline->ApplyPipelineResources();
	}

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 10.0f, 10.0f, 0.0f ), Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	m_bAppShuttingDown = true;

	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleResize( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
	EvtWindowResize e( hwnd, wparam, lparam );

	WindowSize.x = (float)e.NewWidth();
	WindowSize.y = (float)e.NewHeight();
	m_pWindowSizeParameter->InitializeParameterData( &WindowSize );
	
	return( true );
}
//--------------------------------------------------------------------------------
bool App::HandleMouseMove( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
	EvtMouseMove e( hwnd, wparam, lparam );

	// If LBUTTON is down, then we are panning
	if ( e.LButtonDown() ) {		

		// Initialize the data for starting the panning event if this is
		// the first mouse move event with the button down.  Otherwise we
		// update the data live to keep up with the mouse movement.
		
		if ( !m_UIData.LMouseDown ) {
			m_UIData.LMouseDown = true;
			m_UIData.RMouseDown = false;
			m_UIData.LastMouseX = e.GetX();
			m_UIData.LastMouseY = e.GetY();
			m_UIData.MouseDeltaX = 0;
			m_UIData.MouseDeltaY = 0;
		} else {
			m_UIData.MouseDeltaX = m_UIData.LastMouseX - e.GetX();
			m_UIData.MouseDeltaY = m_UIData.LastMouseY - e.GetY();
			m_UIData.LastMouseX = e.GetX();
			m_UIData.LastMouseY = e.GetY();
		}

		// Here we translate the observed mouse movement into a change in
		// the offset to view the images with.
		ViewingParams.x += ((float)m_UIData.MouseDeltaX) / ViewingParams.z; 
		ViewingParams.y += ((float)m_UIData.MouseDeltaY) / ViewingParams.w; 
		m_pViewingParamsParameter->InitializeParameterData( &ViewingParams );

	} else {

		// LBUTTON is not down, so reset its flags and data.
		if ( m_UIData.LMouseDown ) {
			m_UIData.LMouseDown = false;
			m_UIData.MouseDeltaX = 0;
			m_UIData.MouseDeltaY = 0;
		}

		// If RBUTTON is down, then we are zooming
		if ( e.RButtonDown() ) {	

			// Initialize the data for starting the zooming event if this is
			// the first mouse move event with the button down.  Otherwise we
			// update the data live to keep up with the mouse movement.
			
			if ( !m_UIData.RMouseDown ) {
				m_UIData.RMouseDown = true;
				m_UIData.LastMouseX = e.GetX();
				m_UIData.LastMouseY = e.GetY();
				m_UIData.MouseDeltaX = 0;
				m_UIData.MouseDeltaY = 0;
			} else {
				m_UIData.MouseDeltaX = m_UIData.LastMouseX - e.GetX();
				m_UIData.MouseDeltaY = m_UIData.LastMouseY - e.GetY();
				m_UIData.LastMouseX = e.GetX();
				m_UIData.LastMouseY = e.GetY();
			}

			ViewingParams.z += ((float)m_UIData.MouseDeltaY) * 0.001f;
			ViewingParams.w += ((float)m_UIData.MouseDeltaY) * 0.001f;
			m_pViewingParamsParameter->InitializeParameterData( &ViewingParams );

		} else {

			// RBUTTON is not down, so reset its flags and data.
			if ( m_UIData.RMouseDown ) {
				m_UIData.RMouseDown = false;
				m_UIData.MouseDeltaX = 0;
				m_UIData.MouseDeltaY = 0;
			}
		} 
	} 

	InvalidateRect(m_pWindow->GetHandle(), NULL, FALSE);

	return( true );
}
//--------------------------------------------------------------------------------
bool App::HandleMouseWheel( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
	EvtMouseWheel e( hwnd, wparam, lparam );

	// Get the wheel delta.  This is a short value (16-bit, positive or negative)
	// that is stored in a int.  It is 120 value increments unless there is a
	// high precision mouse in use.  We just scale the values and then apply
	// them to our zoom factor, then re-render the scene by invalidating the
	// window area.

	int delta = e.GetWheelDelta();

	ViewingParams.z += ((float)delta) * 0.0001f;
	ViewingParams.w += ((float)delta) * 0.0001f;
	m_pViewingParamsParameter->InitializeParameterData( &ViewingParams );

	InvalidateRect(m_pWindow->GetHandle(), NULL, FALSE);

	return( true );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();

		return( true );
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();
		
		if ( key == 0x4E ) // 'N' Key - Switch to the next algorithm
		{
			SelectNextAlgorithm();
			InvalidateRect(m_pWindow->GetHandle(), NULL, FALSE);

			return( true );
		}
		else if ( key == 0x49 ) // 'I' Key - Switch to the next image
		{
			SelectNextImage();			
			InvalidateRect(m_pWindow->GetHandle(), NULL, FALSE);

			return( true );
		}
		else if ( key == VK_SPACE ) // 'Space' Key - Switch to the next sampler
		{
			SelectNextSampler();			
			InvalidateRect(m_pWindow->GetHandle(), NULL, FALSE);

			return( true );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"ImageProcessor" ) );
}
//--------------------------------------------------------------------------------