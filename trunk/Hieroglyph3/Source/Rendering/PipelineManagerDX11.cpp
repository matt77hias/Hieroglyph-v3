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
#include "ParameterManagerDX11.h"
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



#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PipelineManagerDX11::PipelineManagerDX11()
{
	m_pContext = 0;
	m_pQuery = 0;

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
	SAFE_RELEASE( m_pQuery );
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
void PipelineManagerDX11::BindConstantBufferParameter( ShaderType type, const std::wstring& name, UINT slot, 
                                                      ParameterManagerDX11* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];

	if ( pParameter != 0 )
	{
		// Check the type of the parameter
		if ( pParameter->GetParameterType() == CBUFFER )
		{
			ConstantBufferParameterDX11* pBuffer = reinterpret_cast<ConstantBufferParameterDX11*>( pParameter );
			int ID = ( pBuffer->GetIndex() & 0xffff ); 

			ResourceDX11* pResource = pRenderer->GetResource( ID );

			// Allow a range including -1 up to the number of resources
			if ( pResource || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type
				
				ID3D11Buffer* pBuffer = 0;
				
				if ( ID >= 0 )
					pBuffer = (ID3D11Buffer*)pResource->GetResource();

				ShaderStages[type]->SetConstantBuffer( slot, pBuffer );
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
void PipelineManagerDX11::BindShaderResourceParameter( ShaderType type, const std::wstring& name, UINT slot, 
                                                      ParameterManagerDX11* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];

	if ( pParameter != 0 )
	{
		// Check the type of the parameter
		if ( pParameter->GetParameterType() == SHADER_RESOURCE )
		{
			ShaderResourceParameterDX11* pResource = 
				reinterpret_cast<ShaderResourceParameterDX11*>( pParameter );

			int ID = pResource->GetIndex(); 

			ShaderResourceViewDX11* pView = pRenderer->GetShaderResourceView( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11ShaderResourceView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = pView->m_pShaderResourceView;

				ShaderStages[type]->SetShaderResourceView( slot, pResourceView );
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
void PipelineManagerDX11::BindUnorderedAccessParameter( ShaderType type, const std::wstring& name, UINT slot, 
                                                       ParameterManagerDX11* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];

	if ( pParameter != 0 )
	{
		// Check the type of the parameter
		if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
		{
			UnorderedAccessParameterDX11* pResource = 
				reinterpret_cast<UnorderedAccessParameterDX11*>( pParameter );

			int ID = pResource->GetIndex(); 
			UnorderedAccessViewDX11* pView = pRenderer->GetUnorderedAccessView( ID );

			// Allow a range including -1 up to the number of resources views
			if ( pView || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11UnorderedAccessView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = pView->m_pUnorderedAccessView;

				ShaderStages[type]->SetUnorderedAccessView( slot, pResourceView );
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
		Log::Get().Write( L"Tried to set a non-existing parameter as a unordered access view!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindSamplerStateParameter( ShaderType type, const std::wstring& name, UINT slot, 
                                                    ParameterManagerDX11* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	RenderParameterDX11* pParameter = pParamManager->m_Parameters[name];

	if ( pParameter != 0 )
	{
		// Check the type of the parameter
		if ( pParameter->GetParameterType() == SAMPLER )
		{
			SamplerParameterDX11* pResource = 
				reinterpret_cast<SamplerParameterDX11*>( pParameter );

			int ID = pResource->GetIndex(); 
			SamplerStateDX11* pState = pRenderer->GetSamplerState( ID );

			// Allow a range including -1 up to the number of samplers

			if ( pState || ( ID == -1 ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11SamplerState* pSampler = 0;

				if ( ID >= 0 )
					pSampler = pState->m_pState; 

				ShaderStages[type]->SetSamplerState( slot, pSampler );
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
void PipelineManagerDX11::BindRenderTargets( int index, ResourcePtr RenderTarget )
{
	int RenderID = RenderTarget->m_iResourceRTV;

	RendererDX11* pRenderer = RendererDX11::Get();
	
	RenderTargetViewDX11* pView = pRenderer->GetRenderTargetView( RenderID );

	if ( pView )
	{
		ID3D11RenderTargetView* pRenderTarget = { pView->m_pRenderTargetView };

		OutputMergerStage.SetRenderTargetView( index, pRenderTarget );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid render target view!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindDepthTarget( ResourcePtr DepthTarget )
{
	int DepthID = DepthTarget->m_iResourceDSV;

	RendererDX11* pRenderer = RendererDX11::Get();
	
	DepthStencilViewDX11* pView = pRenderer->GetDepthStencilView( DepthID );

	if ( pView )
	{
		ID3D11DepthStencilView* pDepthStencilView = pView->m_pDepthStencilView;

		OutputMergerStage.SetDepthStencilView( pDepthStencilView );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid depth stencil view!" );
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
	if ( pBuffer )
	{
		//ID3D11Buffer* Buffers = 0;
		//Buffers = (ID3D11Buffer*)m_vResources[ID]->GetResource();
		m_pContext->IASetIndexBuffer( pBuffer, DXGI_FORMAT_R32_UINT, 0 );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid index buffer ID!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyPipelineResources( )
{
	VertexShaderStage.BindResources( m_pContext );
	HullShaderStage.BindResources( m_pContext );
	DomainShaderStage.BindResources( m_pContext );
	GeometryShaderStage.BindResources( m_pContext );
	PixelShaderStage.BindResources( m_pContext );
	ComputeShaderStage.BindResources( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineResources( )
{
	VertexShaderStage.UnbindResources( m_pContext );
	HullShaderStage.UnbindResources( m_pContext );
	DomainShaderStage.UnbindResources( m_pContext );
	GeometryShaderStage.UnbindResources( m_pContext );
	PixelShaderStage.UnbindResources( m_pContext );
	ComputeShaderStage.UnbindResources( m_pContext );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, GeometryDX11& geometry, 
										ParameterManagerDX11* pParamManager )
{
	Draw( effect, geometry.m_VB, geometry.m_IB,
		geometry.GetInputLayout( effect.m_iVertexShader ), geometry.m_ePrimType,
		geometry.GetVertexSize(), geometry.GetIndexCount(), pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, ResourcePtr vb, ResourcePtr ib,
						int inputLayput, D3D11_PRIMITIVE_TOPOLOGY primType,
						UINT vertexStride, UINT numIndices, ParameterManagerDX11* pParamManager )
{
	// Specify the type of geometry that we will be dealing with.
	m_pContext->IASetPrimitiveTopology( primType );

	// Bind the vertex and index buffers.
	BindVertexBuffer( vb, vertexStride );
	BindIndexBuffer( ib );

	// Bind the input layout.
	BindInputLayout( inputLayput );

	// Use the effect to load all of the pipeline stages here.
	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();
	
	m_pContext->DrawIndexed( numIndices, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, GeometryDX11& geometry,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, ParameterManagerDX11* pParamManager )
{
	DrawInstanced( effect, geometry.m_VB, geometry.m_ePrimType, geometry.m_IB,
		geometry.GetInputLayout( effect.m_iVertexShader ),
		geometry.GetVertexSize(), geometry.GetIndexCount(),
		instanceData, instanceDataStride, numInstances, pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, ResourcePtr vb,
								 D3D11_PRIMITIVE_TOPOLOGY primType, ResourcePtr ib,
								 int inputLayout, UINT vertexStride, UINT numIndices,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, ParameterManagerDX11* pParamManager )
{
	// Specify the type of geometry that we will be dealing with.

	m_pContext->IASetPrimitiveTopology( primType );

	// Bind the vertex and index buffers.
	ResourcePtr vertexBuffers[2] = { vb, instanceData };
	UINT strides[2] = { vertexStride, instanceDataStride };
	UINT offsets[2] = { 0, 0 };
	BindVertexBuffers( vertexBuffers, strides, offsets, 0, 2 );
	BindIndexBuffer( ib );


	// Bind the input layout.  The layout will be automatically generated if it
	// doesn't already exist.
	BindInputLayout( inputLayout );

	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->DrawIndexedInstanced( numIndices, numInstances, 0, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z, ParameterManagerDX11* pParamManager )
{
	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->Dispatch( x, y, z );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearBuffers( Vector4f color, float depth, UINT stencil )
{
	// Get the current render target view and depth stencil view from the OM stage.
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { NULL };
	ID3D11DepthStencilView* pDepthStencilView = 0;

	m_pContext->OMGetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, pRenderTargetViews, &pDepthStencilView );

	for( UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
	{
        if( pRenderTargetViews[i] != NULL )
        {
		    float clearColours[] = { color.x, color.y, color.z, color.w }; // RGBA
		    m_pContext->ClearRenderTargetView( pRenderTargetViews[i], clearColours );
            SAFE_RELEASE( pRenderTargetViews[i] );
        }
	}

	if ( pDepthStencilView )
	{
		m_pContext->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH, depth, stencil );
	}

	// Release the two views	
	SAFE_RELEASE( pDepthStencilView );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShader( ShaderType type, int ID, ParameterManagerDX11* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( ID );

	// Check if the shader has a valid identifier
	if ( pShaderDX11 )
	{
		// Before binding the shader, have it update its required parameters.  These
		// parameters will then be bound to the pipeline after the shader is bound.

		pShaderDX11->UpdateParameters( this, pParamManager );

		// Perform the actual binding to the pipeline, and then bind all needed
		// parameters.

		if ( pShaderDX11->GetType() == type )
		{
			switch( type )
			{
				case VERTEX_SHADER:
				{
					ID3D11VertexShader* pShader = reinterpret_cast<VertexShaderDX11*>( pShaderDX11 )->m_pVertexShader;
					m_pContext->VSSetShader( pShader, 0, 0 );
					break;
				}
				case HULL_SHADER:
				{
					ID3D11HullShader* pShader = reinterpret_cast<HullShaderDX11*>( pShaderDX11 )->m_pHullShader;
					m_pContext->HSSetShader( pShader, 0, 0 );
					break;
				}
				case DOMAIN_SHADER:
				{
					ID3D11DomainShader* pShader = reinterpret_cast<DomainShaderDX11*>( pShaderDX11 )->m_pDomainShader;
					m_pContext->DSSetShader( pShader, 0, 0 );
					break;
				}
				case GEOMETRY_SHADER:
				{
					ID3D11GeometryShader* pShader = reinterpret_cast<GeometryShaderDX11*>( pShaderDX11 )->m_pGeometryShader;
					m_pContext->GSSetShader( pShader, 0, 0 );
					break;
				}
				case PIXEL_SHADER:
				{
					ID3D11PixelShader* pShader = reinterpret_cast<PixelShaderDX11*>( pShaderDX11 )->m_pPixelShader;
					m_pContext->PSSetShader( pShader, 0, 0 );
					break;
				}
				case COMPUTE_SHADER:
				{
					ID3D11ComputeShader* pShader = reinterpret_cast<ComputeShaderDX11*>( pShaderDX11 )->m_pComputeShader;
					m_pContext->CSSetShader( pShader, 0, 0 );
					break;
				}
			}

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
	if ( m_pQuery )
		m_pContext->Begin( m_pQuery );
	else
		Log::Get().Write( L"Tried to begin pipeline statistics without a query object!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::EndPipelineStatistics( )
{
	if ( m_pQuery )
		m_pContext->End( m_pQuery );
	else
		Log::Get().Write( L"Tried to end pipeline statistics without a valid query object!" );
}
//--------------------------------------------------------------------------------
std::wstring PipelineManagerDX11::PrintPipelineStatistics( )
{
	D3D11_QUERY_DATA_PIPELINE_STATISTICS QueryData; // This data type is different depending on the query type

	if ( S_OK == m_pContext->GetData( m_pQuery, &QueryData, sizeof(QueryData), 0) )
	{
		std::wstringstream s;
		s << "Pipeline Statistics:" << std::endl;
		s << "Number of vertices read by the IA: " << QueryData.IAVertices << std::endl;
		s << "Number of primitives read by the IA: " << QueryData.IAPrimitives << std::endl;
		s << "Number of vertex shader invocations: " << QueryData.VSInvocations << std::endl;
		s << "Number of hull shader invocations: " << QueryData.HSInvocations << std::endl;
		s << "Number of domain shader invocations: " << QueryData.DSInvocations << std::endl;
		s << "Number of geometry shader invocations: " << QueryData.GSInvocations << std::endl;
		s << "Number of primitives output by the geometry shader: " << QueryData.GSPrimitives << std::endl;
		s << "Number of primitives sent to the rasterizer: " << QueryData.CInvocations << std::endl;
		s << "Number of primitives rendered: " << QueryData.CPrimitives << std::endl;
		s << "Number of pixel shader invocations: " << QueryData.PSInvocations << std::endl;

		return( s.str() );
	}

	return( std::wstring( L"Statistics currently not available..." ) );
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
		m_pContext->FinishCommandList( true, &pList->m_pList );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ExecuteCommandList( CommandListDX11* pList )
{
	if ( pList->ListAvailable() )
		m_pContext->ExecuteCommandList( pList->m_pList, true );
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
