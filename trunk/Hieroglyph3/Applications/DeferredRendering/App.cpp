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

#include "ParameterManagerDX11.h"
#include "SamplerParameterDX11.h"
#include "ShaderResourceParameterDX11.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------
template <typename T>
void ChangeParameter( T& parameter, int maxValue )
{
    int val = static_cast<int>( parameter );
    val = ( val + 1 ) % maxValue;
    parameter = static_cast<T>( val );
}
//--------------------------------------------------------------------------------
App::App()
{
	m_bSaveScreenshot = false;
    m_DisplayMode = Final;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// Basic event handling is supported with the EventManager class.  This is a 
	// singleton class that allows an EventListener to register which events it
	// wants to receive.

	EventManager* pEventManager = EventManager::Get( );

	// The application object wants to know about these three events, so it 
	// registers itself with the appropriate event IDs.

	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYUP, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYDOWN, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_CHAR, this );


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

    m_pRenderer11->SetMultiThreadingState( false );

	// Create the window.
	int width = 1280;
	int height = 720;

	// Create the window wrapper class instance.
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 50, 50 );
	m_pWindow->SetSize( width, height );
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

	// Create the render targets for our G-Buffer
	Texture2dConfigDX11 RTConfig;
	RTConfig.SetColorBuffer( width, height );
	RTConfig.SetFormat( DXGI_FORMAT_R32G32B32A32_FLOAT );
    RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
	for(int i = 0; i < 4; ++i)
		m_GBufferTargets.add( m_pRenderer11->CreateTexture2D( &RTConfig, NULL ) );

	// Next we create a depth buffer for depth/stencil testing, and for depth readback
	Texture2dConfigDX11 DepthTexConfig;
	DepthTexConfig.SetDepthBuffer( width, height );
	DepthTexConfig.SetFormat( DXGI_FORMAT_R24G8_TYPELESS );
	DepthTexConfig.SetBindFlags( D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE );
    
    ShaderResourceViewConfigDX11 DepthSRVConfig;
    D3D11_TEX2D_SRV SRVTex2D;
    SRVTex2D.MipLevels = 1;
    SRVTex2D.MostDetailedMip = 0;    
    DepthSRVConfig.SetTexture2D( SRVTex2D );
    DepthSRVConfig.SetFormat( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
	
    DepthStencilViewConfigDX11 DepthDSVConfig;
    D3D11_TEX2D_DSV DSVTex2D;
    DSVTex2D.MipSlice = 0;
    DepthDSVConfig.SetTexture2D( DSVTex2D );
    DepthDSVConfig.SetFormat( DXGI_FORMAT_D24_UNORM_S8_UINT );
    DepthDSVConfig.SetFlags( D3D11_DSV_READ_ONLY_STENCIL );
    
    m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthTexConfig, NULL, &DepthSRVConfig, NULL, NULL, &DepthDSVConfig );

	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( width );
	viewport.Height = static_cast< float >( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;	

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
	// Get some information about the render target before initializing.

	D3D11_TEXTURE2D_DESC desc = m_GBufferTargets[0]->m_pTexture2dConfig->GetTextureDesc();

	unsigned int ResolutionX = desc.Width;
	unsigned int ResolutionY = desc.Height;

	// Create and initialize the geometry to be rendered.  
	GeometryDX11* pGeometry = new GeometryDX11();
	pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/Sample_Scene.ms3d" ) );
    _ASSERT( pGeometry->ComputeTangentFrame() );
	pGeometry->LoadToBuffers();	
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the material for use by the entity.
	MaterialDX11* pMaterial = new MaterialDX11();

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pGBufferEffect = new RenderEffectDX11();

	pGBufferEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
		std::wstring( L"VSMain" ),
		std::wstring( L"vs_5_0" ) );
    _ASSERT( pGBufferEffect->m_iVertexShader != -1 );
	pGBufferEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/GBuffer.hlsl" ),
		std::wstring( L"PSMain" ),
		std::wstring( L"ps_5_0" ) );
    _ASSERT( pGBufferEffect->m_iPixelShader != -1 );

    // Load textures
    m_DiffuseTexture = m_pRenderer11->LoadTexture( std::wstring( L"../Data/Textures/Hex.png" ) );
    m_NormalMap = m_pRenderer11->LoadTexture( std::wstring( L"../Data/Textures/Hex_Normal.png" ) );
    
    // Set the texture parameters
    ShaderResourceParameterDX11* pDiffuseParam = new ShaderResourceParameterDX11();
    pDiffuseParam->SetParameterData( &m_DiffuseTexture->m_iResourceSRV );
    pDiffuseParam->SetName( std::wstring( L"DiffuseMap" ) );
    pMaterial->AddRenderParameter( pDiffuseParam );

    ShaderResourceParameterDX11* pNormalMapParam = new ShaderResourceParameterDX11();
    pNormalMapParam->SetParameterData( &m_NormalMap->m_iResourceSRV );
    pNormalMapParam->SetName( std::wstring( L"NormalMap" ) );
    pMaterial->AddRenderParameter( pNormalMapParam );

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
    SamplerParameterDX11* pSamplerParam = new SamplerParameterDX11();
    pSamplerParam->SetParameterData( &samplerState );
    pSamplerParam->SetName( std::wstring( L"AnisoSampler" ) );
    pMaterial->AddRenderParameter( pSamplerParam );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_GBUFFER].bRender = true;
	pMaterial->Params[VT_GBUFFER].pEffect = pGBufferEffect;    

	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.
	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.407f, -0.707f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 4.0f, 4.5f, -4.0f );

	m_pGBufferView = new ViewGBuffer( *m_pRenderer11, m_GBufferTargets, m_DepthTarget );

	m_pCamera->SetCameraView( m_pGBufferView );
	m_pCamera->SetProjectionParams( 0.1f, 500.0f, (float)D3DX_PI / 2.0f, (float)ResolutionX / (float)ResolutionY );

    m_pLightsView = new ViewLights( *m_pRenderer11, m_BackBuffer, m_DepthTarget, m_GBufferTargets );

    // Add some lights
    Light light;
    light.Type = Point;

    for ( int x = -6; x <= 6; x += 3 ) 
    {
        for ( int y = 1; y <= 11; y += 3 )
        {
            for ( int z = -6; z <= 6; z += 3 )
            {
                light.Position = Vector3f( static_cast<float>( x ), static_cast<float>( y ), static_cast<float>( z ) );

                float r = ( x + 6.0f ) / 11.0f;
                float g = ( y - 1.0f ) / 9.0f;
                float b = ( z + 6.0f ) / 11.0f;
                light.Color = Vector3f( r, g, b ) * 1.5f;
                m_pLightsView->AddLight( light );
            }
        }
    }

    // Bind the light view to the camera entity
    m_pLightsView->SetRoot( m_pCamera->GetNode() );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( pGeometry );
	m_pEntity->SetMaterial( pMaterial, false );
	m_pEntity->Position() = Vector3f( 0.0f, 0.0f, 0.0f );  

	m_pNode->AttachChild( m_pEntity );

	m_pScene->AddEntity( m_pNode );
	m_pScene->AddCamera( m_pCamera );

    m_SpriteRenderer.Initialize();
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

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart() );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pNode->Rotation() *= rotation;

	// Update the scene, and then render all cameras within the scene.	
	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );

    // Render the light pass
    m_pLightsView->PreDraw( m_pRenderer11 );
    m_pRenderer11->ProcessRenderViewQueue();

    if ( m_DisplayMode != Final )
    {
	    PipelineManagerDX11* pImmPipeline = m_pRenderer11->pImmPipeline;
        ParameterManagerDX11* pParams = m_pRenderer11->m_pParamMgr;
        pImmPipeline->ClearRenderTargets();
	    pImmPipeline->BindRenderTargets( 0, m_BackBuffer );    
        pImmPipeline->ApplyRenderTargets();
        pImmPipeline->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

        ResourcePtr target;
        if ( m_DisplayMode == GBuffer )
        {
            Matrix4f mat = Matrix4f::Identity();
            mat = Matrix4f::ScaleMatrix( 0.5f ) *  Matrix4f::TranslationMatrix(0, 0, 0);
            m_SpriteRenderer.Render( pImmPipeline, pParams, m_GBufferTargets[0], mat );

            mat = Matrix4f::ScaleMatrix( 0.5f ) *  Matrix4f::TranslationMatrix(640, 0, 0);
            m_SpriteRenderer.Render( pImmPipeline, pParams, m_GBufferTargets[1], mat );

            mat = Matrix4f::ScaleMatrix( 0.5f ) *  Matrix4f::TranslationMatrix(0, 360, 0);
            m_SpriteRenderer.Render( pImmPipeline, pParams, m_GBufferTargets[2], mat );

            mat = Matrix4f::ScaleMatrix( 0.5f ) *  Matrix4f::TranslationMatrix(640, 360, 0);
            m_SpriteRenderer.Render( pImmPipeline, pParams, m_GBufferTargets[3], mat );
        }
        else
        {
            ResourcePtr target = m_GBufferTargets[ m_DisplayMode - Normals ];
            m_SpriteRenderer.Render( pImmPipeline, pParams, target, Matrix4f::Identity() );
        }
        
    }

	// Perform the rendering and presentation for the window.
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, std::wstring( L"DeferredRendering_" ), D3DX11_IFF_BMP );
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

        if ( key == 'V' )
        {
            ChangeParameter( m_DisplayMode, NumDisplayModes );
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


	return( false );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
//--------------------------------------------------------------------------------