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
#include "ObjectSpaceCameraPositionWriter.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ObjectSpaceCameraPositionWriter::ObjectSpaceCameraPositionWriter()
{
	// Get the object space camera position parameter reference.  This will be
	// used later to set the calculated value later on.

	VectorParameterDX11* pVectorParameter = 
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"CameraPositionOS" );

	SetRenderParameterRef( pVectorParameter );

	// Grab the references to the parameters that we will be using later on.

	m_pCameraPositionParameter =
		RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( L"ViewPosition" );
		
	m_pWorldMatrixParameter =
		RendererDX11::Get()->m_pParamMgr->GetMatrixParameterRef( L"WorldMatrix" );
}
//--------------------------------------------------------------------------------
ObjectSpaceCameraPositionWriter::~ObjectSpaceCameraPositionWriter()
{
}
//--------------------------------------------------------------------------------
void ObjectSpaceCameraPositionWriter::WriteParameter( IParameterManager* pParamMgr )
{
	// Get the 'CameraPosition' current value
	Vector4f CameraPosition = pParamMgr->GetVectorParameter( m_pCameraPositionParameter );

	// Get the current world matrix, and invert it
	Matrix4f WorldMatrix = pParamMgr->GetMatrixParameter( m_pWorldMatrixParameter );
	Matrix4f InvWorldMatrix = WorldMatrix.Inverse();

	// Calculate the object space camera position
	Vector4f CameraPositionOS = InvWorldMatrix * CameraPosition;

	// Write the value into the parameter manager as 'CameraPositionOS'
	SetValue( CameraPositionOS );
	VectorParameterWriterDX11::WriteParameter( pParamMgr );
}
//--------------------------------------------------------------------------------
