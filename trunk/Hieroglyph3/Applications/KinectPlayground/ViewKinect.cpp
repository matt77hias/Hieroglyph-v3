//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "ViewKinect.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
#include "IParameterManager.h"
#include "Texture2dConfigDX11.h"
#include "KinectManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewKinect::ViewKinect( RendererDX11& Renderer )
{
	m_sParams.iViewType = VT_SIMULATION;

	// Prepare some intial height data for the simulation.

	m_pSysMemColor = new BYTE[640*480*4];
	m_pSysMemDepth = new BYTE[320*240*1];

	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem				= 0;
	InitialData.SysMemPitch			= 0;
	InitialData.SysMemSlicePitch	= 0;

	// Create the resources.  We make a default usage texture as well as a staging
	// texture for each of the depth and color buffers.

	Texture2dConfigDX11 config;
	config.SetDefaults();
	config.SetWidth( 320 );
	config.SetHeight( 240 );
	config.SetFormat( DXGI_FORMAT_R8_UNORM );
	config.SetUsage( D3D11_USAGE_STAGING );
	config.SetBindFlags( 0 );
	config.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );

	KinectDepthStaging = Renderer.CreateTexture2D( &config, 0 );

	config.SetUsage( D3D11_USAGE_DEFAULT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	config.SetCPUAccessFlags( 0 );

	KinectDepth = Renderer.CreateTexture2D( &config, 0 );


	config.SetWidth( 640 );
	config.SetHeight( 480 );
	config.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB );
	config.SetUsage( D3D11_USAGE_STAGING );
	config.SetBindFlags( 0 );
	config.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );

	KinectColorStaging = Renderer.CreateTexture2D( &config, 0 );

	config.SetUsage( D3D11_USAGE_DEFAULT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	config.SetCPUAccessFlags( 0 );

	KinectColor = Renderer.CreateTexture2D( &config, 0 );


	pKinectManager = new KinectManager();
	pKinectManager->Initialize();

	pKinectManager->SetSysMemColorBuffer( m_pSysMemColor );
	pKinectManager->SetSysMemDepthBuffer( m_pSysMemDepth );

	// TODO: Add error checking here.

	// Get references to the parameters that will be updated throughout the simulation.
	//m_pCurrentWaterState = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"CurrentWaterState" ) );
	//m_pNewWaterState = Renderer.m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"NewWaterState" ) );
	//m_pDispatchSize = Renderer.m_pParamMgr->GetVectorParameterRef( std::wstring( L"DispatchSize" ) );
}
//--------------------------------------------------------------------------------
ViewKinect::~ViewKinect()
{
	pKinectManager->Shutdown();
	delete pKinectManager;

	delete [] m_pSysMemColor;
	delete [] m_pSysMemDepth;
}
//--------------------------------------------------------------------------------
void ViewKinect::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewKinect::PreDraw( RendererDX11* pRenderer )
{
	// Queue this view into the renderer for processing.  Since this is a 
	// simulation style view, there is no root and hence no additional recursive
	// 'PreDraw'ing required.
	pRenderer->QueueRenderView( this );
}
//--------------------------------------------------------------------------------
void ViewKinect::Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	// In this view, we check to see if the KinectManager has given us a system
	// memory copy of one or both of the frames.  If so, then we copy it into the
	// GPU resources.

	if ( pKinectManager->GetSysMemColorBuffer() == NULL ) {
		
		// Map staging resource and copy in the data....
		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipelineManager->MapResource( KinectColorStaging->m_iResource, 0, D3D11_MAP_WRITE, 0 );

		BYTE* pRunner = m_pSysMemColor; 
		BYTE* pResource = (BYTE*)resource.pData;
		for ( int y = 0; y < 480; y++ ) {
			memcpy( pResource, pRunner, sizeof(BYTE)*640*4 );
			pRunner += sizeof(BYTE)*640*4;
			pResource += resource.RowPitch;
		}
		pPipelineManager->UnMapResource( KinectColorStaging->m_iResource, 0 );

		// Copy the staging resource to the default usage resource.
		D3D11_BOX box;
		box.left = 0;
		box.right = 640;
		box.top = 0;
		box.bottom = 480;
		box.front = 0;
		box.back = 1;

		pPipelineManager->CopySubresourceRegion( 
			KinectColor, 0, 0, 0, 0, KinectColorStaging, 0, &box );

		// Set up the kinect for the next frame
		pKinectManager->SetSysMemColorBuffer( this->m_pSysMemColor );
	}

	if ( pKinectManager->GetSysMemDepthBuffer() == NULL ) {

		// Map staging resource and copy in the data....
		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipelineManager->MapResource( KinectDepthStaging->m_iResource, 0, D3D11_MAP_WRITE, 0 );

		BYTE* pRunner = m_pSysMemDepth; 
		BYTE* pResource = (BYTE*)resource.pData;
		for ( int y = 0; y < 240; y++ ) {
			memcpy( pResource, pRunner, sizeof(BYTE)*320*1 );
			pRunner += sizeof(BYTE)*320*1;
			pResource += resource.RowPitch;
		}
		pPipelineManager->UnMapResource( KinectDepthStaging->m_iResource, 0 );

		// Copy the staging resource to the default usage resource.
		D3D11_BOX box;
		box.left = 0;
		box.right = 320;
		box.top = 0;
		box.bottom = 240;
		box.front = 0;
		box.back = 1;

		pPipelineManager->CopySubresourceRegion( 
			KinectDepth, 0, 0, 0, 0, KinectDepthStaging, 0, &box );

		// Set up the kinect for the next frame
		pKinectManager->SetSysMemDepthBuffer( this->m_pSysMemDepth );

	}

	// Set this view's render parameters.
	//SetRenderParams( pParamManager );

	// Perform the dispatch call to update the simulation state.
	//pPipelineManager->Dispatch( *pWaterEffect, ThreadGroupsX, ThreadGroupsY, 1, pParamManager );

	// Switch the two resources so that the current state is maintained in slot 0.
	//ResourcePtr TempState = WaterState[0];
	//WaterState[0] = WaterState[1];
	//WaterState[1] = TempState;
}
//--------------------------------------------------------------------------------
void ViewKinect::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, water state '0' is always considered the current
	// state.

	//pParamManager->SetShaderResourceParameter( m_pCurrentWaterState, WaterState[0] );
	//pParamManager->SetUnorderedAccessParameter( m_pNewWaterState, WaterState[1] );
}
//--------------------------------------------------------------------------------
void ViewKinect::SetUsageParams( IParameterManager* pParamManager )
{
	// Set the parameters for allowing an application to use the current state
	// as a height map via a shader resource view.

	//Vector4f DispatchSize = Vector4f( 16.0f, 16.0f, 16.0f * 16.0f, 16.0f * 16.0f );

	//pParamManager->SetShaderResourceParameter( m_pCurrentWaterState, WaterState[0] );
	//pParamManager->SetVectorParameter( m_pDispatchSize, &DispatchSize );
}
//--------------------------------------------------------------------------------
ResourcePtr ViewKinect::GetColorResource()
{
	return( KinectColor );
}
//--------------------------------------------------------------------------------
ResourcePtr ViewKinect::GetDepthResource()
{
	return( KinectDepth );
}
//--------------------------------------------------------------------------------