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
#include "TextureSpaceCameraPositionWriter.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextureSpaceCameraPositionWriter::TextureSpaceCameraPositionWriter()
	: ObjectToTextureSpaceMatrix()
{
	VectorParameterDX11* pVectorParameter = 
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"CameraPositionTS" );

	SetRenderParameterRef( pVectorParameter );
}
//--------------------------------------------------------------------------------
TextureSpaceCameraPositionWriter::~TextureSpaceCameraPositionWriter()
{
}
//--------------------------------------------------------------------------------
void TextureSpaceCameraPositionWriter::SetObjectToTextureSpaceXform( Matrix4f& xform )
{
	ObjectToTextureSpaceMatrix = xform;
}
//--------------------------------------------------------------------------------
void TextureSpaceCameraPositionWriter::WriteParameter( IParameterManager* pParamMgr )
{
	// Get the 'CameraPosition' current value
	Vector4f CameraPosition = pParamMgr->GetVectorParameter( L"ViewPosition" );

	// Get the current world matrix, and invert it
	Matrix4f WorldMatrix = pParamMgr->GetMatrixParameter( L"WorldMatrix" );
	Matrix4f InvWorldMatrix = WorldMatrix.Inverse();

	// Calculate the texture space camera position
	Vector4f CameraPositionTS = InvWorldMatrix * ObjectToTextureSpaceMatrix * CameraPosition;

	// Write the value into the parameter manager as 'CameraPositionTS'
	SetValue( CameraPositionTS );
	VectorParameterWriterDX11::WriteParameter( pParamMgr );
}
//--------------------------------------------------------------------------------
