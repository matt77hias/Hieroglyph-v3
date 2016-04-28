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
#include "PCH.h"
#include "ViewParaboloidEnvMap.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "SceneGraph.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewParaboloidEnvMap::ViewParaboloidEnvMap( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	m_RenderTarget = RenderTarget;
	m_DepthTarget = DepthTarget;

	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_ParaboloidBasis.MakeIdentity();

	m_pEntity = nullptr;
		
	// Set up a viewport based on the dimensions of the resource.

	Texture2dDX11* pTexture = Renderer.GetTexture2DByIndex( m_RenderTarget->m_iResource );

	if ( pTexture != NULL ) {
		D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

		// Create a view port to use on the scene.  This basically selects the 
		// entire floating point area of the render target.
		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast< float >( desc.Width );
		viewport.Height = static_cast< float >( desc.Height );
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		SetViewPort( Renderer.CreateViewPort( viewport ) );
	} else {
		// TODO: Fail here...
	}

	// Get references to the parameters that will be used.

	m_pParaboloidTextureParam = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ParaboloidTexture" ) );
	m_pParaboloidBasisParam = Renderer.m_pParamMgr->GetMatrixParameterRef( std::wstring( L"ParaboloidBasis" ) );

	m_iMaxRecurrence = 1;
	m_iCurrRecurrence = m_iMaxRecurrence;
}
//--------------------------------------------------------------------------------
ViewParaboloidEnvMap::~ViewParaboloidEnvMap()
{
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetMaxRecurrence( int max )
{
	m_iMaxRecurrence = max;
}
//--------------------------------------------------------------------------------
int ViewParaboloidEnvMap::GetMaxRecurrence( )
{
	return( m_iMaxRecurrence );
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::QueuePreTasks( RendererDX11* pRenderer )
{
	if ( m_pEntity != NULL )
	{
		m_ParaboloidBasis = m_pEntity->Transform.GetView();
		SetViewMatrix( m_ParaboloidBasis );
	}

	if ( m_iCurrRecurrence > 0 )
	{
		// Decrement the count to prevent unneeded updates
		m_iCurrRecurrence--;

		// Queue this view into the renderer for processing.
		pRenderer->QueueTask( this );

		if ( m_pScene )
		{
			std::vector<Entity3D*> set;
			GetAllEntities( m_pScene->GetRoot(), set );

			for ( auto pEntity : set )
			{
				if ( pEntity != this->m_pEntity )
					pEntity->PreRender( pRenderer, VT_DUAL_PARABOLOID_ENVMAP );
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_pScene )
	{
		// Remove any shader resource views from the pipeline to ensure there are
		// no conflicts with leftover resource bindings from a previous pass.
		pPipelineManager->ClearPipelineSRVs();
		pPipelineManager->ApplyPipelineResources();

		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		pPipelineManager->ClearBuffers( m_vColor, 1.0f );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Run through the graph and render each of the entities
		std::vector<Entity3D*> set;
		GetAllEntities( m_pScene->GetRoot(), set );

		for ( auto pEntity : set )
		{
			if ( pEntity != this->m_pEntity )
				pEntity->Render( pPipelineManager, pParamManager, VT_DUAL_PARABOLOID_ENVMAP );
		}

		pPipelineManager->ClearRenderTargets();
	}
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::Resize( UINT width, UINT height )
{
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetRenderParams( IParameterManager* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetUsageParams( IParameterManager* pParamManager )
{
	pParamManager->SetMatrixParameter( m_pParaboloidBasisParam, &m_ParaboloidBasis );
	pParamManager->SetShaderResourceParameter( m_pParaboloidTextureParam, m_RenderTarget );
}
//--------------------------------------------------------------------------------
std::wstring ViewParaboloidEnvMap::GetName()
{
	return( L"ViewParaboloidEnvMap" );
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::ResetRecurrence()
{
	m_iCurrRecurrence = m_iMaxRecurrence;
}
//--------------------------------------------------------------------------------
