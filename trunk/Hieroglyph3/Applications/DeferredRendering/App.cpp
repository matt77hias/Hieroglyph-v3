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
#include "DepthStencilStateConfigDX11.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------
Vector3f Lerp( const Vector3f& x, const Vector3f& y, const Vector3f& s )
{
    return x + s * ( y - x );
}
//--------------------------------------------------------------------------------
App::App() : m_vpWidth( 1280 ), m_vpHeight( 720 )
{
	m_bSaveScreenshot = false;    
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

    // Create render targets for all AA modes
    for ( int aaMode = 0; aaMode < AAMode::NumSettings; ++aaMode )
    {
        int rtWidth = m_vpWidth;
        int rtHeight = m_vpHeight;
        if ( aaMode == AAMode::SSAA )
        {
            rtWidth *= 2;
            rtHeight *= 2;
        }

        DXGI_SAMPLE_DESC sampleDesc;
        sampleDesc.Count = aaMode == AAMode::MSAA ? 4 : 1;
        sampleDesc.Quality = 0;

	    // Create the render targets for our unoptimized G-Buffer, which just uses 
        // 32-bit floats for everything
	    Texture2dConfigDX11 RTConfig;
	    RTConfig.SetColorBuffer( rtWidth, rtHeight );
	    RTConfig.SetFormat( DXGI_FORMAT_R32G32B32A32_FLOAT );
        RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
        RTConfig.SetSampleDesc( sampleDesc );
	    for(int i = 0; i < 4; ++i)
            m_GBuffer[GBufferOptMode::OptDisabled][aaMode].add( m_pRenderer11->CreateTexture2D( &RTConfig, NULL ) );

        // Create the render targets for our optimized G-Buffer

        // 2-component signed normalized format for spheremap-encoded normals
        RTConfig.SetFormat( DXGI_FORMAT_R16G16_SNORM );
        m_GBuffer[GBufferOptMode::OptEnabled][aaMode].add( m_pRenderer11->CreateTexture2D( &RTConfig, NULL ) );

        // 3-component 10-bit unsigned normalized format for diffuse albedo
        RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
        m_GBuffer[GBufferOptMode::OptEnabled][aaMode].add( m_pRenderer11->CreateTexture2D( &RTConfig, NULL ) );

        // 4-component 8-bit unsigned normalized format for specular albedo and power
        RTConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
        m_GBuffer[GBufferOptMode::OptEnabled][aaMode].add( m_pRenderer11->CreateTexture2D( &RTConfig, NULL ) );

        // We need one last render target for the final image
        RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
        m_FinalTarget[aaMode] = m_pRenderer11->CreateTexture2D( &RTConfig, NULL );

	    // Next we create a depth buffer for depth/stencil testing
	    Texture2dConfigDX11 DepthTexConfig;
	    DepthTexConfig.SetDepthBuffer( rtWidth, rtHeight );	
        DepthTexConfig.SetFormat( DXGI_FORMAT_D24_UNORM_S8_UINT );	
        DepthTexConfig.SetBindFlags( D3D11_BIND_DEPTH_STENCIL );
        DepthTexConfig.SetSampleDesc( sampleDesc );
    	
        DepthStencilViewConfigDX11 DepthDSVConfig;
        D3D11_TEX2D_DSV DSVTex2D;
        DSVTex2D.MipSlice = 0;
        DepthDSVConfig.SetTexture2D( DSVTex2D );
        DepthDSVConfig.SetFormat( DXGI_FORMAT_D24_UNORM_S8_UINT );                

        if ( aaMode == AAMode::MSAA )         
            DepthDSVConfig.SetViewDimensions( D3D11_DSV_DIMENSION_TEXTURE2DMS );
        else
            DepthDSVConfig.SetViewDimensions( D3D11_DSV_DIMENSION_TEXTURE2D );           
        
        m_DepthTarget[aaMode] = m_pRenderer11->CreateTexture2D( &DepthTexConfig, NULL, NULL, NULL, NULL, &DepthDSVConfig );

        // Now we need to create a another depth-stencil buffer that we can copy to,
        // for depth readback
        DepthTexConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
        DepthTexConfig.SetFormat( DXGI_FORMAT_R24G8_TYPELESS );

        ShaderResourceViewConfigDX11 DepthSRVConfig;
        D3D11_TEX2D_SRV SRVTex2D;
        SRVTex2D.MipLevels = 1;
        SRVTex2D.MostDetailedMip = 0;    
        DepthSRVConfig.SetTexture2D( SRVTex2D );
        DepthSRVConfig.SetFormat( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );

        if ( aaMode == AAMode::MSAA )         
            DepthSRVConfig.SetViewDimensions( D3D11_SRV_DIMENSION_TEXTURE2DMS );
        else
            DepthSRVConfig.SetViewDimensions( D3D11_SRV_DIMENSION_TEXTURE2D ); 

        m_DepthTargetCopy[aaMode] = m_pRenderer11->CreateTexture2D( &DepthTexConfig, NULL, &DepthSRVConfig, NULL, NULL, NULL );

        // Create a view port to use on the scene.  This basically selects the 
        // entire floating point area of the render target.
        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast< float >( rtWidth );
        viewport.Height = static_cast< float >( rtHeight );
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        m_iViewport[aaMode] = m_pRenderer11->CreateViewPort( viewport );
    }

    // Create a render target for MSAA resolve
    Texture2dConfigDX11 RTConfig;
    RTConfig.SetColorBuffer( m_vpWidth, m_vpHeight);
    RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
    RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
    m_ResolveTarget = m_pRenderer11->CreateTexture2D( &RTConfig, NULL );

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
    ShaderResourceParameterDX11* pDiffuseParam = new ShaderResourceParameterDX11();
    pDiffuseParam->SetParameterData( &m_DiffuseTexture->m_iResourceSRV );
    pDiffuseParam->SetName( std::wstring( L"DiffuseMap" ) );
    m_pMaterial->AddRenderParameter( pDiffuseParam );

    ShaderResourceParameterDX11* pNormalMapParam = new ShaderResourceParameterDX11();
    pNormalMapParam->SetParameterData( &m_NormalMap->m_iResourceSRV );
    pNormalMapParam->SetName( std::wstring( L"NormalMap" ) );
    m_pMaterial->AddRenderParameter( pNormalMapParam );

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
    m_pMaterial->AddRenderParameter( pSamplerParam );

	// Enable the material to render the given view type
	m_pMaterial->Params[VT_GBUFFER].bRender = true;	

	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.
	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.407f, -0.707f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 4.0f, 4.5f, -4.0f );

	m_pGBufferView = new ViewGBuffer( *m_pRenderer11 );

	m_pCamera->SetCameraView( m_pGBufferView );

    const float nearClip = 1.0f;
    const float farClip = 15.0f;
	m_pCamera->SetProjectionParams( nearClip, farClip, (float)D3DX_PI / 2.0f, (float)m_vpWidth / (float)m_vpHeight );

    m_pLightsView = new ViewLights( *m_pRenderer11 );    

    // Bind the light view to the camera entity
    m_pLightsView->SetEntity( m_pCamera->GetBody() );
    m_pLightsView->SetRoot( m_pCamera->GetNode() );
    m_pLightsView->SetProjMatrix( m_pGBufferView->GetProjMatrix() );
    m_pLightsView->SetClipPlanes( nearClip, farClip );

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

    SetupViews();

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

    // Render to the backbuffer
    PipelineManagerDX11* pImmPipeline = m_pRenderer11->pImmPipeline;
    ParameterManagerDX11* pParams = m_pRenderer11->m_pParamMgr;
    pImmPipeline->ClearRenderTargets();
    pImmPipeline->BindRenderTargets( 0, m_BackBuffer );    
    pImmPipeline->ApplyRenderTargets();
    pImmPipeline->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

    TArray<ResourcePtr>& gBuffer = m_GBuffer[GBufferOptMode::Value][AAMode::Value];
    float scaleFactor = AAMode::Value == AAMode::SSAA ? 0.5f : 1.0f;
              
    if ( DisplayMode::Value == DisplayMode::Final )
    {
        ResourcePtr target = m_FinalTarget[AAMode::Value];
        if ( AAMode::Value == AAMode::MSAA )
        {
            // Need to resolve the MSAA target before we can render it
            pImmPipeline->ResolveSubresource( m_ResolveTarget, 0, target, 0, DXGI_FORMAT_R10G10B10A2_UNORM );
            target = m_ResolveTarget;
        }
        
        m_SpriteRenderer.Render( pImmPipeline, pParams, target, Matrix4f::ScaleMatrix( scaleFactor ) );
    }
    else
    {
        if ( AAMode::Value == AAMode::MSAA )
        {
            std::wstring text = L"Unable to view G-Buffers while MSAA is enabled";
            Matrix4f mat = Matrix4f::Identity();
            mat.SetTranslation( Vector3f( 500.0f, 350.0f, 0.0f ) );
            m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), mat );
        }
        else
        {
            if ( DisplayMode::Value == DisplayMode::GBuffer )
            {
                Matrix4f mat = Matrix4f::Identity();
                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(0, 0, 0);
                m_SpriteRenderer.Render( pImmPipeline, pParams, gBuffer[0], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(640, 0, 0);
                m_SpriteRenderer.Render( pImmPipeline, pParams, gBuffer[1], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(0, 360, 0);
                m_SpriteRenderer.Render( pImmPipeline, pParams, gBuffer[2], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(640, 360, 0);

                if ( GBufferOptMode::Enabled() )
                    m_SpriteRenderer.Render( pImmPipeline, pParams, m_DepthTarget[AAMode::Value], mat );
                else
                    m_SpriteRenderer.Render( pImmPipeline, pParams, gBuffer[3], mat );
            }
            else
            {
                ResourcePtr target;                
                if ( GBufferOptMode::Enabled() && DisplayMode::Value == DisplayMode::Position )
                    target = m_DepthTarget[AAMode::Value];
                else
                    target = gBuffer[ DisplayMode::Value - DisplayMode::Normals ];
                
                m_SpriteRenderer.Render( pImmPipeline, pParams, target, Matrix4f::ScaleMatrix( scaleFactor ) );
            }
        }
    }            

    DrawHUD( );

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


	return( false );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
//--------------------------------------------------------------------------------
void App::DrawHUD( )
{
    PipelineManagerDX11* pImmPipeline = m_pRenderer11->pImmPipeline;
    ParameterManagerDX11* pParams = m_pRenderer11->m_pParamMgr;        

    Matrix4f transform = Matrix4f::Identity();
    transform.SetTranslation( Vector3f( 30.0f, 30.0f, 0.0f ) );
    std::wstring text = L"FPS: " + ToString( m_pTimer->Framerate() );
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );

    float x = 30.0f;
    float y = 600.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = DisplayMode::ToString();
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = LightMode::ToString();
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );
    text = GBufferOptMode::ToString();
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );    
    text = LightOptMode::ToString();
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );

    y += 20.0f;
    transform.SetTranslation( Vector3f( x, y, 0.0f ) );    
    text = AAMode::ToString();
    m_SpriteRenderer.RenderText( pImmPipeline, pParams, m_Font, text.c_str(), transform );
}
//--------------------------------------------------------------------------------
void App::SetupViews( )
{
    // Set the lights to render    
    Light light;
    light.Type = Point;

    const int cubeSize = 3 + LightMode::Value * 2;
    const int cubeMin = -(cubeSize / 2);
    const int cubeMax = cubeSize / 2;

    const Vector3f minExtents ( -4.0f, 1.0f, -4.0f );
    const Vector3f maxExtents ( 4.0f, 11.0f, 4.0f );
    const Vector3f minColor ( 1.0f, 0.0f, 0.0f );
    const Vector3f maxColor ( 0.0f, 1.0f, 1.0f );

    for ( int x = cubeMin; x <= cubeMax; x++ ) 
    {
        for ( int y = cubeMin; y <= cubeMax; y++ )
        {
            for ( int z = cubeMin; z <= cubeMax; z++ )
            {
                Vector3f lerp;
                lerp.x = static_cast<float>( x - cubeMin ) / ( cubeSize - 1 ); 
                lerp.y = static_cast<float>( y - cubeMin ) / ( cubeSize - 1 );
                lerp.z = static_cast<float>( z - cubeMin ) / ( cubeSize - 1 );

                light.Position = Lerp( minExtents, maxExtents, lerp );
                light.Color = Lerp( minColor, maxColor, lerp ) * 1.5f;
                m_pLightsView->AddLight(light);
            }
        }
    }

    int vpWidth = m_vpWidth;
    int vpHeight = m_vpHeight;
    if ( AAMode::Value == AAMode::SSAA )
    {
        vpWidth *= 2;
        vpHeight *= 2;
    }

    // Set the GBuffer targets, and the material effect
    m_pGBufferView->SetTargets( m_GBuffer[GBufferOptMode::Value][AAMode::Value], 
                                m_DepthTarget[AAMode::Value],
                                m_iViewport[AAMode::Value] );
    m_pLightsView->SetTargets( m_GBuffer[GBufferOptMode::Value][AAMode::Value], 
                               m_FinalTarget[AAMode::Value],
                               m_DepthTarget[AAMode::Value], 
                               m_DepthTargetCopy[AAMode::Value],
                               m_iViewport[AAMode::Value], vpWidth, vpHeight );

    m_pMaterial->Params[VT_GBUFFER].pEffect = m_pGBufferEffect[GBufferOptMode::Value];
}
//--------------------------------------------------------------------------------