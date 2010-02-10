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
#include "ISceneInterface.h"
#include <sstream>
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ISceneInterface::ISceneInterface()
{
	m_pRoot = new Node3D();
}
//--------------------------------------------------------------------------------
ISceneInterface::~ISceneInterface()
{
	delete m_pRoot;
}
//--------------------------------------------------------------------------------
Node3D* ISceneInterface::GetRoot()
{
	return( m_pRoot );
}
//--------------------------------------------------------------------------------
void ISceneInterface::Update( float time )
{
	// Perform the udpate on the root, which will propagate through the scene
	// and update all entities in the scene.

	m_pRoot->Update( time );
}
//--------------------------------------------------------------------------------
void ISceneInterface::Render( )
{
	// Render here!  Each camera should use it's render view to create an
	// appropriate image.

	for ( int i = 0; i < m_vCameras.count(); i++ )
		m_vCameras[i]->RenderFrame();
}
//--------------------------------------------------------------------------------
void ISceneInterface::AddCamera( ICameraInterface* camera )
{
	m_vCameras.add( camera );
	camera->SetScene( this );
	AddEntity( camera->GetNode() );
}
//--------------------------------------------------------------------------------
void ISceneInterface::AddEntity( Entity3D* entity )
{
	// TODO: Use a more sophisticated system for tracking the entities.  This 
	//       could eventually allow for automatic destruction instead of manually
	//       having to release each entity.

	m_pRoot->AttachChild( entity );
}
//--------------------------------------------------------------------------------
void ISceneInterface::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	m_pRoot->BuildPickRecord( ray, record );
}
//--------------------------------------------------------------------------------