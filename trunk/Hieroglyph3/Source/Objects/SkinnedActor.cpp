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
#include "SkinnedActor.h"
#include "MatrixArrayParameterDX11.h"
#include "Log.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SkinnedActor::SkinnedActor()
{
	m_pMatrixParameter = 0;
	m_pMatrixNormalParameter = 0;
	m_pMatrices = 0;
	m_pNormalMatrices = 0;

	m_pGeometryEntity = new Entity3D();
}
//--------------------------------------------------------------------------------
SkinnedActor::~SkinnedActor()
{
	// Delete each controller's entity.  This will in turn delete each of the
	// controller's that are attached to the entity.
	for ( int i = 0; i < m_Bones.count(); i++ )
	{
		Entity3D* pEntity = m_Bones[i]->GetEntity();
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

		SkinnedBoneController* pController = new SkinnedBoneController();

		pController->SetBindPosition( BindPosition );
		pController->SetBindRotation( BindRotation );
		//pBone->Position() = BindPosition;
		//pBone->Rotation().RotationZYX( BindRotation );

		pController->SetPositionStream( pPositions );
		pController->SetRotationStream( pRotations );

		// Add the controller onto this bone node.  
		pBone->AttachController( pController );
		//pBone->SetLocalMatrixCalculation( false );


		// Set the bind pose on the bone.
		pBone->Position() = BindPosition;
		pBone->Rotation().Rotation( BindRotation );
		//pBone->Rotation().RotationZYX( BindRotation );

		// Store the node in the bones list.
		m_Bones.add( pController );



		// For debugging, add the visualization for the bones...

		RendererDX11* pRenderer = RendererDX11::Get();
		
		Entity3D* pAxes = new Entity3D();
		pAxes->SetMaterial( MaterialGeneratorDX11::GenerateSolidColor( *pRenderer ) );

		GeometryDX11* pGeometry = new GeometryDX11();
		GeometryGeneratorDX11::GenerateAxisGeometry( pGeometry );
		pGeometry->LoadToBuffers();
		pAxes->SetGeometry( pGeometry );

		pBone->AttachChild( pAxes );
	}
}
//--------------------------------------------------------------------------------
void SkinnedActor::SetBindPose( )
{

	// Have each controller record the current world matrix inverse for the bind 
	// pose inverse.
	for ( int i = 0; i < m_Bones.count(); i++ )
	{
		m_Bones[i]->SetLocalSkeleton();
		//m_Bones[i]->SetGlobalSkeleton();
		//m_Bones[i]->SetBindPose();
	}

	// Update the model's world matrices.

	for ( int i = 0; i < m_Bones.count(); i++ )
	{
		//m_Bones[i]->SetLocalSkeleton();
		m_Bones[i]->SetGlobalSkeleton();
		//m_Bones[i]->SetBindPose();
	}

	GetNode()->Update( 0.0f );

	for ( int i = 0; i < m_Bones.count(); i++ )
	{
		//m_Bones[i]->SetLocalSkeleton();
		//m_Bones[i]->SetGlobalSkeleton();
		m_Bones[i]->SetBindPose();
	}


	// Create the matrix array parameter, and add it to the entity.
	if ( m_pMatrixParameter )
		Log::Get().Write( L"Bitch about someone calling 'SetBindPose' more than once..." );

	// Create and set up the matrix array rendering parameter.  This gets added
	// to the skinned actor's body, which is an entity holding the models geometry.
	m_pMatrixParameter = new MatrixArrayParameterDX11( m_Bones.count() );
	m_pMatrixParameter->SetName( std::wstring( L"SkinMatrices" ) );
	GetBody()->AddRenderParameter( m_pMatrixParameter );

	m_pMatrixNormalParameter= new MatrixArrayParameterDX11( m_Bones.count() );
	m_pMatrixNormalParameter->SetName( std::wstring( L"SkinNormalMatrices" ) );
	GetBody()->AddRenderParameter( m_pMatrixNormalParameter );
	
	// Create an array to hold the CPU side matrices.
	m_pMatrices = new Matrix4f[m_Bones.count()];
	m_pNormalMatrices = new Matrix4f[m_Bones.count()];
}
//--------------------------------------------------------------------------------
void SkinnedActor::SetSkinningMatrices( RendererDX11& Renderer )
{
	// Update the CPU side animation matrices.
	for ( int i = 0; i < m_Bones.count(); i++ )
	{
		m_pMatrices[i] = m_Bones[i]->GetTransform();
		m_pNormalMatrices[i] = m_Bones[i]->GetNormalTransform();
	}

	// Set the data into the GPU parameter list.
	m_pMatrixParameter->SetParameterData( m_pMatrices );
	m_pMatrixNormalParameter->SetParameterData( m_pNormalMatrices );
}
//--------------------------------------------------------------------------------
void SkinnedActor::PlayAnimation( int index )
{
	for ( int i = 0; i < m_Bones.count(); i++ )
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
	for ( int i = 0; i < m_Bones.count(); i++ )
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
	for ( int i = 0; i < m_Bones.count(); i++ )
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