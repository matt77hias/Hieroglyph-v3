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

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "ViewDeferredRenderer.h"
#include "FirstPersonCamera.h"

#include "IParameterManager.h"
#include "SamplerParameterWriterDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "DepthStencilStateConfigDX11.h"

#include "DepthStencilViewConfigDX11.h"
#include "ShaderResourceViewConfigDX11.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------
App::App()
{
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 800, 480, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	SetMultiThreadedMode( false );

	return( true );
}
//--------------------------------------------------------------------------------
bool App::ConfigureRenderingSetup()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	ViewDeferredRenderer* pDeferredView	= new ViewDeferredRenderer( *m_pRenderer11, m_BackBuffer );
	m_pRenderView = pDeferredView;
	
	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_BackBuffer );


	m_pCamera = new FirstPersonCamera();
	m_pCamera->Spatial().SetRotation( Vector3f( 0.407f, -0.707f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 4.0f, 4.5f, -4.0f ) );

	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );

    const float nearClip = 1.0f;
    const float farClip = 15.0f;

	m_pCamera->SetProjectionParams( nearClip, farClip, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>(GLYPH_PI) / 2.0f );
	pDeferredView->SetClipPlanes( nearClip, farClip );

	m_pScene->AddCamera( m_pCamera );

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
	// Create and initialize the geometry to be rendered.
	GeometryPtr pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"Sample_Scene.ms3d" ) );
    bool success = pGeometry->ComputeTangentFrame();
    _ASSERT( success );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// We'll make 2 effects for filling the G-Buffer: one without optimizations, and one with
    m_pGBufferEffect[GBufferOptMode::OptDisabled] = new RenderEffectDX11();
	m_pGBufferEffect[GBufferOptMode::OptDisabled]->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"GBuffer.hlsl" ),
		std::wstring( L"VSMain" ),
		std::wstring( L"vs_5_0" ) ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptDisabled]->GetVertexShader() != -1 );

	m_pGBufferEffect[GBufferOptMode::OptDisabled]->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"GBuffer.hlsl" ),
		std::wstring( L"PSMain" ),
		std::wstring( L"ps_5_0" ) ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptDisabled]->GetPixelShader() != -1 );

    m_pGBufferEffect[GBufferOptMode::OptEnabled] = new RenderEffectDX11();
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
        std::wstring( L"GBuffer.hlsl" ),
        std::wstring( L"VSMainOptimized" ),
        std::wstring( L"vs_5_0" ) ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptEnabled]->GetVertexShader() != -1 );

    m_pGBufferEffect[GBufferOptMode::OptEnabled]->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
        std::wstring( L"GBuffer.hlsl" ),
        std::wstring( L"PSMainOptimized" ),
        std::wstring( L"ps_5_0" ) ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptEnabled]->GetPixelShader() != -1 );

    // Create a depth stencil state for the G-Buffer material
    // Create the depth stencil view. We'll enable depth writes and depth tests.
    // We'll also enable stencil writes, and set the stencil buffer to 1 for each pixel rendered.
    DepthStencilStateConfigDX11 dsConfig;
    dsConfig.DepthEnable = true;
    dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
    dsConfig.StencilEnable = true;
    dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsConfig.BackFace = dsConfig.FrontFace;

    m_iGBufferDSState = m_pRenderer11->CreateDepthStencilState( &dsConfig );

    if ( m_iGBufferDSState == -1 )
        Log::Get().Write( L"Failed to create G-Buffer depth stencil state" );


    // Create a rasterizer state with back-face culling enabled
    RasterizerStateConfigDX11 rsConfig;
    rsConfig.MultisampleEnable = TRUE;
    rsConfig.CullMode = D3D11_CULL_BACK;
    m_iGBufferRSState = m_pRenderer11->CreateRasterizerState( &rsConfig );

    if ( m_iGBufferRSState == -1 )
        Log::Get().Write( L"Failed to create G-Buffer rasterizer state" );

    m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iDepthStencilState = m_iGBufferDSState;
    m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_uStencilRef = 1;
    m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iRasterizerState = m_iGBufferRSState;
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iDepthStencilState = m_iGBufferDSState;
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_uStencilRef = 1;
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iRasterizerState = m_iGBufferRSState;

    m_pMaterial = MaterialPtr( new MaterialDX11() );

    // Load textures. For the diffuse map, we'll specify that we want an sRGB format so that
    // the texture data is gamma-corrected when sampled in the shader
   
	// TODO: Check if DirectXTK supports this loading information!
	//
	//D3DX11_IMAGE_LOAD_INFO loadInfo;
    //loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    m_DiffuseTexture = m_pRenderer11->LoadTexture( std::wstring( L"Hex.png" ) /*, &loadInfo*/ );
    m_NormalMap = m_pRenderer11->LoadTexture( std::wstring( L"Hex_Normal.png" ) );

    _ASSERT( m_DiffuseTexture->m_iResource != -1 );
    _ASSERT( m_NormalMap->m_iResource != -1 );

    // Set the texture parameters
    m_pMaterial->Parameters.SetShaderResourceParameter( L"DiffuseMap", m_DiffuseTexture );
	m_pMaterial->Parameters.SetShaderResourceParameter( L"NormalMap", m_NormalMap );

    // Create a sampler state
    D3D11_SAMPLER_DESC sampDesc;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.MaxAnisotropy = 16;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.MinLOD = 0.0f;
    sampDesc.MipLODBias = 0.0f;
    int samplerState = m_pRenderer11->CreateSamplerState( &sampDesc );
	m_pMaterial->Parameters.SetSamplerParameter( L"AnisoSampler", samplerState );

	// Enable the material to render the given view type
	m_pMaterial->Params[VT_GBUFFER].bRender = true;
	m_pMaterial->Params[VT_GBUFFER].pEffect = m_pGBufferEffect[GBufferOptMode::Value];


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of
	// manually manipulating it.

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry( pGeometry );
	m_pActor->GetBody()->Visual.SetMaterial( m_pMaterial );
	m_pActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 0.0f, 0.0f );

	m_pScene->AddActor( m_pActor );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can
	// then used for animation during the frame.

	m_pTimer->Update();

	// Create a series of time factors for use in the simulation.  The factors
	// are as follows:
	// x: Time in seconds since the last frame.
	// y: Current framerate, which is updated once per second.
	// z: Time in seconds since application startup.
	// w: Current frame number since application startup.

	Vector4f TimeFactors = Vector4f( m_pTimer->Elapsed(), (float)m_pTimer->Framerate(),
		m_pTimer->Runtime(), (float)m_pTimer->FrameCount() );

	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"TimeFactors" ), &TimeFactors );


	m_pMaterial->Params[VT_GBUFFER].pEffect = m_pGBufferEffect[GBufferOptMode::Value];

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pActor->GetNode()->Transform.Rotation() *= rotation;

	DrawHUD();

	// Update the scene, and then render all cameras within the scene.
	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );
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
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();

        if ( key == LightOptMode::Key )
        {
            LightOptMode::Increment();
            return true;
        }

        if ( key == GBufferOptMode::Key )
        {
            GBufferOptMode::Increment();
            return true;
        }

        if ( key == LightMode::Key )
        {
            LightMode::Increment();
            return true;
        }

        if ( key == DisplayMode::Key )
        {
            DisplayMode::Increment();
            return true;
        }

        if ( key == AAMode::Key )
        {
            AAMode::Increment();
            return true;
        }
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"DeferredRendering" ) );
}
//--------------------------------------------------------------------------------
void App::DrawHUD( )
{
    Matrix4f transform = Matrix4f::Identity();
    transform.SetTranslation( Vector3f( 30.0f, 30.0f, 0.0f ) );
    std::wstring text = L"FPS: " + ToString( m_pTimer->Framerate() );
    m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );


    float x = 30.0f;
    float y = m_iHeight - 120.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = DisplayMode::ToString();
    m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = LightMode::ToString();
	m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = GBufferOptMode::ToString();
    m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = LightOptMode::ToString();
    m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = AAMode::ToString();
    m_pTextOverlayView->WriteText( text, transform, Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
}
//--------------------------------------------------------------------------------
