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
	OutputMergerStage.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetBlendState( int ID )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	ID3D11BlendState* pBlendState = pRenderer->GetBlendState( ID )->m_pState;

	if ( pBlendState )
	{
		float afBlendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pContext->OMSetBlendState( pBlendState, afBlendFactors, 0xFFFFFFFF );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid blend state ID!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetDepthStencilState( int ID, UINT stencilRef )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	ID3D11DepthStencilState* pDepthState = pRenderer->GetDepthState( ID )->m_pState;

	if ( pDepthState )
	{
		m_pContext->OMSetDepthStencilState( pDepthState, stencilRef );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid depth stencil state ID!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetRasterizerState( int ID )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	ID3D11RasterizerState* pRasterizerState = pRenderer->GetRasterizerState( ID )->m_pState;

	if ( pRasterizerState )
	{
		m_pContext->RSSetState( pRasterizerState );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid rasterizer state ID!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetViewPort( int ID )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	ViewPortDX11* pViewport = pRenderer->GetViewPort( ID );

	if ( pViewport )
		m_pContext->RSSetViewports( 1, &pViewport->m_ViewPort );
	else
		Log::Get().Write( L"Tried to set an invalid view port index!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetScissorRects( UINT NumRects, const D3D11_RECT* pRects )
{
    RendererDX11* pRenderer = RendererDX11::Get();
    
    m_pContext->RSSetScissorRects( NumRects, pRects );
}
//--------------------------------------------------------------------------------
D3D11_VIEWPORT PipelineManagerDX11::GetCurrentViewport( ) {
	D3D11_VIEWPORT vp;
	UINT numVP = 1;
	m_pContext->RSGetViewports( &numVP, &vp );
	return vp;
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindConstantBufferParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	// TODO: Verify that using GetParameter(...) here is ok for all multithreaded cases! 
	//RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];
	//RenderParameterDX11* pParameter = pParamManager->GetParameterRef( name );
	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 )
	{
		// Check the type of the parameter
		if ( pParam->GetParameterType() == CBUFFER )
		{
			ConstantBufferParameterDX11* pBuffer = reinterpret_cast<ConstantBufferParameterDX11*>( pParam );
			int ID = ( pBuffer->GetIndex( tID ) & 0xffff ); 

			ResourceDX11* pResource = pRenderer->GetResource( ID );

			// Allow a range including -1 up to the number of resources
			if ( pResource || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type
				
				ID3D11Buffer* pBuffer = 0;
				
				if ( ID >= 0 )
					pBuffer = (ID3D11Buffer*)pResource->GetResource();

				ShaderStages[type]->DesiredState.SetConstantBuffer( slot, pBuffer );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid constant buffer ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-constant buffer ID as a constant buffer!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a constant buffer!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShaderResourceParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	// TODO: Verify that using GetParameter(...) here is ok for all multithreaded cases! 
	//RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];
	//RenderParameterDX11* pParameter = pParamManager->GetParameterRef( name );
	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 )
	{
		// Check the type of the parameter
		if ( pParam->GetParameterType() == SHADER_RESOURCE )
		{
			ShaderResourceParameterDX11* pResource = 
				reinterpret_cast<ShaderResourceParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 

			ShaderResourceViewDX11* pView = pRenderer->GetShaderResourceView( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11ShaderResourceView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = pView->m_pShaderResourceView;

				ShaderStages[type]->DesiredState.SetShaderResourceView( slot, pResourceView );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid shader resource ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-shader resource ID as a shader resource!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a shader resource!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindUnorderedAccessParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                       IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	// TODO: Verify that using GetParameter(...) here is ok for all multithreaded cases! 
	//RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];
	//RenderParameterDX11* pParameter = pParamManager->GetParameterRef( name );
	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 )
	{
		// Check the type of the parameter
		if ( pParam->GetParameterType() == UNORDERED_ACCESS )
		{
			UnorderedAccessParameterDX11* pResource = 
				reinterpret_cast<UnorderedAccessParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 
			unsigned int initial = pResource->GetInitialCount( tID );

			UnorderedAccessViewDX11* pView = pRenderer->GetUnorderedAccessView( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11UnorderedAccessView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = pView->m_pUnorderedAccessView;

				ShaderStages[type]->DesiredState.SetUnorderedAccessView( slot, pResourceView, initial );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid unordered access ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-unordered access view ID as a unordered access view!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a unordered access view!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindSamplerStateParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                    IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	// TODO: Verify that using GetParameter(...) here is ok for all multithreaded cases! 
	//RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];
	//RenderParameterDX11* pParameter = pParamManager->GetParameterRef( name );
	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 )
	{
		// Check the type of the parameter
		if ( pParam->GetParameterType() == SAMPLER )
		{
			SamplerParameterDX11* pResource = 
				reinterpret_cast<SamplerParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 
			SamplerStateDX11* pState = pRenderer->GetSamplerState( ID );

			// Allow a range including -1 up to the number of samplers

			if ( pState || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11SamplerState* pSampler = 0;

				if ( ID >= 0 )
					pSampler = pState->m_pState; 

				ShaderStages[type]->DesiredState.SetSamplerState( slot, pSampler );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid sampler state ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-sampler state ID as a sampler state!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a sampler state!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearRenderTargets( )
{
	OutputMergerStage.UnbindResources( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyRenderTargets( )
{
	OutputMergerStage.BindResources( m_pContext );
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
	InputAssemblerStage.ApplyDesiredState( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnbindInputLayout( )
{
	m_pContext->IASetInputLayout( 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnbindVertexBuffer( )
{	
	ID3D11Buffer* Buffers[] = { 0 };
	UINT Strides[] = { 0 };
	UINT Offsets[] = { 0 };

	m_pContext->IASetVertexBuffers( 0, 1, Buffers, Strides, Offsets );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnbindAllVertexBuffers( )
{
	ID3D11Buffer* Buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { NULL };
	UINT Strides [D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
	UINT Offsets [D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };

	m_pContext->IASetVertexBuffers( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT,
		Buffers, Strides, Offsets );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnbindIndexBuffer( )
{
	m_pContext->IASetIndexBuffer( 0, DXGI_FORMAT_R32_UINT, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindInputLayout( int ID )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	InputLayoutDX11* pLayout = pRenderer->GetInputLayout( ID );

	if ( pLayout )
		m_pContext->IASetInputLayout( pLayout->m_pInputLayout );
	else
		Log::Get().Write( L"Tried to bind an invalid input layout ID!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindVertexBuffer( ResourcePtr resource, UINT stride, UINT offset )
{
	// TODO: Add the ability to set multiple vertex buffers at once, and to 
	//       provide an offset to the data contained in the buffers.

	int index = resource->m_iResource;

	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	RendererDX11* pRenderer = RendererDX11::Get();
	ID3D11Buffer* pBuffer = (ID3D11Buffer*)pRenderer->GetResource( ID )->GetResource();
	
	ID3D11Buffer* Buffers[] = { pBuffer };
	UINT Strides[] = { stride };
	UINT Offsets[] = { offset };

	if ( pBuffer )
	{
		m_pContext->IASetVertexBuffers( 0, 1, Buffers, Strides, Offsets );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid vertex buffer ID!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindVertexBuffers( const ResourcePtr* resources, const UINT* strides,
									 const UINT* offsets, UINT startSlot, UINT numBuffers )
{
	ID3D11Buffer* Buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { NULL };
	RendererDX11* pRenderer = RendererDX11::Get();

	for ( UINT i = 0; i < numBuffers; ++i )
	{
		int index = resources[i]->m_iResource;

		// Select only the index portion of the handle.
		int TYPE	= index & 0x00FF0000;
		int ID		= index & 0x0000FFFF;

		Buffers[i] = reinterpret_cast<ID3D11Buffer*>( pRenderer->GetResource( ID )->GetResource() );
		if ( !Buffers[i] )					
			Log::Get().Write( L"Tried to bind an invalid vertex buffer ID!" );
	}

	m_pContext->IASetVertexBuffers( startSlot, numBuffers, Buffers, strides, offsets );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindIndexBuffer( ResourcePtr resource )
{
	// TODO: Add the ability to use different formats and offsets to this function!
	int index = resource->m_iResource;

	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	RendererDX11* pRenderer = RendererDX11::Get();
	ID3D11Buffer* pBuffer = (ID3D11Buffer*)pRenderer->GetResource( ID )->GetResource();

	// If the resource is in range, then attempt to set it
	if ( pBuffer ) {
		m_pContext->IASetIndexBuffer( pBuffer, DXGI_FORMAT_R32_UINT, 0 );
	}
	else {
		Log::Get().Write( L"Tried to bind an invalid index buffer ID!" );
	}
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

	//this->ApplyPipelineResources( );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineState( )
{
	// TODO: This method needs to update the state of each pipeline stage class!!!

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

	//RasterizerStage

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.SetToDefaultState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
	
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
	// Get the current configuration on the pipeline and print it

	m_pContext->DrawIndexed( IndexCount, StartIndex, VertexOffset );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, GeometryPtr geometry, 
										IParameterManager* pParamManager )
{
	Draw( effect, geometry->m_VB, geometry->m_IB,
		geometry->GetInputLayout( effect.m_iVertexShader ), geometry->m_ePrimType,
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
		geometry->GetInputLayout( effect.m_iVertexShader ),
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

	int Type = args->m_iResource & 0x00FF0000;
	int ID = args->m_iResource & 0x0000FFFF;

	if ( Type =! RT_INDIRECTARGSBUFFER )
	{
		Log::Get().Write( L"Tried to use the wrong resource type for an indirect dispatch!" );
		return;
	}

	ID3D11Buffer* pArgsBuffer = (ID3D11Buffer*)RendererDX11::Get()->GetResource( ID )->GetResource();


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
	int Type = dest->m_iResource & 0x00FF0000;
	int ID = dest->m_iResource & 0x0000FFFF;

	if ( Type =! RT_INDIRECTARGSBUFFER )
	{
		Log::Get().Write( L"Tried to use the wrong resource type for an indirect drawing!" );
		return;
	}

	ID3D11Buffer* pArgsBuffer = (ID3D11Buffer*)RendererDX11::Get()->GetResource( ID )->GetResource();

	UnorderedAccessViewDX11* pView = RendererDX11::Get()->GetUnorderedAccessView( uav->m_iResourceUAV );

	ID3D11UnorderedAccessView* pSrcView = pView->m_pUnorderedAccessView;

	m_pContext->CopyStructureCount( pArgsBuffer, offset, pSrcView );

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

	UINT viewCount = OutputMergerStage.GetViewsSetCount();

	for( UINT i = 0; i < viewCount; ++i )
	{
	    float clearColours[] = { color.x, color.y, color.z, color.w }; // RGBA
	    m_pContext->ClearRenderTargetView( OutputMergerStage.RenderTargetViews[i], clearColours );
	}

	// Check if the output merger currently has a depth target set, and if so clear it.

	if ( OutputMergerStage.DepthTargetViews != 0 )
	{
		m_pContext->ClearDepthStencilView( OutputMergerStage.DepthTargetViews, D3D11_CLEAR_DEPTH, depth, stencil );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShader( ShaderType type, int ID, IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( ID );

	// Check if the shader has a valid identifier
	if ( pShaderDX11 )
	{
		// Perform the actual binding to the pipeline, and then bind all needed
		// parameters.

		if ( pShaderDX11->GetType() == type )
		{
			// Before binding the shader, have it update its required parameters.  These
			// parameters will then be bound to the pipeline after the shader is bound.

			pShaderDX11->UpdateParameters( this, pParamManager );

			// Record the shader ID for use later on
			ShaderStages[type]->DesiredState.SetShaderProgram( ID );

			pShaderDX11->BindParameters( this, pParamManager );
		}
		else
		{
			Log::Get().Write( L"Tried to set the wrong type of shader ID!" );
		}
	}
	else
	{
		if ( ID == -1 )
			this->UnbindShader( type );
		else
			Log::Get().Write( L"Tried to set an invalid shader ID!" );
	}
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( int index, UINT subresource, D3D11_MAP actions, UINT flags )
{
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Resource* pResource = 0;
	D3D11_MAPPED_SUBRESOURCE Data;

	pResource = RendererDX11::Get()->GetResource( ID )->GetResource();

	// Map the resource
	HRESULT hr = m_pContext->Map( pResource, subresource, actions, flags, &Data );
	
	if ( FAILED( hr ) )
		Log::Get().Write( L"Failed to map resource!" );

	return( Data );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( int index, UINT subresource )
{
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Resource* pResource = 0;

	pResource = RendererDX11::Get()->GetResource( ID )->GetResource();

	// Unmap the resource - there is no HRESULT returned, so trust that it works...
	m_pContext->Unmap( pResource, subresource );
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
	// Get the index from the handle
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	// Get the texture as a resource and save it to file
	ID3D11Resource* pResource = RendererDX11::Get()->GetResource( ID )->GetResource();

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
void PipelineManagerDX11::UnbindShader( ShaderType type )
{
	switch( type )
	{
	case VERTEX_SHADER:
		m_pContext->VSSetShader( 0, 0, 0 );
		break;
	case HULL_SHADER:
		m_pContext->HSSetShader( 0, 0, 0 );
		break;
	case DOMAIN_SHADER:
		m_pContext->DSSetShader( 0, 0, 0 );
		break;
	case GEOMETRY_SHADER:
		m_pContext->GSSetShader( 0, 0, 0 );
		break;
	case PIXEL_SHADER:
		m_pContext->PSSetShader( 0, 0, 0 );
		break;
	case COMPUTE_SHADER:
		m_pContext->CSSetShader( 0, 0, 0 );
		break;
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopySubresourceRegion( ResourcePtr DestResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ,
	ResourcePtr SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox )
{
	int DestType = DestResource->m_iResource & 0x00FF0000;
	int DestID = DestResource->m_iResource & 0x0000FFFF;
	ID3D11Resource* pDestResource = RendererDX11::Get()->GetResource(DestID)->GetResource();


	int SrcType = SrcResource->m_iResource & 0x00FF0000;
	int SrcID = SrcResource->m_iResource & 0x0000FFFF;
	ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResource(SrcID)->GetResource();

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
		//
		// TODO: Add the other stage states here...

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

		//RasterizerStage

		PixelShaderStage.ClearCurrentState();
		PixelShaderStage.ClearDesiredState();

		OutputMergerStage.SetToDefaultState();

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

	//RasterizerStage

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.SetToDefaultState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ResolveSubresource( ResourcePtr DestResource, UINT DstSubresource, 
                                              ResourcePtr SrcResource, UINT SrcSubresource, 
                                              DXGI_FORMAT format )
{
    int DestType = DestResource->m_iResource & 0x00FF0000;
    int DestID = DestResource->m_iResource & 0x0000FFFF;
    ID3D11Resource* pDestResource = RendererDX11::Get()->GetResource(DestID)->GetResource();

    int SrcType = SrcResource->m_iResource & 0x00FF0000;
    int SrcID = SrcResource->m_iResource & 0x0000FFFF;
    ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResource(SrcID)->GetResource();
 
    m_pContext->ResolveSubresource( pDestResource, DstSubresource, pSrcResource, SrcSubresource, format );
}
//--------------------------------------------------------------------------------
