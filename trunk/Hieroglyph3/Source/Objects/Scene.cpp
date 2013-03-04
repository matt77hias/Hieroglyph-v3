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
	for ( auto pActor : m_vActors ) {
		SAFE_DELETE( pActor );
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

	for ( auto pCamera : m_vCameras )
	{
		Parameters.InitRenderParams();
		pCamera->RenderFrame( pRenderer );
	}
}
//--------------------------------------------------------------------------------
void Scene::AddCamera( Camera* camera )
{
	m_vCameras.push_back( camera );
	camera->SetScene( this );
	AddActor( camera );
}
//--------------------------------------------------------------------------------
void Scene::AddActor( Actor* actor )
{
	// Link the actor's node into the scene, and then add the actor to the list
	// for cleanup later on.

	m_pRoot->AttachChild( actor->GetNode() );
	m_vActors.push_back( actor );
}
//--------------------------------------------------------------------------------
void Scene::RemoveActor( Actor* pActor )
{
	// First we iterate through the list of actors and remove any instances of 
	// this actor. 

	auto it = m_vActors.begin();

	while ( it != m_vActors.end() ) {
		if ( *it == pActor ) it = m_vActors.erase( it );
	}

	// Now we remove the actor's node from the scene's root node, if it is there.
	// In general, an actor should always be attached directly to the scene's 
	// root, but there may be cases where an actor is attached to another actor.
	// To accommodate both situations, we do the removal by accessing the actor's
	// node's parent.  This ensures that the actor is completely removed from the
	// scene, and can safely be added back into the scene at a later time.

	Node3D* pParent = static_cast<Node3D*>( pActor->GetNode()->GetParent() );
	if ( pParent ) pParent->DetachChild( pActor->GetNode() );
}
//--------------------------------------------------------------------------------
void Scene::BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record )
{
	m_pRoot->BuildPickRecord( ray, record );
}
//--------------------------------------------------------------------------------