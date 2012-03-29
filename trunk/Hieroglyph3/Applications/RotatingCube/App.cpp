//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//
// Author: Jason Kinzer (based on DirectX Tutorial 4)
//--------------------------------------------------------------------------------

#include "App.h"
#include "Log.h"

#include <sstream>
#include <xnamath.h>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "PipelineManagerDX11.h"

#include "BlendStateConfigDX11.h"
#include "BufferConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"


using namespace Glyph3;

//--------------------------------------------------------------------------------
// Structure for Vertex Buffer
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bSaveScreenshot = false;
}

//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 640;
	int height = 480;

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 25, 25 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( GetName() );
	m_pWindow->Initialize( this );

	
	// Create the renderer and initialize it for the desired device
	// type and feature level.
	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_10_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0 ) )
		{
			ShowWindow( m_pWindow->GetHandle(), SW_HIDE );
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.

	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
	m_pWindow->SetSwapChain( m_iSwapChain );

	// We'll keep a copy of the render target index to use in later examples.

	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_iSwapChain );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );
	
	// Bind the swap chain render target and the depth buffer for use in 
	// rendering.  

	m_pRenderer11->pImmPipeline->ClearRenderTargets();
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.SetRenderTarget( 0, m_RenderTarget->m_iResourceRTV );
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.SetDepthStencilTarget( m_DepthTarget->m_iResourceDSV );
	m_pRenderer11->pImmPipeline->ApplyRenderTargets();

	m_Effect.m_iVertexShader =
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"RotatingCube.hlsl" ),
		std::wstring( L"VSMain" ),
		std::wstring( L"vs_4_0" ),
		true);

	m_Effect.m_iGeometryShader =
		m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"RotatingCube.hlsl" ),
		std::wstring( L"GSMain" ),
		std::wstring( L"gs_4_0" ),
		true);

	m_Effect.m_iPixelShader =
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"RotatingCube.hlsl" ),
		std::wstring( L"PSMain" ),
		std::wstring( L"ps_4_0" ),
		true);

	m_pRenderer11->GetShader(m_Effect.m_iVertexShader)->PrintShaderDetails();

	// just use default states

	DepthStencilStateConfigDX11 dsConfig;
	int iDepthStencilState = m_pRenderer11->CreateDepthStencilState( &dsConfig );
	if ( iDepthStencilState == -1 ) {
		Log::Get().Write( L"Failed to create light depth stencil state" );
		assert(false);
	}

	BlendStateConfigDX11 blendConfig;
	int iBlendState = m_pRenderer11->CreateBlendState( &blendConfig );
	if ( iBlendState == -1 ) {
		Log::Get().Write( L"Failed to create light blend state" );
		assert(false);
	}

	RasterizerStateConfigDX11 rsConfig;
	rsConfig.CullMode = D3D11_CULL_BACK;
	//rsConfig.FillMode = D3D11_FILL_WIREFRAME;
	int iRasterizerState = m_pRenderer11->CreateRasterizerState( &rsConfig );
	if ( iRasterizerState == -1 ) {
		Log::Get().Write( L"Failed to create light rasterizer state" );
		assert(false);
	}

	m_Effect.m_iBlendState = iBlendState;
	m_Effect.m_iDepthStencilState = iDepthStencilState;
	m_Effect.m_iRasterizerState = iRasterizerState;
	m_Effect.m_uStencilRef = iDepthStencilState;

	// create the vertex buffer layout descriptor (this is usually done by GeometryDX11)

	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	TArray<D3D11_INPUT_ELEMENT_DESC> layout;
	layout.add(desc[0]);
	layout.add(desc[1]);
	m_VertexLayout = m_pRenderer11->CreateInputLayout(layout, m_Effect.m_iVertexShader);
	if (m_VertexLayout == -1) {
		Log::Get().Write( L"Failed to create vertex layout" );
		assert(false);
	}

	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( width );
	viewport.Height = static_cast< float >( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	int ViewPort = m_pRenderer11->CreateViewPort( viewport );
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.SetViewportCount( 1 );
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.SetViewport( 0, ViewPort );

	return( true );
}

//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	if ( m_pRenderer11 )
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	if ( m_pWindow )
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
	}
}

//--------------------------------------------------------------------------------
void App::Initialize()
{
	// create the vertex buffer resource (this is usually done by GeometryDX11)
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
			{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ) },
		};
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		BufferConfigDX11 vbConfig;
		vbConfig.SetDefaultVertexBuffer(8 * sizeof(Vertex), false);
		m_pVertexBuffer = m_pRenderer11->CreateVertexBuffer( &vbConfig, &data );
		if( m_pVertexBuffer->m_iResource == -1 ) {
			Log::Get().Write( L"Failed to create vertex buffer" );   
			assert(false);
		}
	}

	// create the index buffer resource (this is usually done by GeometryDX11)
	{
		UINT indices[] =
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		BufferConfigDX11 vbConfig;
		vbConfig.SetDefaultIndexBuffer(sizeof(UINT) * 36, false);
		m_pIndexBuffer = m_pRenderer11->CreateIndexBuffer(&vbConfig, &data);
		if( m_pIndexBuffer->m_iResource == -1 ) {
			Log::Get().Write( L"Failed to create index buffer" );   
			assert(false);
		}
	}

	// Setup the "camera"

	// Create the world matrix

	m_WorldMatrix = Matrix4f::Identity();

	// Create the view matrix

	XMVECTOR Eye = XMVectorSet( 0.0f, 1.0f, -5.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	// not recommended, just to demonstrate interoperation with xnamath
	m_ViewMatrix = *((Matrix4f*)&XMMatrixLookAtLH(Eye, At, Up));

	// Create the projection matrix

	// not recommended, just to demonstrate interoperation with xnamath
	m_ProjMatrix = *((Matrix4f*)&XMMatrixPerspectiveFovLH( XM_PIDIV2, (FLOAT)m_pWindow->GetWidth() / (FLOAT)m_pWindow->GetHeight(), 0.01f, 100.0f ));

	// Engine will automatically keep all concat'd matrices up to date

	m_pRenderer11->m_pParamMgr->SetWorldMatrixParameter(&m_WorldMatrix);
	m_pRenderer11->m_pParamMgr->SetViewMatrixParameter(&m_ViewMatrix);
	m_pRenderer11->m_pParamMgr->SetProjMatrixParameter(&m_ProjMatrix);
}

//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );

	// Clear the window

	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ), 1.0f );

	// Update the object position in worldspace

	static DWORD dwTimeStart = 0;
	static float t = 0.0f;
	DWORD dwTimeCur = GetTickCount();
	if( dwTimeStart == 0 )
		dwTimeStart = dwTimeCur;
	t = ( dwTimeCur - dwTimeStart ) / 1000.0f;

	m_WorldMatrix = Matrix4f::RotationMatrixY(t) * Matrix4f::RotationMatrixX(t);

	m_pRenderer11->m_pParamMgr->SetWorldMatrixParameter(&m_WorldMatrix);

	// Perform the actual drawing

	UINT stride = sizeof(Vertex);
	m_pRenderer11->pImmPipeline->Draw(m_Effect, m_pVertexBuffer, m_pIndexBuffer, m_VertexLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, stride, 36, m_pRenderer11->m_pParamMgr);

	// Present the results

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, std::wstring( L"BasicApplication_" ), D3DX11_IFF_BMP );
	}
}

//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}

//--------------------------------------------------------------------------------
bool App::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDown* pKeyDown = (EvtKeyDown*)pEvent;

		unsigned int key = pKeyDown->GetCharacterCode();

		return( true );
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUp* pKeyUp = (EvtKeyUp*)pEvent;

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == VK_ESCAPE ) // 'Esc' Key - Exit the application
		{
			this->RequestTermination();
			return( true );
		}
		else if ( key == 0x53 ) // 'S' Key - Save a screen shot for the next frame
		{
			m_bSaveScreenshot = true;
			return( true );
		}
		else
		{
			return( false );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( Application::HandleEvent( pEvent ) );
}

//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
