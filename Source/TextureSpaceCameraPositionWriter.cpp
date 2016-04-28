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
#include "TextureSpaceCameraPositionWriter.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextureSpaceCameraPositionWriter::TextureSpaceCameraPositionWriter()
	: ObjectToTextureSpaceMatrix()
{
	// Get the texture space camera position parameter reference.  This will be
	// used later to set the calculated value later on.

	VectorParameterDX11* pVectorParameter = 
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"CameraPositionTS" );

	SetRenderParameterRef( pVectorParameter );

	// Grab the references to the parameters that we will be using later on.

	m_pCameraPositionParameter =
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"ViewPosition" );
		
	m_pWorldMatrixParameter =
		RendererDX11::Get()->m_pParamMgr->GetMatrixParameterRef( L"WorldMatrix" );
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
	Vector4f CameraPosition = pParamMgr->GetVectorParameter( m_pCameraPositionParameter );

	// Get the current world matrix, and invert it
	Matrix4f WorldMatrix = pParamMgr->GetMatrixParameter( m_pWorldMatrixParameter );
	Matrix4f InvWorldMatrix = WorldMatrix.Inverse();

	// Calculate the texture space camera position
	Vector4f CameraPositionTS = InvWorldMatrix * ObjectToTextureSpaceMatrix * CameraPosition;

	// Write the value into the parameter manager as 'CameraPositionTS'
	SetValue( CameraPositionTS );
	VectorParameterWriterDX11::WriteParameter( pParamMgr );
}
//--------------------------------------------------------------------------------
