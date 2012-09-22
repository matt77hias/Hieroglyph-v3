//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineManagerDX11.h"
#include "Log.h"
#include "BlendStateDX11.h"
#include "DepthStencilStateDX11.h"
#include "RasterizerStateDX11.h"
#include "ViewPortDX11.h"
#include "SamplerStateDX11.h"
#include "InputLayoutDX11.h"
#include "IParameterManager.h"
#include "ResourceDX11.h"

#include "RenderParameterDX11.h"
#include "VectorParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "SamplerParameterDX11.h"

#include "ShaderResourceViewDX11.h"
#include "RenderTargetViewDX11.h"
#include "DepthStencilViewDX11.h"
#include "UnorderedAccessViewDX11.h"

#include "CommandListDX11.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"

#include "VertexShaderDX11.h"
#include "HullShaderDX11.h"
#include "DomainShaderDX11.h"
#include "GeometryShaderDX11.h"
#include "PixelShaderDX11.h"
#include "ComputeShaderDX11.h"

#include "IndirectArgsBufferDX11.h"

#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PipelineManagerDX11::PipelineManagerDX11()
{
	m_pContext = 0;
    m_iCurrentQuery = 0;
    for( int i = 0; i < NumQueries; ++i)
	    m_Queries[i] = 0;

    ZeroMemory(&m_PipelineStatsData, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS));

	// Reference the shader stages for use in the binding process.

	ShaderStages[VERTEX_SHADER] = &VertexShaderStage;
	ShaderStages[HULL_SHADER] = &HullShaderStage;
	ShaderStages[DOMAIN_SHADER] = &DomainShaderStage;
	ShaderStages[GEOMETRY_SHADER] = &GeometryShaderStage;
	ShaderStages[PIXEL_SHADER] = &PixelShaderStage;
	ShaderStages[COMPUTE_SHADER] = &ComputeShaderStage;
}
//--------------------------------------------------------------------------------
PipelineManagerDX11::~PipelineManagerDX11()
{
	if( m_pContext ) m_pContext->ClearState();
	if( m_pContext ) m_pContext->Flush();

	SAFE_RELEASE( m_pContext );
    for( int i = 0; i < NumQueries; ++i)
	    SAFE_RELEASE( m_Queries[i] );	
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetDeviceContext( ID3D11DeviceContext* pContext, D3D_FEATURE_LEVEL level )
{
	m_pContext = pContext;
	m_FeatureLevel = level;

	// For each pipeline stage object, set its feature level here so they know
	// what they can do and what they can't do.

	ShaderStages[VERTEX_SHADER]->SetFeatureLevel( level );
	ShaderStages[HULL_SHADER]->SetFeatureLevel( level );
	ShaderStages[DOMAIN_SHADER]->SetFeatureLevel( level );
	ShaderStages[GEOMETRY_SHADER]->SetFeatureLevel( level );
	ShaderStages[PIXEL_SHADER]->SetFeatureLevel( level );
	ShaderStages[COMPUTE_SHADER]->SetFeatureLevel( level );

	InputAssemblerStage.SetFeautureLevel( level );
	RasterizerStage.SetFeatureLevel( level );
	OutputMergerStage.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindConstantBufferParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == CBUFFER ) {
			ConstantBufferParameterDX11* pBuffer = reinterpret_cast<ConstantBufferParameterDX11*>( pParam );
			int ID = pBuffer->GetIndex( tID ); 

			ResourceDX11* pResource = pRenderer->GetResourceByIndex( ID );

			// Allow a range including -1 up to the number of resources
			if ( pResource || ( ID == -1 ) ) {
				// Get the resource to be set, and pass it in to the desired shader type
				
				ID3D11Buffer* pBuffer = 0;
				
				if ( ID >= 0 ) {
					pBuffer = (ID3D11Buffer*)pResource->GetResource();
				}

				ShaderStages[type]->DesiredState.SetConstantBuffer( slot, pBuffer );
			} else {
				Log::Get().Write( L"Tried to set an invalid constant buffer ID!" );
			}
		} else {
			Log::Get().Write( L"Tried to set a non-constant buffer ID as a constant buffer!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a constant buffer!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShaderResourceParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == SHADER_RESOURCE ) {
			ShaderResourceParameterDX11* pResource = 
				reinterpret_cast<ShaderResourceParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 

			ShaderResourceViewDX11* pView = pRenderer->GetShaderResourceViewByIndex( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) ) {

				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11ShaderResourceView* pResourceView = 0;

				if ( ID >= 0 ) {
					pResourceView = pView->m_pShaderResourceView;
				}

				ShaderStages[type]->DesiredState.SetShaderResourceView( slot, pResourceView );
			} else {
				Log::Get().Write( L"Tried to set an invalid shader resource ID!" );
			}
		} else {
			Log::Get().Write( L"Tried to set a non-shader resource ID as a shader resource!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a shader resource!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindUnorderedAccessParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                       IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == UNORDERED_ACCESS ) {

			UnorderedAccessParameterDX11* pResource = 
				reinterpret_cast<UnorderedAccessParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 
			unsigned int initial = pResource->GetInitialCount( tID );

			UnorderedAccessViewDX11* pView = pRenderer->GetUnorderedAccessViewByIndex( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) ) {

				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11UnorderedAccessView* pResourceView = 0;

				if ( ID >= 0 ) {
					pResourceView = pView->m_pUnorderedAccessView;
				}

				ShaderStages[type]->DesiredState.SetUnorderedAccessView( slot, pResourceView, initial );
			} else {
				Log::Get().Write( L"Tried to set an invalid unordered access ID!" );
			} 
		} else {
			Log::Get().Write( L"Tried to set a non-unordered access view ID as a unordered access view!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a unordered access view!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindSamplerStateParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                    IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == SAMPLER ) {

			SamplerParameterDX11* pResource = 
				reinterpret_cast<SamplerParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 
			SamplerStateDX11* pState = pRenderer->GetSamplerState( ID );

			// Allow a range including -1 up to the number of samplers

			if ( pState || ( ID == -1 ) ) {

				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11SamplerState* pSampler = 0;

				if ( ID >= 0 ) {
					pSampler = pState->m_pState; 
				}

				ShaderStages[type]->DesiredState.SetSamplerState( slot, pSampler );
			} else {
				Log::Get().Write( L"Tried to set an invalid sampler state ID!" );
			}
		} else {
			Log::Get().Write( L"Tried to set a non-sampler state ID as a sampler state!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a sampler state!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearRenderTargets( )
{
	OutputMergerStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyRenderTargets( )
{
	OutputMergerStage.ApplyDesiredRenderTargetStates( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyInputResources( )
{
	InputAssemblerStage.ApplyDesiredState( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearInputResources( )
{
	InputAssemblerStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyPipelineResources( )
{
	VertexShaderStage.ApplyDesiredState( m_pContext );
	HullShaderStage.ApplyDesiredState( m_pContext );
	DomainShaderStage.ApplyDesiredState( m_pContext );
	GeometryShaderStage.ApplyDesiredState( m_pContext );
	PixelShaderStage.ApplyDesiredState( m_pContext );
	ComputeShaderStage.ApplyDesiredState( m_pContext );

	// TODO: this may not be the correct place to set this state! The Rasterizer
	// state should probably be split into two parts like the OM state.

	RasterizerStage.ApplyDesiredState( m_pContext );
	OutputMergerStage.ApplyDesiredBlendAndDepthStencilStates( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineResources( )
{
	VertexShaderStage.ClearDesiredState( );
	HullShaderStage.ClearDesiredState( );
	DomainShaderStage.ClearDesiredState( );
	GeometryShaderStage.ClearDesiredState( );
	PixelShaderStage.ClearDesiredState( );
	ComputeShaderStage.ClearDesiredState( );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineState( )
{
	// First clear out our cached states.

	InputAssemblerStage.ClearCurrentState();
	InputAssemblerStage.ClearDesiredState();

	VertexShaderStage.ClearCurrentState();
	VertexShaderStage.ClearDesiredState();

	HullShaderStage.ClearCurrentState();
	HullShaderStage.ClearDesiredState();

	DomainShaderStage.ClearCurrentState();
	DomainShaderStage.ClearDesiredState();

	GeometryShaderStage.ClearCurrentState();
	GeometryShaderStage.ClearDesiredState();

	RasterizerStage.ClearCurrentState( );
	RasterizerStage.ClearDesiredState( );

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.ClearCurrentState();
	OutputMergerStage.ClearDesiredState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
	
	// After our cached states have been cleared, then we can clear the API state.

	m_pContext->ClearState();

	if ( m_pContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED ) {
		ID3D11CommandList* pList;
		m_pContext->FinishCommandList( true, &pList );
		pList->Release();
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndexed( UINT IndexCount, UINT StartIndex, int VertexOffset )
{
	m_pContext->DrawIndexed( IndexCount, StartIndex, VertexOffset );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( UINT VertexCount, UINT StartVertex )
{
	m_pContext->Draw( VertexCount, StartVertex );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
	m_pContext->DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, GeometryPtr geometry, 
										IParameterManager* pParamManager )
{
	Draw( effect, geometry->m_VB, geometry->m_IB,
		geometry->GetInputLayout( effect.GetVertexShader() ), geometry->m_ePrimType,
		geometry->GetVertexSize(), geometry->GetIndexCount(), pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, ResourcePtr vb, ResourcePtr ib,
						int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
						UINT vertexStride, UINT numIndices, IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();

	// Configure the pipeline input data with the input assembler
	// state object.

	InputAssemblerStage.DesiredState.SetPrimitiveTopology( primType );
	
	// Bind the vertex and index buffers.
	if ( vb != NULL ) {
		InputAssemblerStage.DesiredState.SetVertexBuffer( 0, vb->m_iResource, 0, vertexStride );
	}

	if ( ib != NULL ) {
		InputAssemblerStage.DesiredState.SetIndexBuffer( ib->m_iResource );
		InputAssemblerStage.DesiredState.SetIndexBufferFormat( DXGI_FORMAT_R32_UINT );
	}

	// Bind the input layout.
	if ( vb != NULL ) {
		InputAssemblerStage.DesiredState.SetInputLayout( inputLayout );
	}

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext );

	// Use the effect to load all of the pipeline stages here.
	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();
	
	m_pContext->DrawIndexed( numIndices, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawNonIndexed( RenderEffectDX11& effect, ResourcePtr vb, 
                                           int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
                                           UINT vertexStride, UINT vertexCount, UINT startVertexLocation,
                                           IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();
	
	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.SetPrimitiveTopology( primType );

    // Bind the vertex buffer.
	InputAssemblerStage.DesiredState.SetVertexBuffer( 0, vb->m_iResource, 0, vertexStride );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.SetInputLayout( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext );

    // Use the effect to load all of the pipeline stages here.
    ClearPipelineResources();
    effect.ConfigurePipeline( this, pParamManager );
    ApplyPipelineResources();

    m_pContext->Draw( vertexCount, startVertexLocation );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, GeometryPtr geometry,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, IParameterManager* pParamManager )
{
	DrawInstanced( effect, geometry->m_VB, geometry->m_ePrimType, geometry->m_IB,
		geometry->GetInputLayout( effect.GetVertexShader() ),
		geometry->GetVertexSize(), geometry->GetIndexCount(),
		instanceData, instanceDataStride, numInstances, pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, ResourcePtr vb,
								 D3D11_PRIMITIVE_TOPOLOGY primType, ResourcePtr ib,
								 int inputLayout, UINT vertexStride, UINT numIndices,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();

	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.SetPrimitiveTopology( primType );

    // Bind the vertex buffers.
	InputAssemblerStage.DesiredState.SetVertexBuffer( 0, vb->m_iResource, 0, vertexStride );
	InputAssemblerStage.DesiredState.SetVertexBuffer( 1, instanceData->m_iResource, 0, instanceDataStride );

	// Bind the index buffer.
	InputAssemblerStage.DesiredState.SetIndexBuffer( ib->m_iResource );
	InputAssemblerStage.DesiredState.SetIndexBufferFormat( DXGI_FORMAT_R32_UINT );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.SetInputLayout( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext );

	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->DrawIndexedInstanced( numIndices, numInstances, 0, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DispatchIndirect( RenderEffectDX11& effect,
										    ResourcePtr args,
											UINT offset,
											IParameterManager* pParamManager )
{
	// Use the effect to load all of the pipeline stages here.

	BufferDX11* pBuffer = RendererDX11::Get()->GetIndirectArgsBufferByIndex( args->m_iResource );


	if ( pBuffer == NULL ) 	{
		Log::Get().Write( L"Tried to use the wrong resource type for an indirect dispatch!" );
		return;
	}

	ID3D11Buffer* pArgsBuffer = (ID3D11Buffer*)pBuffer->GetResource();


	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->DispatchIndirect( pArgsBuffer, offset );

}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndirect( RenderEffectDX11& effect,
									    ResourcePtr args,
										UINT offset,
										int inputLayout,
										D3D11_PRIMITIVE_TOPOLOGY primType,
										UINT vertexStride,
										IParameterManager* pParamManager )
{
	IndirectArgsBufferDX11* pBuffer = RendererDX11::Get()->GetIndirectArgsBufferByIndex( args->m_iResource );

	if ( !pBuffer ) {
		Log::Get().Write( L"Failure to get indirect args buffer, aborting call..." );
		return;
	} 

	ID3D11Buffer* pArgsBuffer = reinterpret_cast<ID3D11Buffer*>( pBuffer->GetResource() );

	InputAssemblerStage.ClearDesiredState();

	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.SetPrimitiveTopology( primType );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.SetInputLayout( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext );

	// Use the effect to load all of the pipeline stages here.
	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();
	
	m_pContext->DrawInstancedIndirect( pArgsBuffer, offset );

}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z, IParameterManager* pParamManager )
{
	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->Dispatch( x, y, z );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopyStructureCount( ResourcePtr dest, UINT offset, ResourcePtr uav )
{
	int id = dest->m_iResource;

	BufferDX11* pArgsBuffer = RendererDX11::Get()->GetGenericBufferByIndex( id );
	ID3D11Buffer* pRawArgsBuffer = 0;

	if ( pArgsBuffer ) {
		pRawArgsBuffer = (ID3D11Buffer*)pArgsBuffer->GetResource(); // TODO: add ID3D11Buffer accessor to the buffer resource classes!
	}

	UnorderedAccessViewDX11* pView = RendererDX11::Get()->GetUnorderedAccessViewByIndex( uav->m_iResourceUAV );
	ID3D11UnorderedAccessView* pRawView = 0;

	if ( pView ) {
		pRawView = pView->m_pUnorderedAccessView;
	}

	m_pContext->CopyStructureCount( pRawArgsBuffer, offset, pRawView );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearBuffers( Vector4f color, float depth, UINT stencil )
{
	// Get the current render target view and depth stencil view from the OM stage.
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { NULL };
	ID3D11DepthStencilView* pDepthStencilView = 0;

	// Our output merger class manages the current render targets bound to the pipeline.
	// The number of views that are currently set refers to the views that the application
	// has submitted to the pipeline, but not necessarily that have been bound to the API.
	// For each view that the application thinks is bound, we clear it as requested.  Since
	// the view count is the number of non-null views, we don't need to check here.

	UINT viewCount = OutputMergerStage.GetCurrentState().GetRenderTargetCount();

	for( UINT i = 0; i < viewCount; ++i )
	{
	    float clearColours[] = { color.x, color.y, color.z, color.w }; // RGBA
		int rtv = OutputMergerStage.GetCurrentState().GetRenderTarget( i );
		RenderTargetViewDX11* pRTV = RendererDX11::Get()->GetRenderTargetViewByIndex( rtv );
		if ( pRTV != 0 ) {
			pRenderTargetViews[i] = pRTV->m_pRenderTargetView; 
			m_pContext->ClearRenderTargetView( pRenderTargetViews[i], clearColours );
		}
	}

	// Check if the output merger currently has a depth target set, and if so clear it.

	if ( OutputMergerStage.GetCurrentState().GetDepthStencilTarget() != -1 )
	{
		int dsv = OutputMergerStage.GetCurrentState().GetDepthStencilTarget();
		DepthStencilViewDX11* pDSV = RendererDX11::Get()->GetDepthStencilViewByIndex( dsv );
		if ( pDSV != 0 ) {
			pDepthStencilView = pDSV->m_pDepthStencilView;
			m_pContext->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH, depth, stencil );
		}
		
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShader( ShaderType type, int ID, IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( ID );

	// Record the shader ID for use later on
	ShaderStages[type]->DesiredState.SetShaderProgram( ID );

	// Check if the shader has a valid identifier
	if ( pShaderDX11 )
	{
		// Perform the actual binding to the pipeline, and then bind all needed
		// parameters.

		if ( pShaderDX11->GetType() == type )
		{
			// Before binding the shader, have it update its required parameters.  These
			// parameters will then be bound to the pipeline after the shader is bound.

			//pShaderDX11->GetReflection()->UpdateParameters( this, pParamManager );
			pShaderDX11->GetReflection()->BindParameters( type, this, pParamManager );
		}
		else
		{
			Log::Get().Write( L"Tried to set the wrong type of shader ID!" );
		}
	}
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( int rid, UINT subresource, D3D11_MAP actions, UINT flags )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	return( MapResource( pGlyphResource, subresource, actions, flags ) );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( int rid, UINT subresource )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	UnMapResource( pGlyphResource, subresource );
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( ResourcePtr pResource, UINT subresource, D3D11_MAP actions, UINT flags )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( pResource->m_iResource );

	return( MapResource( pGlyphResource, subresource, actions, flags ) );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( ResourcePtr pResource, UINT subresource )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( pResource->m_iResource );

	UnMapResource( pGlyphResource, subresource );
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( ResourceDX11* pGlyphResource, UINT subresource, D3D11_MAP actions, UINT flags )
{
	D3D11_MAPPED_SUBRESOURCE Data;
	Data.pData = NULL;
	Data.DepthPitch = Data.RowPitch = 0;

	if ( nullptr == pGlyphResource ) {
		Log::Get().Write( L"Trying to map a subresource that doesn't exist!!!" );
		return( Data );
	}

	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( nullptr == pResource ) {
		Log::Get().Write( L"Trying to map a subresource that has no native resource in it!!!" );
		return( Data );
	}

	// Perform the mapping of the resource.
	HRESULT hr = m_pContext->Map( pResource, subresource, actions, flags, &Data );
	
	if ( FAILED( hr ) ) {
		Log::Get().Write( L"Failed to map resource!" );
	}

	return( Data );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( ResourceDX11* pGlyphResource, UINT subresource )
{
	if ( NULL == pGlyphResource ) {
		Log::Get().Write( L"Trying to unmap a subresource that doesn't exist!!!" );
		return;
	}

	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( NULL == pResource ) {
		Log::Get().Write( L"Trying to unmap a subresource that has no native resource in it!!!" );
		return;
	}

	// Unmap the resource - there is no HRESULT returned, so trust that it works...
	m_pContext->Unmap( pResource, subresource );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UpdateSubresource( int rid, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	if ( NULL == pGlyphResource ) {
		Log::Get().Write( L"Trying to update a subresource that doesn't exist!!!" );
		return;
	}

	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( NULL == pResource ) {
		Log::Get().Write( L"Trying to update a subresource that has no native resource in it!!!" );
		return;
	}

	// Perform the update of the resource.
	m_pContext->UpdateSubresource( pResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::StartPipelineStatistics( )
{
	if ( m_Queries[m_iCurrentQuery] )            
		m_pContext->Begin( m_Queries[m_iCurrentQuery] );    
	else
		Log::Get().Write( L"Tried to begin pipeline statistics without a query object!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::EndPipelineStatistics( )
{
	if ( m_Queries[m_iCurrentQuery] )
	{        
		m_pContext->End( m_Queries[m_iCurrentQuery] );
     
        m_iCurrentQuery = ( m_iCurrentQuery + 1 ) % NumQueries;
        HRESULT hr = m_pContext->GetData( m_Queries[m_iCurrentQuery], &m_PipelineStatsData, 
                                            sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0);
        if ( FAILED( hr ) )
            Log::Get().Write( L"Failed attempting to retrieve query data" );        
	}
	else
		Log::Get().Write( L"Tried to end pipeline statistics without a valid query object!" );
}
//--------------------------------------------------------------------------------
std::wstring PipelineManagerDX11::PrintPipelineStatistics( )
{
	std::wstringstream s;
	s << "Pipeline Statistics:" << std::endl;
	s << "Number of vertices read by the IA: " << m_PipelineStatsData.IAVertices << std::endl;
	s << "Number of primitives read by the IA: " << m_PipelineStatsData.IAPrimitives << std::endl;
	s << "Number of vertex shader invocations: " << m_PipelineStatsData.VSInvocations << std::endl;
	s << "Number of hull shader invocations: " << m_PipelineStatsData.HSInvocations << std::endl;
	s << "Number of domain shader invocations: " << m_PipelineStatsData.DSInvocations << std::endl;
	s << "Number of geometry shader invocations: " << m_PipelineStatsData.GSInvocations << std::endl;
	s << "Number of primitives output by the geometry shader: " << m_PipelineStatsData.GSPrimitives << std::endl;
	s << "Number of primitives sent to the rasterizer: " << m_PipelineStatsData.CInvocations << std::endl;
	s << "Number of primitives rendered: " << m_PipelineStatsData.CPrimitives << std::endl;
	s << "Number of pixel shader invocations: " << m_PipelineStatsData.PSInvocations << std::endl;

	return( s.str() );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SaveTextureScreenShot( int index, std::wstring filename, D3DX11_IMAGE_FILE_FORMAT format )
{
	// Get the texture as a resource and save it to file
	ID3D11Resource* pResource = RendererDX11::Get()->GetResourceByIndex( index )->GetResource();

	if ( pResource )
	{
		// Increment the file number for allowing a sequence of images to be stored...
		static int iScreenNum = 100000;
		iScreenNum++;

		// Build the output file name
		std::wstringstream out;
		out << filename << iScreenNum;

		// Select the appropriate format to add the extension to the name.  DDS format
		// may be the only one that can handle certain texture formats, so it may be a 
		// backup format later on.

		switch ( format )
		{
		case D3DX11_IFF_BMP:
			out << ".bmp";
			break;
		case D3DX11_IFF_JPG:
			out << ".jpg";
			break;
		case D3DX11_IFF_PNG:
			out << ".png";
			break;
		case D3DX11_IFF_DDS:
			out << ".dds";
			break;
		case D3DX11_IFF_TIFF:
			out << ".tiff";
			break;
		case D3DX11_IFF_GIF:
			out << ".gif";
			break;
		case D3DX11_IFF_WMP:
			out << ".wmp";
			break;
		default:
			Log::Get().Write( L"Tried to save a texture image in an unsupported format!" );
		}


		HRESULT hr = D3DX11SaveTextureToFile(
		  m_pContext,
		  pResource,
		  format,
		  out.str().c_str()
		);

		if ( FAILED( hr ) )
			Log::Get().Write( L"D3DX11SaveTextureToFile has failed!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopySubresourceRegion( ResourcePtr DestResource, UINT DstSubresource, 
	UINT DstX, UINT DstY, UINT DstZ, ResourcePtr SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox )
{
	ID3D11Resource* pDestResource = RendererDX11::Get()->GetResourceByIndex(DestResource->m_iResource)->GetResource();
	ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResourceByIndex(SrcResource->m_iResource)->GetResource();

	m_pContext->CopySubresourceRegion( pDestResource, DstSubresource, DstX, DstY, DstZ,
		pSrcResource, SrcSubresource, pSrcBox );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::GenerateCommandList( CommandListDX11* pList )
{
	if ( m_pContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED )
	{
		m_pContext->FinishCommandList( false, &pList->m_pList );

		// Reset the cached context state to default, since we do that for all
		// command lists.

		InputAssemblerStage.ClearCurrentState();
		InputAssemblerStage.ClearDesiredState();

		VertexShaderStage.ClearCurrentState();
		VertexShaderStage.ClearDesiredState();

		HullShaderStage.ClearCurrentState();
		HullShaderStage.ClearDesiredState();

		DomainShaderStage.ClearCurrentState();
		DomainShaderStage.ClearDesiredState();

		GeometryShaderStage.ClearCurrentState();
		GeometryShaderStage.ClearDesiredState();

		RasterizerStage.ClearCurrentState( );
		RasterizerStage.ClearDesiredState( );

		PixelShaderStage.ClearCurrentState();
		PixelShaderStage.ClearDesiredState();

		OutputMergerStage.ClearCurrentState();
		OutputMergerStage.ClearDesiredState();

		ComputeShaderStage.ClearCurrentState();
		ComputeShaderStage.ClearDesiredState();

	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ExecuteCommandList( CommandListDX11* pList )
{
	if ( pList->ListAvailable() )
		m_pContext->ExecuteCommandList( pList->m_pList, false );

	InputAssemblerStage.ClearCurrentState();
	InputAssemblerStage.ClearDesiredState();

	VertexShaderStage.ClearCurrentState();
	VertexShaderStage.ClearDesiredState();

	HullShaderStage.ClearCurrentState();
	HullShaderStage.ClearDesiredState();

	DomainShaderStage.ClearCurrentState();
	DomainShaderStage.ClearDesiredState();

	GeometryShaderStage.ClearCurrentState();
	GeometryShaderStage.ClearDesiredState();

	RasterizerStage.ClearCurrentState( );
	RasterizerStage.ClearDesiredState( );

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.ClearCurrentState();
	OutputMergerStage.ClearDesiredState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ResolveSubresource( ResourcePtr DestResource, UINT DstSubresource, 
                                              ResourcePtr SrcResource, UINT SrcSubresource, 
                                              DXGI_FORMAT format )
{
    int DestID = DestResource->m_iResource;
    ID3D11Resource* pDestResource = RendererDX11::Get()->GetResourceByIndex(DestID)->GetResource();

    int SrcID = SrcResource->m_iResource;
    ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResourceByIndex(SrcID)->GetResource();
 
    m_pContext->ResolveSubresource( pDestResource, DstSubresource, pSrcResource, SrcSubresource, format );
}
//--------------------------------------------------------------------------------
