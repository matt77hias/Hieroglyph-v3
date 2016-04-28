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
#include "SkinnedActor.h"
#include "MatrixArrayParameterDX11.h"
#include "Log.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "MatrixArrayParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SkinnedActor::SkinnedActor()
{
	m_pMatrices = 0;
	m_pNormalMatrices = 0;

	m_pGeometryEntity = new Entity3D();
}
//--------------------------------------------------------------------------------
SkinnedActor::~SkinnedActor()
{
	// Delete each controller's entity.  This will in turn delete each of the
	// controller's that are attached to the entity.
	for ( auto pController : m_Bones )
	{
		Node3D* pEntity = pController->GetEntity();
		SAFE_DELETE( pEntity );
	}

	if ( m_pMatrices )
		delete [] m_pMatrices;

	if ( m_pNormalMatrices )
		delete [] m_pNormalMatrices;

	SAFE_DELETE( m_pGeometryEntity );
}
//--------------------------------------------------------------------------------
void SkinnedActor::AddBoneNode( Node3D* pBone, Vector3f BindPosition, Vector3f BindRotation,
								AnimationStream<Vector3f>* pPositions, AnimationStream<Vector3f>* pRotations )
{
	if ( pBone )
	{
		// Create a controller to apply all of the animation to the bone.  The 
		// streams and the bind information are loaded into the controller.

		SkinnedBoneController<Node3D>* pController = new SkinnedBoneController<Node3D>();

		pController->SetBindPosition( BindPosition );
		pController->SetBindRotation( BindRotation );

		pController->SetPositionStream( pPositions );
		pController->SetRotationStream( pRotations );

		// Add the controller onto this bone node.  
		pBone->Controllers.Attach( pController );


		// Set the bind pose on the bone.
		pBone->Transform.Position() = BindPosition;
		pBone->Transform.Rotation().Rotation( BindRotation );

		// Store the node in the bones list.
		m_Bones.push_back( pController );



		// For debugging, add the visualization for the bones...

		RendererDX11* pRenderer = RendererDX11::Get();
		
		Entity3D* pAxes = new Entity3D();
		pAxes->Visual.SetMaterial( MaterialGeneratorDX11::GenerateSolidColor( *pRenderer ) );

		GeometryPtr pGeometry = GeometryPtr( new GeometryDX11() );
		GeometryGeneratorDX11::GenerateAxisGeometry( pGeometry );
		pGeometry->LoadToBuffers();
		pAxes->Visual.SetGeometry( pGeometry );

		pBone->AttachChild( pAxes );
	}
}
//--------------------------------------------------------------------------------
void SkinnedActor::SetBindPose( )
{
	// Have each controller record the current world matrix inverse for the bind 
	// pose inverse.

	GetNode()->Update( 0.0f );

	for ( auto pController : m_Bones )
	{
		pController->SetBindPose();
	}


	// Create an array to hold the CPU side matrices.

	m_pMatrices = new Matrix4f[m_Bones.size()];
	m_pNormalMatrices = new Matrix4f[m_Bones.size()];

	// Create and set up the matrix array rendering parameter.  This gets added
	// to the skinned actor's body, which is an entity holding the models geometry.

	GetBody()->Parameters.SetMatrixArrayParameter( L"SkinMatrices", m_pMatrices, m_Bones.size() );
	GetBody()->Parameters.SetMatrixArrayParameter( L"SkinNormalMatrices", m_pNormalMatrices, m_Bones.size() );

}
//--------------------------------------------------------------------------------
void SkinnedActor::SetSkinningMatrices( RendererDX11& Renderer )
{
	// Update the CPU side animation matrices.
	for ( unsigned int i = 0; i < m_Bones.size(); i++ )
	{
		m_pMatrices[i] = m_Bones[i]->GetTransform();
		m_pNormalMatrices[i] = m_Bones[i]->GetNormalTransform();
	}
}
//--------------------------------------------------------------------------------
void SkinnedActor::PlayAnimation( int index )
{
	for ( unsigned int i = 0; i < m_Bones.size(); i++ )
	{
		AnimationStream<Vector3f>* pStream = 0;
		
		pStream = m_Bones[i]->GetPositionStream();
		if ( pStream )
			pStream->PlayAnimation( index );

		pStream = m_Bones[i]->GetRotationStream();
		if ( pStream )
			pStream->PlayAnimation( index );
	}
}
//--------------------------------------------------------------------------------
void SkinnedActor::PlayAnimation( std::wstring& name )
{
	for ( unsigned int i = 0; i < m_Bones.size(); i++ )
	{
		AnimationStream<Vector3f>* pStream = 0;
		
		pStream = m_Bones[i]->GetPositionStream();
		if ( pStream )
			pStream->PlayAnimation( name );

		pStream = m_Bones[i]->GetRotationStream();
		if ( pStream )
			pStream->PlayAnimation( name );
	}
}
//--------------------------------------------------------------------------------
void SkinnedActor::PlayAllAnimations( )
{
	for ( unsigned int i = 0; i < m_Bones.size(); i++ )
	{
		AnimationStream<Vector3f>* pStream = 0;
		
		pStream = m_Bones[i]->GetPositionStream();
		if ( pStream )
			pStream->PlayAllAnimations( );

		pStream = m_Bones[i]->GetRotationStream();
		if ( pStream )
			pStream->PlayAllAnimations();
	}
}
//--------------------------------------------------------------------------------
Entity3D* SkinnedActor::GetGeometryEntity()
{
	return( m_pGeometryEntity );
}
//--------------------------------------------------------------------------------