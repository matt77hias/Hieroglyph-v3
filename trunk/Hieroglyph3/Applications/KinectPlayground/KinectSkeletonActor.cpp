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
	GeometryPtr pGeometry = GeometryLoaderDX11::loadMS3DFile2( L"small_box.ms3d" );
    pGeometry->LoadToBuffers();
    pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	MaterialPtr pMaterial = MaterialGeneratorDX11::GeneratePhong( *RendererDX11::Get() );

	// Loop and creat one entity for each joint
	for ( int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++ ) {
		
		// Create an entity, and specify its geometry and material
		Entity3D* pEntity = new Entity3D();
		pEntity->SetGeometry( pGeometry );
		pEntity->SetMaterial( pMaterial );

		// Attach the entities to the node of the actor.
		GetNode()->AttachChild( pEntity );

		// We add the joint entity to a list so that we can access it later on.
		m_Joints.add( pEntity );

		// Adding the entity as an element will automatically release it 
		// when the actor is destroyed.
		AddElement( pEntity );
	}
}
//--------------------------------------------------------------------------------
KinectSkeletonActor::~KinectSkeletonActor()
{
}
//--------------------------------------------------------------------------------
void KinectSkeletonActor::UpdateSkeleton( NUI_SKELETON_DATA* pData )
{
	//std::wstringstream s;
	//s << L"Skeleton Frame: " << std::endl;

	for ( int joint = 0; joint < NUI_SKELETON_POSITION_COUNT; joint++ ) {

		m_Joints[joint]->Position() = Vector3f( pData->SkeletonPositions[joint].x,
												pData->SkeletonPositions[joint].y,
												pData->SkeletonPositions[joint].z );

		//s << L"Joint: "<< joint << L" - " << pData->SkeletonPositions[joint].x << L"," << pData->SkeletonPositions[joint].y << L"," << pData->SkeletonPositions[joint].z << std::endl;
	}

	//Log::Get().Write( s.str() );
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