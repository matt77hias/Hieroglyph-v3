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
// Actor
//
// The actor class provides a common interface for working with groups of entities
// and the other objects that can be combined with them.  Each instances comes 
// with one node and one entity.  The entity (referred to as the 'body') allows 
// the actor to have a rendered presence in the scene, while the node allows the
// actor to have multiple other entities or nodes to be attached to it.
//
// Whenever an entity or node is attached to an actor, the user has the option of
// adding it as an 'element' to this actor.  This essentially means that the 
// objects will be deleted by the actor when it is deleted.  There is no direct
// link between the scene graph based at the actor's node and the ownership 
// indicated by the element mechanism - entities can be managed as elements 
// regardless of if they are attached to the scene graph or not.
//
// This mechanism allows an actor subclass to manage all its objects in one simple
// way.  If it adds an entity to render the axes of one of its objects, that axis 
// object can be enabled (attached to the graph) or disabled (detached from the 
// graph) but its lifetime will always be managed by the actor once it is added
// as an 'element'.
//--------------------------------------------------------------------------------
#ifndef Actor_h
#define Actor_h
//--------------------------------------------------------------------------------
#include "Node3D.h"
#include "Entity3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Actor
	{
	public:
		Actor();
		virtual ~Actor();

		Node3D* GetNode() const;
		Node3D* GetNode();
		Entity3D* GetBody() const;
		Entity3D* GetBody();


		void AddElement( Entity3D* pElement );
		void RemoveElement( Entity3D* pElement );
		void AddElement( Node3D* pElement );
		void RemoveElement( Node3D* pElement );

		//void AddSubtree( Node3D* pNode );
		//void RemoveSubtree( Node3D* pNode );

	protected:
		Node3D*					m_pRoot;
		Entity3D*				m_pBody;

		std::vector<Entity3D*>	m_EntityElements;
		std::vector<Node3D*>	m_NodeElements;
	};
};
//--------------------------------------------------------------------------------
#endif // Actor_h
