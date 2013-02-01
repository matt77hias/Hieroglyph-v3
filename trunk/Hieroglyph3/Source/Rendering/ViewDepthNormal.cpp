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
#include "ViewDepthNormal.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewDepthNormal::ViewDepthNormal( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
	: ViewPerspective( Renderer, RenderTarget, DepthTarget )
{
	// This view is the same as a perspective view, with only a different 
	// rendering type as indicated here.  In addition, the render target
	// that gets passed to this view will serve as the depth/normal buffer
	// as opposed to the standard render target.

	m_sParams.iViewType = VT_LINEAR_DEPTH_NORMAL;

	m_pDepthNormalBuffer = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"DepthNormalBuffer" ) );
}
//--------------------------------------------------------------------------------
ViewDepthNormal::~ViewDepthNormal()
{
}
//--------------------------------------------------------------------------------
void ViewDepthNormal::SetUsageParams( IParameterManager* pParamManager )
{
	// This view will bind the depth/normal buffer to the "DepthNormalBuffer" shader
	// resource view parameter, so that other views can make use of it.

	pParamManager->SetShaderResourceParameter( m_pDepthNormalBuffer, m_RenderTarget );
}
//--------------------------------------------------------------------------------
