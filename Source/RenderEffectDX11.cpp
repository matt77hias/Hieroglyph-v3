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
#include "RenderEffectDX11.h"
#include "PipelineManagerDX11.h"
#include "IParameterManager.h"
#include "ConstantBufferDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderEffectDX11::RenderEffectDX11()
{
	for ( int i = 0; i < 6; i++ ) {
		m_aiIndices[i] = -1;
		m_apShaders[i] = nullptr;
	}

	m_iBlendState = -1;
	m_iDepthStencilState = -1;
	m_iRasterizerState = -1;
    m_uStencilRef = 0;
}
//--------------------------------------------------------------------------------
RenderEffectDX11::~RenderEffectDX11()
{

}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetVertexShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[VERTEX_SHADER] = index;
	m_apShaders[VERTEX_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[VERTEX_SHADER] != nullptr ) {
		if ( m_apShaders[VERTEX_SHADER]->GetType() != VERTEX_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-vertex shader to the vertex shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetHullShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[HULL_SHADER] = index;
	m_apShaders[HULL_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[HULL_SHADER] != nullptr ) {
		if ( m_apShaders[HULL_SHADER]->GetType() != HULL_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-hull shader to the hull shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetDomainShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[DOMAIN_SHADER] = index;
	m_apShaders[DOMAIN_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[DOMAIN_SHADER] != nullptr ) {
		if ( m_apShaders[DOMAIN_SHADER]->GetType() != DOMAIN_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-domain shader to the domain shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetGeometryShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[GEOMETRY_SHADER] = index;
	m_apShaders[GEOMETRY_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[GEOMETRY_SHADER] != nullptr ) {
		if ( m_apShaders[GEOMETRY_SHADER]->GetType() != GEOMETRY_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-geometry shader to the geometry shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetPixelShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[PIXEL_SHADER] = index;
	m_apShaders[PIXEL_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[PIXEL_SHADER] != nullptr ) {
		if ( m_apShaders[PIXEL_SHADER]->GetType() != PIXEL_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-pixel shader to the pixel shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::SetComputeShader( int index )
{
	// No checks are necessary here, since it is perfectly legal to set a null
	// shader and deactivate the stage.

	m_aiIndices[COMPUTE_SHADER] = index;
	m_apShaders[COMPUTE_SHADER] = RendererDX11::Get()->GetShader( index );

	if ( m_apShaders[COMPUTE_SHADER] != nullptr ) {
		if ( m_apShaders[COMPUTE_SHADER]->GetType() != COMPUTE_SHADER ) {
			Log::Get().Write( L"Trying to bind a non-compute shader to the compute shader..." );
		}
	}

	UpdateConstantBufferList();
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::UpdateConstantBufferList()
{
	// Empty the list to start out.

	m_uniqueConstBuffers.empty();


	// Check each stage to see if it has a valid shader program.  If so, then 
	// inspect its reflection data to ensure that the overall list of constant
	// buffers includes that stage's needs.

	for ( int stage = 0; stage < 6; stage++ ) {
	
		ShaderDX11* pShader = m_apShaders[stage];

		if ( nullptr != pShader ) {

			ShaderReflectionDX11* pReflection = pShader->GetReflection();
		
			for ( unsigned int i = 0; i < pReflection->ConstantBuffers.size(); i++ ) {
			
				RenderParameterDX11* pParameter = pReflection->ConstantBuffers[i].pParamRef;

				// If it isn't already included, then add it to the list.

				bool bAlreadyThere = false;
				for ( auto pExistingParam : m_uniqueConstBuffers ) {
					if ( pParameter == pExistingParam ) {
						bAlreadyThere = true;
						break;
					}
				}

				if ( !bAlreadyThere ) {
					m_uniqueConstBuffers.push_back( pParameter );
				} else {
					Log::Get().Write( L"Skipped adding a duplicate constant buffer..." );
				}

			}
		}
	}
}
//--------------------------------------------------------------------------------
void RenderEffectDX11::ConfigurePipeline( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// For the standard states, only apply the state change if there is a valid
	// state in this render effect.  If not, use the current state that has already
	// been specified before this effect was bound.

	if ( m_iBlendState != -1 ) {
		pPipeline->OutputMergerStage.DesiredState.BlendState.SetState( m_iBlendState );
	} else {
		pPipeline->OutputMergerStage.DesiredState.BlendState.SetState( 0 );
	}

	if ( m_iDepthStencilState != -1 ) {
		pPipeline->OutputMergerStage.DesiredState.DepthStencilState.SetState( m_iDepthStencilState );
		pPipeline->OutputMergerStage.DesiredState.StencilRef.SetState( m_uStencilRef );
	} else {
		pPipeline->OutputMergerStage.DesiredState.DepthStencilState.SetState( 0 );
	}

	if ( m_iRasterizerState != -1 ) {
		pPipeline->RasterizerStage.DesiredState.RasterizerState.SetState( m_iRasterizerState );
	} else {
		pPipeline->RasterizerStage.DesiredState.RasterizerState.SetState( 0 );
	}

	// Update the contents of the needed constant buffers.

	for ( auto pParameter : m_uniqueConstBuffers ) {
		ConstantBufferDX11* cbuffer = RendererDX11::Get()->GetConstantBufferByIndex( pParamManager->GetConstantBufferParameter( pParameter ) );
		cbuffer->EvaluateMappings( pPipeline, pParamManager );
	}

	// For the shaders, we bind each one even if the shader is not valid in this
	// render effect.  This allows for various pipeline stages to be disabled when
	// they are not being used.  The renderer will update each shader's required
	// resources based on the shader reflection information stored in each shader.

	pPipeline->BindShader( VERTEX_SHADER, m_aiIndices[VERTEX_SHADER], pParamManager );
	pPipeline->BindShader( HULL_SHADER, m_aiIndices[HULL_SHADER], pParamManager );
	pPipeline->BindShader( DOMAIN_SHADER, m_aiIndices[DOMAIN_SHADER], pParamManager );
	pPipeline->BindShader( GEOMETRY_SHADER, m_aiIndices[GEOMETRY_SHADER], pParamManager );
	pPipeline->BindShader( PIXEL_SHADER, m_aiIndices[PIXEL_SHADER], pParamManager );
	pPipeline->BindShader( COMPUTE_SHADER, m_aiIndices[COMPUTE_SHADER], pParamManager );

	// After making the changes to the resources above, the renderer will apply 
	// the changes to the pipeline when a draw/dispatch call is made.
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetVertexShader( )
{
	return( m_aiIndices[VERTEX_SHADER] );
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetHullShader( )
{
	return( m_aiIndices[HULL_SHADER] );
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetDomainShader( )
{
	return( m_aiIndices[DOMAIN_SHADER] );
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetGeometryShader( )
{
	return( m_aiIndices[GEOMETRY_SHADER] );
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetPixelShader( )
{
	return( m_aiIndices[PIXEL_SHADER] );
}
//--------------------------------------------------------------------------------
int RenderEffectDX11::GetComputeShader( )
{
	return( m_aiIndices[COMPUTE_SHADER] );
}
//--------------------------------------------------------------------------------
