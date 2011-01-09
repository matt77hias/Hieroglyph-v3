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
#include "PCH.h"
#include "ViewParaboloidEnvMap.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ParameterManagerDX11.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewParaboloidEnvMap::ViewParaboloidEnvMap( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	m_sParams.iViewType = VT_DUAL_PARABOLOID_ENVMAP;

	m_RenderTarget = RenderTarget;
	m_DepthTarget = DepthTarget;

	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_ParaboloidBasis.MakeIdentity();

	m_pEntity = 0;
	m_vColor.MakeZero();

	ResourceDX11* pResource = Renderer.GetResource( m_RenderTarget->m_iResource & 0x0000ffff );

	if ( pResource->GetType() == D3D11_RESOURCE_DIMENSION_TEXTURE2D )
	{
		Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
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

		m_iViewport = Renderer.CreateViewPort( viewport );
	}
}
//--------------------------------------------------------------------------------
ViewParaboloidEnvMap::~ViewParaboloidEnvMap()
{
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetBackColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::PreDraw( RendererDX11* pRenderer )
{
	if ( m_pEntity != NULL )
	{
		m_ParaboloidBasis = m_pEntity->GetView();
		SetViewMatrix( m_ParaboloidBasis );
	}

	if ( m_iCurrRecurrence > 0 )
	{
		// Decrement the count to prevent unneeded updates
		m_iCurrRecurrence--;

		// Queue this view into the renderer for processing.
		pRenderer->QueueRenderView( this );

		if ( m_pRoot )
		{
			TArray<Entity3D*> set;
			m_pRoot->GetEntities( set );

			for ( int i = 0; i < set.count(); i++ )
			{
				if ( set[i] != this->m_pEntity )
					set[i]->PreRender( pRenderer, GetType() );// pPipelineManager, pParamManager, GetType() );
			}
			// Run through the graph and pre-render the entities
			//m_pRoot->PreRender( pRenderer, GetType() );
		}
	}
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
	//if ( m_iCurrRecurrence > 0 )
	{

		if ( m_pRoot )
		{
			// Set the parameters for rendering this view
			pPipelineManager->ClearRenderTargets();
			pPipelineManager->BindRenderTargets( 0, m_RenderTarget );
			pPipelineManager->BindDepthTarget( m_DepthTarget );
			pPipelineManager->ApplyRenderTargets();

			pPipelineManager->SetViewPort( m_iViewport );

			// Set default states for these stages
			pPipelineManager->SetRasterizerState( 0 );
			pPipelineManager->SetDepthStencilState( 0 );
			pPipelineManager->SetBlendState( 0 );

			pPipelineManager->ClearBuffers( m_vColor, 1.0f );

			// Set this view's render parameters
			SetRenderParams( pParamManager );

			// Run through the graph and render each of the entities
			TArray<Entity3D*> set;
			m_pRoot->GetEntities( set );

			for ( int i = 0; i < set.count(); i++ )
			{
				if ( set[i] != this->m_pEntity )
					set[i]->Render( pPipelineManager, pParamManager, GetType() );
			}
			//m_pRoot->Render( pPipelineManager, pParamManager, GetType() );

			pPipelineManager->ClearRenderTargets();
		}
	}
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ )
{
	//m_viewport.X = x;
	//m_viewport.Y = y;
	//m_viewport.Width = w;
	//m_viewport.Height = h;
	//m_viewport.MinZ = MinZ;
	//m_viewport.MaxZ = MaxZ;
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewParaboloidEnvMap::SetUsageParams( ParameterManagerDX11* pParamManager )
{
	pParamManager->SetMatrixParameter( std::wstring( L"ParaboloidBasis" ), &m_ParaboloidBasis );
	pParamManager->SetShaderResourceParameter( std::wstring( L"ParaboloidTexture" ), m_RenderTarget );
}
//--------------------------------------------------------------------------------
