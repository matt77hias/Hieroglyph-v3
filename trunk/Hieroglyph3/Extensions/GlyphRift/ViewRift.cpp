//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "ViewRift.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "BoundsVisualizerActor.h"
#include "Kernel/OVR_Math.h"
#include "ShaderResourceViewDX11.h"
#include "RenderTargetViewDX11.h"
#include "SwapChainDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewRift::ViewRift( RiftHMDPtr hmd, ResourcePtr RenderTarget, int SwapChain  )
{
	m_pHmd = hmd;

	// Get the desired texture sizes for the render targets.  Note that these are
	// typically larger than the resolution of the display panel itself.

	ovrHmdDesc hmdDesc = m_pHmd->GetHMDDesc();
	OVR::Sizei tex0Size = ovrHmd_GetFovTextureSize( m_pHmd->GetHMD(), ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
	OVR::Sizei tex1Size = ovrHmd_GetFovTextureSize( m_pHmd->GetHMD(), ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

	unsigned int width = max(tex0Size.w,tex1Size.w);
	unsigned int height = max(tex0Size.h,tex1Size.h);

	// Create a single depth buffer, which will be used in rendering the scene
	// into each eye.

	Texture2dConfigDX11 depthConfig;
	depthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = RendererDX11::Get()->CreateTexture2D( &depthConfig, 0 );

	// Create two render targets - one for each eye.  These are created as both
	// render targets and shader resources so that they can be used by the Oculus
	// SDK to do the distortion rendering at the end of the frame.

	Texture2dConfigDX11 renderConfig;
	renderConfig.SetWidth( width );
	renderConfig.SetHeight( height );
	renderConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
	renderConfig.SetBindFlags( D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );
	m_LeftTarget = RendererDX11::Get()->CreateTexture2D( &renderConfig, 0 );
	m_RightTarget = RendererDX11::Get()->CreateTexture2D( &renderConfig, 0 );

	// Read back the actual size of the texture created for use in the other
	// setup functions below.

	D3D11_TEXTURE2D_DESC desc = RendererDX11::Get()->GetTexture2DByIndex( m_DepthTarget->m_iResource )->GetActualDescription();

	// Create two view ports to use on the scene - one for each eye.  Since they
	// are the exact same, we can just assign them to one another.

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( desc.Width );
	viewport.Height = static_cast< float >( desc.Height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_LeftEyeViewport = RendererDX11::Get()->CreateViewPort( viewport );
	m_RightEyeViewport = m_LeftEyeViewport;

	// Fill in the Oculus SDK version of the viewports to be used in their
	// distortion rendering.

	m_pHmd->eyeViewports[0] = OVR::Recti( 0, 0, desc.Width, desc.Height );
	m_pHmd->eyeViewports[1] = OVR::Recti( 0, 0, desc.Width, desc.Height );

	// Set up the D3D texture data, including ID3D11Texture2D and 
	// ID3D11ShaderResourceView pointers.  These will be used in distortion
	// rendering.

	m_pHmd->eyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
	m_pHmd->eyeTexture[0].D3D11.Header.TextureSize = OVR::Sizei( desc.Width, desc.Height );
	m_pHmd->eyeTexture[0].D3D11.Header.RenderViewport = m_pHmd->eyeViewports[ovrEye_Left];
	m_pHmd->eyeTexture[0].D3D11.pTexture = static_cast<ID3D11Texture2D*>( RendererDX11::Get()->GetTexture2DByIndex( m_LeftTarget->m_iResource )->GetResource() );
	m_pHmd->eyeTexture[0].D3D11.pSRView = RendererDX11::Get()->GetShaderResourceViewByIndex( m_LeftTarget->m_iResourceSRV ).GetSRV();
			
	// Right eye uses the same texture, but different rendering viewport.
	m_pHmd->eyeTexture[1].D3D11.Header.API = ovrRenderAPI_D3D11;
	m_pHmd->eyeTexture[1].D3D11.Header.TextureSize = OVR::Sizei( desc.Width, desc.Height );
	m_pHmd->eyeTexture[1].D3D11.Header.RenderViewport = m_pHmd->eyeViewports[ovrEye_Right];
	m_pHmd->eyeTexture[1].D3D11.pTexture = static_cast<ID3D11Texture2D*>( RendererDX11::Get()->GetTexture2DByIndex( m_RightTarget->m_iResource )->GetResource() );
	m_pHmd->eyeTexture[1].D3D11.pSRView = RendererDX11::Get()->GetShaderResourceViewByIndex( m_RightTarget->m_iResourceSRV ).GetSRV();


	// Get the actual size of the swap chain we created, and use that in the 
	// Oculus SDK setup functions below.  This will be used when mapping the
	// eye textures onto the backbuffer, so the sizes should match.

	DXGI_SWAP_CHAIN_DESC swapDesc;
	RendererDX11::Get()->GetSwapChainByIndex( SwapChain )->GetSwapChain()->GetDesc( &swapDesc );

	OVR::Sizei size( swapDesc.BufferDesc.Width, swapDesc.BufferDesc.Height );
	
	// The rendering configuration requires the raw API pointers used for
	// rendering.  This seems like a bad practice, but they manage all of the
	// distortion rendering 'for free' so it is used here.

	ovrD3D11Config cfg;
	cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
	cfg.D3D11.Header.RTSize = size;
	cfg.D3D11.Header.Multisample = 1;
	cfg.D3D11.pDevice = RendererDX11::Get()->GetDevice();
	cfg.D3D11.pDeviceContext = RendererDX11::Get()->pImmPipeline->GetDeviceContext();
	cfg.D3D11.pBackBufferRT = RendererDX11::Get()->GetRenderTargetViewByIndex( RenderTarget->m_iResourceRTV ).GetRTV();
	cfg.D3D11.pSwapChain = RendererDX11::Get()->GetSwapChainByIndex( SwapChain )->GetSwapChain();

	const unsigned   DistortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

	if ( !ovrHmd_ConfigureRendering( m_pHmd->GetHMD(), &cfg.Config, DistortionCaps, m_pHmd->eyeFov, m_pHmd->eyeRenderDesc ) ) {
		throw std::invalid_argument( "Couldn't configure rift HMD rendering!" );
	}
	
	// Set up the projection matrices according to the FOV for each eye.  These
	// can be calculated once and then reused as long as the FOV is not changed
	// during runtime.

	leftProj = m_pHmd->GetPerspectiveFov( 0, 0.01f, 10000.0f );
	rightProj = m_pHmd->GetPerspectiveFov( 1, 0.01f, 10000.0f );
}
//--------------------------------------------------------------------------------
ViewRift::~ViewRift()
{
}
//--------------------------------------------------------------------------------
void ViewRift::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewRift::QueuePreTasks( RendererDX11* pRenderer )
{
	if ( m_pEntity != NULL )
	{
		Matrix4f view = m_pEntity->GetView();
		SetViewMatrix( view );
	}

	// Queue this view into the renderer for processing.
	pRenderer->QueueTask( this );

	if ( m_pScene )
	{
		// Run through the graph and pre-render the entities
		m_pScene->PreRender( pRenderer, VT_PERSPECTIVE );
	}
}
//--------------------------------------------------------------------------------
void ViewRift::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	// Here we assume that the ovrHmd_BeginFrame function has already been called
	// by the application at the beginning of the rendering phase.  The 
	// application will also call the ovrHmd_EndFrame function after all rendering
	// is completed.

	// TODO: This should be converted to a function that can just be called on 
	//       each eye.  That will eliminate the code duplication, and make it more
	//       maintainable.

	if ( m_pScene )
	{
		// Set up the view matrices for each eye
		Matrix4f centerView = GetViewMatrix();
		leftView = centerView * Matrix4f::TranslationMatrix( m_pHmd->eyeRenderDesc[ovrEye_Left].ViewAdjust.x, 0.0f, 0.0f );
		rightView = centerView * Matrix4f::TranslationMatrix( m_pHmd->eyeRenderDesc[ovrEye_Right].ViewAdjust.x, 0.0f, 0.0f );


		// Render the LEFT eye first
		ovrPosef eyePose = ovrHmd_BeginEyeRender(m_pHmd->GetHMD(), ovrEye_Left);

		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_LeftTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();
		pPipelineManager->ClearBuffers( m_vColor, 1.0f );

		// Configure the desired viewports in this pipeline
		SetViewPort( m_LeftEyeViewport );
		ConfigureViewports( pPipelineManager );


		// Set this view's render parameters
		SetViewMatrix( leftView );
		SetProjMatrix( leftProj );
		SetRenderParams( pParamManager );

		// Set the light parameters (currently only supporting the first light...)
		if ( m_pScene->GetLightCount() > 0 ) {
			m_pScene->GetLight( 0 )->Parameters.SetRenderParams( pParamManager );
		}

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );

		// If the debug view is enabled, then we can render some additional scene
		// related information as an overlay on this view.  Note that this is
		// accomplished without the actor being attached to the scene!
		if ( m_bDebugViewEnabled )
		{
			std::vector<Entity3D*> list;
			m_pScene->GetRoot()->GetEntities( list );
			m_pDebugVisualizer->UpdateBoundsData( list );

			m_pDebugVisualizer->GetBody()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
		}

		ovrHmd_EndEyeRender(m_pHmd->GetHMD(), ovrEye_Left, eyePose, &m_pHmd->eyeTexture[0].Texture);



		// Render the RIGHT eye second
		eyePose = ovrHmd_BeginEyeRender(m_pHmd->GetHMD(), ovrEye_Right);

		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RightTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();
		pPipelineManager->ClearBuffers( m_vColor, 1.0f );


		// Configure the desired viewports in this pipeline
		SetViewPort( m_RightEyeViewport );
		ConfigureViewports( pPipelineManager );

		// Set this view's render parameters
		SetViewMatrix( rightView );
		SetProjMatrix( rightProj );
		SetRenderParams( pParamManager );

		// Set the light parameters (currently only supporting the first light...)
		if ( m_pScene->GetLightCount() > 0 ) {
			m_pScene->GetLight( 0 )->Parameters.SetRenderParams( pParamManager );
		}

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );

		// If the debug view is enabled, then we can render some additional scene
		// related information as an overlay on this view.  Note that this is
		// accomplished without the actor being attached to the scene!
		if ( m_bDebugViewEnabled )
		{
			std::vector<Entity3D*> list;
			m_pScene->GetRoot()->GetEntities( list );
			m_pDebugVisualizer->UpdateBoundsData( list );

			m_pDebugVisualizer->GetBody()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
		}

		ovrHmd_EndEyeRender(m_pHmd->GetHMD(), ovrEye_Right, eyePose, &m_pHmd->eyeTexture[1].Texture);
	}
}
//--------------------------------------------------------------------------------
void ViewRift::Resize( UINT width, UINT height )
{
	RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );
	RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );
}
//--------------------------------------------------------------------------------
std::wstring ViewRift::GetName()
{
	return( L"ViewRift" );
}
//--------------------------------------------------------------------------------