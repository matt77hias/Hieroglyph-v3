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
#include "TextureSpaceLightPositionWriter.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextureSpaceLightPositionWriter::TextureSpaceLightPositionWriter()
	: ObjectToTextureSpaceMatrix()
{
	// Get the texture space light position parameter reference.  This will be
	// used later to set the calculated value later on.

	VectorParameterDX11* pVectorParameter = 
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"LightPositionTS" );

	SetRenderParameterRef( pVectorParameter );

	// Grab the references to the parameters that we will be using later on.

	m_pLightPositionParameter =
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"LightPosition" );
		
	m_pWorldMatrixParameter =
		RendererDX11::Get()->m_pParamMgr->GetMatrixParameterRef( L"WorldMatrix" );
}
//--------------------------------------------------------------------------------
TextureSpaceLightPositionWriter::~TextureSpaceLightPositionWriter()
{
}
//--------------------------------------------------------------------------------
void TextureSpaceLightPositionWriter::SetObjectToTextureSpaceXform( Matrix4f& xform )
{
	ObjectToTextureSpaceMatrix = xform;
}
//--------------------------------------------------------------------------------
void TextureSpaceLightPositionWriter::WriteParameter( IParameterManager* pParamMgr )
{
	// Get the 'LightPosition' current value
	Vector4f LightPosition = pParamMgr->GetVectorParameter( m_pLightPositionParameter );

	// Get the current world matrix, and invert it
	Matrix4f WorldMatrix = pParamMgr->GetMatrixParameter( m_pWorldMatrixParameter );
	Matrix4f InvWorldMatrix = WorldMatrix.Inverse();

	// Calculate the texture space light position
	Vector4f LightPositionTS = InvWorldMatrix * ObjectToTextureSpaceMatrix * LightPosition;

	// Write the value into the parameter manager as 'LightPositionTS'
	SetValue( LightPositionTS );
	VectorParameterWriterDX11::WriteParameter( pParamMgr );
}
//--------------------------------------------------------------------------------
