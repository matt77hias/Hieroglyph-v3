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
#include "RenderEffectDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderEffectDX11::RenderEffectDX11()
{
	m_iVertexShader = -1;
	m_iHullShader = -1;
	m_iDomainShader = -1;
	m_iGeometryShader = -1;
	m_iPixelShader = -1;
	m_iComputeShader = -1;

	m_iBlendState = -1;
	m_iDepthStencilState = -1;
	m_iRasterizerState = -1;
}
//--------------------------------------------------------------------------------
RenderEffectDX11::~RenderEffectDX11()
{

}
//--------------------------------------------------------------------------------
void RenderEffectDX11::ConfigurePipeline( RendererDX11* pRenderer )
{
	// For the standard states, only apply the state change if there is a valid
	// state in this render effect.  If not, use the current state that has already
	// been specified before this effect was bound.

	if ( m_iBlendState != -1 )
		pRenderer->SetBlendState( m_iBlendState );

	if ( m_iDepthStencilState != -1 )
		pRenderer->SetDepthStencilState( m_iDepthStencilState );

	if ( m_iRasterizerState != -1 )
		pRenderer->SetRasterizerState( m_iRasterizerState );

	// For the shaders, we bind each one even if the shader is not valid in this
	// render effect.  This allows for various pipeline stages to be disabled when
	// they are not being used.  The renderer will update each shader's required
	// resources based on the shader reflection information stored in each shader.

	pRenderer->BindShader( VERTEX_SHADER, m_iVertexShader );
	pRenderer->BindShader( HULL_SHADER, m_iHullShader );
	pRenderer->BindShader( DOMAIN_SHADER, m_iDomainShader );
	pRenderer->BindShader( GEOMETRY_SHADER, m_iGeometryShader );
	pRenderer->BindShader( PIXEL_SHADER, m_iPixelShader );
	pRenderer->BindShader( COMPUTE_SHADER, m_iComputeShader );

	// After making the changes to the resources above, the renderer will apply 
	// the changes to the pipeline when a draw/dispatch call is made.
}
//--------------------------------------------------------------------------------