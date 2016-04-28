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
#include "GlyphKinect/ViewKinect.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
#include "IParameterManager.h"
#include "Texture2dConfigDX11.h"
#include "GlyphKinect/KinectManager.h"

#include "GlyphKinect/ArmsForwardPose.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewKinect::ViewKinect( RendererDX11& Renderer )
{
	// Acquire two CPU buffers for caching the CPU data.  We use an 8-bit 
	// 4-component value for the color buffer, and a 16-bit 1-component texture
	// for the depth buffer - so we allocate the accordingly sized arrays.

	m_pSysMemColor = new BYTE[640*480*4];
	m_pSysMemDepth = new BYTE[320*240*2];
	m_pSysSkeleton = new BYTE[sizeof(NUI_SKELETON_FRAME)];

	// Create the resources.  We make a default usage texture as well as a staging
	// texture for each of the depth and color buffers.

	Texture2dConfigDX11 config;
	config.SetDefaults();
	config.SetWidth( 320 );
	config.SetHeight( 240 );
	config.SetFormat( DXGI_FORMAT_R16_UINT );
	config.SetUsage( D3D11_USAGE_STAGING );
	config.SetBindFlags( 0 );
	config.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );

	KinectDepthStaging = Renderer.CreateTexture2D( &config, 0 );

	config.SetUsage( D3D11_USAGE_DEFAULT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	config.SetCPUAccessFlags( 0 );

	KinectDepth = Renderer.CreateTexture2D( &config, 0 );


	// Create the offset texture and its staging resource.

	config.SetWidth( 320 );
	config.SetHeight( 240 );
	config.SetFormat( DXGI_FORMAT_R16G16_UINT );
	config.SetUsage( D3D11_USAGE_STAGING );
	config.SetBindFlags( 0 );
	config.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );

	KinectOffsetStaging = Renderer.CreateTexture2D( &config, 0 );

	config.SetUsage( D3D11_USAGE_DEFAULT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	config.SetCPUAccessFlags( 0 );

	KinectOffset = Renderer.CreateTexture2D( &config, 0 );


	// Create the color texture and its staging resource.

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

	// Create and initialize the Kinect Manager class.  This will handle
	// all of the interaction with the Kinect SDK for us.

	pKinectManager = new KinectManager();
	pKinectManager->Initialize();

	// Indicate to the Kinect Manager that we would like to receive the next
	// frame as soon as it is available.  This is done by giving it a pointer
	// to the buffer where to store the frame data.

	pKinectManager->SetSysMemColorBuffer( m_pSysMemColor );
	pKinectManager->SetSysMemDepthBuffer( m_pSysMemDepth );
	pKinectManager->SetSysMemSkeletonBuffer( m_pSysSkeleton );

	// Get handles to the parameters that will be holding these resources.

	m_pKinectDepthBufferParameter = 
		Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"KinectDepthBuffer" ) );

	m_pKinectColorBufferParameter =
		Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"KinectColorBuffer" ) );

	m_pKinectOffsetBufferParameter =
		Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"KinectOffsetBuffer" ) );

	m_pSkeletonActor = NULL;
}
//--------------------------------------------------------------------------------
ViewKinect::~ViewKinect()
{
	pKinectManager->Shutdown();
	delete pKinectManager;

	delete [] m_pSysMemColor;
	delete [] m_pSysMemDepth;
	delete m_pSysSkeleton;
}
//--------------------------------------------------------------------------------
void ViewKinect::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewKinect::QueuePreTasks( RendererDX11* pRenderer )
{
	// Queue this view into the renderer for processing.  Since this is a 
	// simulation style view, there is no root and hence no additional recursive
	// 'PreDraw'ing required.

	pRenderer->QueueTask( this );
}
//--------------------------------------------------------------------------------
void ViewKinect::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
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
			memcpy( pResource, pRunner, sizeof(BYTE)*320*2 );
			pRunner += sizeof(BYTE)*320*2;
			pResource += resource.RowPitch;
		}
		pPipelineManager->UnMapResource( KinectDepthStaging->m_iResource, 0 );

		// Copy the staging resource to the default usage resource.
		D3D11_BOX box;
		box.left = 0;  box.right = 320;
		box.top = 0;   box.bottom = 240;
		box.front = 0; box.back = 1;

		pPipelineManager->CopySubresourceRegion( 
			KinectDepth, 0, 0, 0, 0, KinectDepthStaging, 0, &box );

		// Set up the kinect for the next frame
		pKinectManager->SetSysMemDepthBuffer( this->m_pSysMemDepth );



		if ( true ) {

			// Map staging resource and copy in the data....
			D3D11_MAPPED_SUBRESOURCE resource = 
				pPipelineManager->MapResource( KinectOffsetStaging->m_iResource, 0, D3D11_MAP_WRITE, 0 );

			BYTE* pResource = (BYTE*)resource.pData;

			for ( long y = 0; y < 240; y++ ) {
				for ( long x = 0; x < 320; x++ ) {
		
					long cx = 0;
					long cy = 0;
					
					USHORT d = *((USHORT*)(&(m_pSysMemDepth[(x+320*y)*2])));
					d <<= 3;

					NuiImageGetColorPixelCoordinatesFromDepthPixel( 
						NUI_IMAGE_RESOLUTION_640x480,
						NULL,
						x, y, d,
						&cx, &cy );

					// Do some range checks on the returned data.

					if ( cx > 639 ) cx = 639;
					if ( cx < 0 ) cx = 0;
					if ( cy > 479 ) cy = 479;
					if ( cy < 0 ) cy = 0;
					
					*((USHORT*)(pResource+x*4))   = (USHORT)cx;
					*((USHORT*)(pResource+x*4+2)) = (USHORT)cy;
					
				}

				pResource += resource.RowPitch;
			}

			pPipelineManager->UnMapResource( KinectOffsetStaging->m_iResource, 0 );

			// Copy the staging resource to the default usage resource.
			D3D11_BOX box;
			box.left = 0;  box.right = 320;
			box.top = 0;   box.bottom = 240;
			box.front = 0; box.back = 1;

			pPipelineManager->CopySubresourceRegion( 
				KinectOffset, 0, 0, 0, 0, KinectOffsetStaging, 0, &box );
		}
	}


	if ( pKinectManager->GetSysMemSkeletonBuffer() == NULL ) {
		
		// Our pointer is now filled with data - we need to do something with it.
		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( ((NUI_SKELETON_FRAME*)m_pSysSkeleton)->SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
			{
				// Valid data is here!
				if ( m_pSkeletonActor != NULL ) {
					m_pSkeletonActor->UpdateSkeleton( &(((NUI_SKELETON_FRAME*)m_pSysSkeleton)->SkeletonData[i]) );
				}

				ArmsForwardPose pose;
				pose.TestPose( ((NUI_SKELETON_FRAME*)m_pSysSkeleton)->SkeletonData[i] );
			}
		}

		// Reset the pointer in the Kinect Manager to allow the next frame to come.
		pKinectManager->SetSysMemSkeletonBuffer( m_pSysSkeleton );
	}
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
	// Set the two resulting buffers as shader resource parameters.

	pParamManager->SetShaderResourceParameter( m_pKinectDepthBufferParameter, KinectDepth );
	pParamManager->SetShaderResourceParameter( m_pKinectColorBufferParameter, KinectColor );
	pParamManager->SetShaderResourceParameter( m_pKinectOffsetBufferParameter, KinectOffset );
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
void ViewKinect::SetSkeletonActor( KinectSkeletonActor* pActor )
{
	m_pSkeletonActor = pActor;
}
//--------------------------------------------------------------------------------
std::wstring ViewKinect::GetName()
{
	return( L"ViewKinect" );
}
//--------------------------------------------------------------------------------