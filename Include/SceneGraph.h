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
// SceneGraph
//
//--------------------------------------------------------------------------------
#ifndef SceneGraph_h
#define SceneGraph_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Entity3D.h"
#include "Node3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// Returns the next node up in the scene graph.
	template <typename T>
	Node3D* GetParent( T* element ) {
		
		assert( element != nullptr );
		
		return element->GetParent();	
	};

	// Returns the last node in the graph which does not have a parent.
	template <typename T>
	Node3D* GetRoot( T* element ) {

		assert( element != nullptr );

		Node3D* parent = GetParent( element );

		while ( parent != nullptr ) {
			element = parent;
			parent = GetParent( element );
		}

		return element;
	}

	
	template <typename T>
	int GraphDepth( T* element ) {

		assert( element != nullptr );

		Node3D* parent = GetParent( element );

		if ( parent == nullptr )
			return( 0 );
		else
			return( GraphDepth( parent ) + 1 );
	}


	void GetAllEntities( Node3D* node, std::vector< Entity3D* >& set );

	// The pick record is the correct way to build a list of the entities that are 
	// intersecting the ray.  The other two methods are just as valid, but perform
	// a different type of query than the pick record.

	void BuildPickRecord( Node3D* node, const Ray3f& ray, std::vector<PickRecord>& record );
	bool EntityInSubTree( Node3D* node, Entity3D* entity );
	void GetIntersectingEntities( Node3D* node, std::vector< Entity3D* >& set, Sphere3f& bounds );
	void GetIntersectingEntities( Node3D* node, std::vector< Entity3D* >& set, Frustum3f& bounds );
};
//--------------------------------------------------------------------------------
#endif // SceneGraph_h
//--------------------------------------------------------------------------------
