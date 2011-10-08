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

#include "ViewAmbientOcclusion.h"

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
App::App() : m_vpWidth( 1024 ), m_vpHeight( 576 )
{
	m_bSaveScreenshot = false;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

    //m_pRenderer11->SetMultiThreadingState( false );

	// Create the window wrapper class instance.
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 50, 50 );
	m_pWindow->SetSize( m_vpWidth, m_vpHeight );
	m_pWindow->SetCaption( std::wstring( L"Direct3D 11 Window" ) );
	m_pWindow->Initialize();

	// Create a swap chain for the window.
	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_pWindow->SetSwapChain( m_pRenderer11->CreateSwapChain( &Config ) );

	// We'll keep a copy of the swap chain's render target index to
	// use later.
	m_BackBuffer = m_pRenderer11->GetSwapChainResource( m_pWindow->GetSwapChain() );


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
	// Create and initialize the geometry to be rendered.
	GeometryDX11* pGeometry = new GeometryDX11();
	pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/Sample_Scene.ms3d" ) );
    bool success = pGeometry->ComputeTangentFrame();
    _ASSERT( success );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// We'll make 2 effects for filling the G-Buffer: one without optimizations, and one with
    m_pGBufferEffect[GBufferOptMode::OptDisabled] = new RenderEffectDX11();
	m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iVertexShader =
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
		std::wstring( L"VSMain" ),
		std::wstring( L"vs_5_0" ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iVertexShader != -1 );
	m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iPixelShader =
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
		std::wstring( L"PSMain" ),
		std::wstring( L"ps_5_0" ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptDisabled]->m_iPixelShader != -1 );

    m_pGBufferEffect[GBufferOptMode::OptEnabled] = new RenderEffectDX11();
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iVertexShader =
        m_pRenderer11->LoadShader( VERTEX_SHADER,
        std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
        std::wstring( L"VSMainOptimized" ),
        std::wstring( L"vs_5_0" ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iVertexShader != -1 );
    m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iPixelShader =
        m_pRenderer11->LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
        std::wstring( L"PSMainOptimized" ),
        std::wstring( L"ps_5_0" ) );
    _ASSERT( m_pGBufferEffect[GBufferOptMode::OptEnabled]->m_iPixelShader != -1 );

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

    m_pMaterial = new MaterialDX11();

    // Load textures. For the diffuse map, we'll specify that we want an sRGB format so that
    // the texture data is gamma-corrected when sampled in the shader
    D3DX11_IMAGE_LOAD_INFO loadInfo;
    loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    m_DiffuseTexture = m_pRenderer11->LoadTexture( std::wstring( L"../Data/Textures/Hex.png" ), &loadInfo );
    m_NormalMap = m_pRenderer11->LoadTexture( std::wstring( L"../Data/Textures/Hex_Normal.png" ) );

    _ASSERT( m_DiffuseTexture->m_iResource != -1 );
    _ASSERT( m_NormalMap->m_iResource != -1 );

    // Set the texture parameters
	ShaderResourceParameterWriterDX11* pDiffuseParam = new ShaderResourceParameterWriterDX11();
	pDiffuseParam->SetRenderParameterRef(
		m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"DiffuseMap" ) ) );
    pDiffuseParam->SetValue( m_DiffuseTexture );
    m_pMaterial->Parameters.AddRenderParameter( pDiffuseParam );

    ShaderResourceParameterWriterDX11* pNormalMapParam = new ShaderResourceParameterWriterDX11();
	pNormalMapParam->SetRenderParameterRef(
		m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"NormalMap" ) ) );
    pNormalMapParam->SetValue( m_NormalMap );
    m_pMaterial->Parameters.AddRenderParameter( pNormalMapParam );

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

    // Create a sampler state parameter
    SamplerParameterWriterDX11* pSamplerParam = new SamplerParameterWriterDX11();
    pSamplerParam->SetRenderParameterRef(
		(RenderParameterDX11*)m_pRenderer11->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"AnisoSampler" ) ) );
	pSamplerParam->SetValue( samplerState );
    m_pMaterial->Parameters.AddRenderParameter( pSamplerParam );

	// Enable the material to render the given view type
	m_pMaterial->Params[VT_GBUFFER].bRender = true;

	// Create the camera, and the render view that will produce an image of the
	// from the camera's point of view of the scene.
	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.407f, -0.707f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 4.0f, 4.5f, -4.0f );

	m_pDeferredView = new ViewDeferredRenderer( *m_pRenderer11, m_BackBuffer );
	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_BackBuffer );
	
	m_pCamera->SetCameraView( m_pDeferredView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );

    const float nearClip = 1.0f;
    const float farClip = 15.0f;
	m_pCamera->SetProjectionParams( nearClip, farClip, (float)m_vpWidth / (float)m_vpHeight , (float)D3DX_PI / 2.0f);

    // Bind the light view to the camera entity
    m_pDeferredView->SetEntity( m_pCamera->GetBody() );
    m_pDeferredView->SetRoot( m_pCamera->GetNode() );
    m_pDeferredView->SetClipPlanes( nearClip, farClip );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( pGeometry );
	m_pEntity->SetMaterial( m_pMaterial, false );
	m_pEntity->Position() = Vector3f( 0.0f, 0.0f, 0.0f );

	m_pNode->AttachChild( m_pEntity );

	m_pScene->AddEntity( m_pNode );
	m_pScene->AddCamera( m_pCamera );

    m_SpriteRenderer.Initialize();
    m_Font.Initialize( L"Arial", 14, 0, true );
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

    //SetupViews();
	m_pMaterial->Params[VT_GBUFFER].pEffect = m_pGBufferEffect[GBufferOptMode::Value];

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pNode->Rotation() *= rotation;

	DrawHUD();

	// Update the scene, and then render all cameras within the scene.
	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );



	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, GetName(), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pEntity );

	SAFE_DELETE( m_pNode );

	SAFE_DELETE( m_pCamera );

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
    float y = m_vpHeight - 120.0f;
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
