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
#include "Actor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Actor::Actor()
{
	m_pRoot = new Node3D();
	m_pBody = new Entity3D();
	m_pRoot->AttachChild( m_pBody );

	// Add the root and body to the element list for cleanup later on.
	AddElement( m_pRoot );
	AddElement( m_pBody );
}
//--------------------------------------------------------------------------------
Actor::~Actor()
{
	for ( auto pEntity : m_EntityElements )
		SAFE_DELETE( pEntity );

	for ( auto pNode : m_NodeElements )
		SAFE_DELETE( pNode );
}
//--------------------------------------------------------------------------------
Node3D* Actor::GetNode() const
{
	return( m_pRoot );
}
//--------------------------------------------------------------------------------
Entity3D* Actor::GetBody() const
{
	return( m_pBody );
}
//--------------------------------------------------------------------------------
Node3D* Actor::GetNode()
{
	return( m_pRoot );
}
//--------------------------------------------------------------------------------
Entity3D* Actor::GetBody()
{
	return( m_pBody );
}
//--------------------------------------------------------------------------------
void Actor::AddElement( Entity3D* pElement )
{
	// Add the entity to the element list, so that it's lifetime will be 
	// controlled by the actor.  This does not do anything with the connection
	// to the scene graph, so beware!

	m_EntityElements.push_back( pElement );
}
//--------------------------------------------------------------------------------
void Actor::RemoveElement( Entity3D* pElement )
{
	// Here we iterate through the list of actors and remove any instances of 
	// this actor. 

	auto it = m_EntityElements.begin();

	while ( it != m_EntityElements.end() ) {
		if ( *it == pElement ) 
			it = m_EntityElements.erase( it );
		else
			it++;
	}

	// Removing an element from this list does not automatically remove any sub-
	// tree objects.  For example, if a node is directly removed using this 
	// method, its children are not automatically removed.  If this is what you 
	// want, then you should acquire a list of the children, and iterate over it
	// to remove each entity.  This functionality is provided in the 'sub-tree'
	// methods for convenience.
}
//--------------------------------------------------------------------------------
void Actor::AddElement( Node3D* pElement )
{
	// Add the entity to the element list, so that it's lifetime will be 
	// controlled by the actor.  This does not do anything with the connection
	// to the scene graph, so beware!

	m_NodeElements.push_back( pElement );
}
//--------------------------------------------------------------------------------
void Actor::RemoveElement( Node3D* pElement )
{
	// Here we iterate through the list of actors and remove any instances of 
	// this actor. 

	auto it = m_NodeElements.begin();

	while ( it != m_NodeElements.end() ) {
		if ( *it == pElement ) 
			it = m_NodeElements.erase( it );
		else
			it++;
	}

	// Removing an element from this list does not automatically remove any sub-
	// tree objects.  For example, if a node is directly removed using this 
	// method, its children are not automatically removed.  If this is what you 
	// want, then you should acquire a list of the children, and iterate over it
	// to remove each entity.  This functionality is provided in the 'sub-tree'
	// methods for convenience.
}
//--------------------------------------------------------------------------------
//void Actor::AddSubtree( Node3D* pNode )
//{
//	// Acquire a list of all the entities within this subtree.
//
//	std::vector<Entity3D*> entities;
//	pNode->GetEntities( entities );
//
//	// Add each of these entities to the 'elements' list to manage their lifetimes.
//
//	for ( auto pEntity : entities ) {
//		AddElement( pEntity );
//	}
//
//	// NOTE: Any changes to the sub-tree will not be reflected in the element list
//	//       after this call!  If you change a subtree, it is up to you to put
//	//       the resulting changes in the element list!
//}
////--------------------------------------------------------------------------------
//void Actor::RemoveSubtree( Node3D* pNode )
//{
//	// Acquire a list of all the entities within this subtree.
//
//	std::vector<Entity3D*> entities;
//	pNode->GetEntities( entities );
//
//	// Remove each of these entities from the 'elements' list to no longer manage 
//	// their lifetimes.
//
//	for ( auto pEntity : entities ) {
//		RemoveElement( pEntity );
//	}
//
//	// NOTE: Any changes to the sub-tree will not be reflected in the element list
//	//       after this call!  If you change a subtree, it is up to you to put
//	//       the resulting changes in the element list!
//}
//--------------------------------------------------------------------------------
