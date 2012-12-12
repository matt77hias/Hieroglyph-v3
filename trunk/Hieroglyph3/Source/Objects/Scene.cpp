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
#include "Scene.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Scene::Scene()
{
	m_pRoot = new Node3D();
}
//--------------------------------------------------------------------------------
Scene::~Scene()
{
	// Delete all the actors that have been added to the scene.
	for ( int i = 0; i < m_vActors.count(); i++ ) {
		SAFE_DELETE( m_vActors[i] );
	}

	delete m_pRoot;
}
//--------------------------------------------------------------------------------
Node3D* Scene::GetRoot()
{
	return( m_pRoot );
}
//--------------------------------------------------------------------------------
void Scene::Update( float time )
{
	// Perform the udpate on the root, which will propagate through the scene
	// and update all entities in the scene.

	m_pRoot->Update( time );
}
//--------------------------------------------------------------------------------
void Scene::Render( RendererDX11* pRenderer )
{
	// Render here!  Each camera should use it's render view to create an
	// appropriate image.

	for ( int i = 0; i < m_vCameras.count(); i++ )
	{
		Parameters.InitRenderParams();
		m_vCameras[i]->RenderFrame( pRenderer );
	}
}
//--------------------------------------------------------------------------------
void Scene::AddCamera( Camera* camera )
{
	m_vCameras.add( camera );
	camera->SetScene( this );
	AddActor( camera );
}
//--------------------------------------------------------------------------------
void Scene::AddActor( Actor* actor )
{
	// Link the actor's node into the scene, and then add the actor to the list
	// for cleanup later on.

	m_pRoot->AttachChild( actor->GetNode() );
	m_vActors.add( actor );
}
//--------------------------------------------------------------------------------
void Scene::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	m_pRoot->BuildPickRecord( ray, record );
}
//--------------------------------------------------------------------------------