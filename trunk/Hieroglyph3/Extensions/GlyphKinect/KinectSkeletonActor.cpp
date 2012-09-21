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
#include "KinectSkeletonActor.h"
#include "RendererDX11.h"
#include "Log.h"
#include "GeometryLoaderDX11.h"
#include "MaterialGeneratorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
KinectSkeletonActor::KinectSkeletonActor()
{
	// Utilize a common geometry and material for our joints
	//GeometryPtr pGeometry = GeometryLoaderDX11::loadMS3DFile2( L"small_box.ms3d" );
    //pGeometry->LoadToBuffers();
    //pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//MaterialPtr pMaterial = MaterialGeneratorDX11::GeneratePhong( *RendererDX11::Get() );

	// Loop and creat one entity for each joint
	//for ( int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++ ) {
		
		// Create an entity, and specify its geometry and material
		//Entity3D* pEntity = new Entity3D();
		//pEntity->SetGeometry( pGeometry );
		//pEntity->SetMaterial( pMaterial );

		// Attach the entities to the node of the actor.
		//GetNode()->AttachChild( pEntity );

		// We add the joint entity to a list so that we can access it later on.
		//m_Joints.add( pEntity );

		// Adding the entity as an element will automatically release it 
		// when the actor is destroyed.
		//AddElement( pEntity );
	//}
}
//--------------------------------------------------------------------------------
KinectSkeletonActor::~KinectSkeletonActor()
{
}
//--------------------------------------------------------------------------------
void KinectSkeletonActor::UpdateSkeleton( NUI_SKELETON_DATA* pData )
{
	ResetGeometry();
	SetColor( Vector4f( 1.0f, 1.0f, 0.0f, 0.0f ) );

	Vector3f p1, p2;

	for ( int joint = 0; joint < NUI_SKELETON_POSITION_COUNT; joint++ ) {

		p1 = Vector3f( pData->SkeletonPositions[joint].x,
					   pData->SkeletonPositions[joint].y,
					   pData->SkeletonPositions[joint].z );

		DrawSphere( p1, 0.03f );
	}

	SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ) );

	// Head to hips
	DrawLinkage( pData, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER );
	DrawLinkage( pData, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE );
	DrawLinkage( pData, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER );

	// Left arm
	DrawLinkage( pData, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT );

	// Right arm
	DrawLinkage( pData, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT );

	// Left leg
	DrawLinkage( pData, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT );

	// Right leg
	DrawLinkage( pData, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT );
	DrawLinkage( pData, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT );
}
//--------------------------------------------------------------------------------
void KinectSkeletonActor::DrawLinkage( NUI_SKELETON_DATA* pData, NUI_SKELETON_POSITION_INDEX i1, NUI_SKELETON_POSITION_INDEX i2 )
{
	Vector3f p1, p2;

	p1 = Vector3f( pData->SkeletonPositions[i1].x,
				   pData->SkeletonPositions[i1].y,
				   pData->SkeletonPositions[i1].z );
	
	p2 = Vector3f( pData->SkeletonPositions[i2].x,
				   pData->SkeletonPositions[i2].y,
				   pData->SkeletonPositions[i2].z );

	DrawCylinder( p1, p2, 0.015f, 0.015f );
}
//--------------------------------------------------------------------------------
Entity3D* KinectSkeletonActor::GetJointEntity( unsigned int joint )
{
	if ( joint < NUI_SKELETON_POSITION_COUNT ) {
		return( m_Joints[joint] );
	} else {
		return( 0 );
	}
}
//--------------------------------------------------------------------------------