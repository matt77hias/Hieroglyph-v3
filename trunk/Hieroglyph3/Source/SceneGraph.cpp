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
#include "SceneGraph.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
void Glyph3::GetAllEntities( Node3D* node, std::vector< Entity3D* >& set ) {

	set.reserve( set.size() + node->Leafs().size() );
	set.insert( set.end(), node->Leafs().begin(), node->Leafs().end() );

	// Get all of the leafs from this node, then decend to its children
	for ( auto n : node->Nodes() ) {
		GetAllEntities( n, set );
	}
}
//--------------------------------------------------------------------------------
void Glyph3::GetIntersectingEntities( Node3D* node, std::vector< Entity3D* >& set, Frustum3f& bounds )
{
	// TODO: THis has to be fixed!
	//if ( bounds.Intersects( GetWorldBoundingSphere() ) )
	//	set.push_back( this );
}
//--------------------------------------------------------------------------------
void Glyph3::GetIntersectingEntities( Node3D* node, std::vector< Entity3D* >& set, Sphere3f& bounds )
{
	// TODO: THis has to be fixed!
	//if ( bounds.Intersects( GetWorldBoundingSphere() ) )
	//	set.push_back( this );
}
//--------------------------------------------------------------------------------
void Glyph3::BuildPickRecord( Node3D* node, const Ray3f& ray, std::vector<PickRecord>& record )
{
	// Get a flat list of all the entities in this node's subtree.
	std::vector<Entity3D*> set;
	GetAllEntities( node, set );

	for ( auto& entity : set )
	{
		if ( entity->Shape.GetNumberOfShapes() > 0 )
		{
			Matrix4f InvWorld = entity->Transform.WorldMatrix().Inverse();
			Vector4f position = Vector4f( ray.Origin.x, ray.Origin.y, ray.Origin.z, 1.0f );
			Vector4f direction = Vector4f( ray.Direction.x, ray.Direction.y, ray.Direction.z, 0.0f );
		
			position = InvWorld * position;
			direction = InvWorld * direction;

			Ray3f ObjectRay;
			ObjectRay.Origin.x = position.x;
			ObjectRay.Origin.y = position.y;
			ObjectRay.Origin.z = position.z;
			ObjectRay.Direction.x = direction.x;
			ObjectRay.Direction.y = direction.y;
			ObjectRay.Direction.z = direction.z;

			float fT = 10000000000.0f;
			if ( entity->Shape.RayIntersection( ObjectRay, &fT ) )
			{
				PickRecord Record;
				Record.pEntity = entity;
				Record.fDistance = fT;
				record.push_back( Record );
			}
		}
		//else
		//{
		//	IntrRay3fSphere3f Intersector( ray, GetWorldBoundingSphere() );
		//	if ( Intersector.Find() )
		//	{
		//		PickRecord Record;

		//		Record.pEntity = this;
		//		Record.fDistance = min( Intersector.m_afRayT[0], Intersector.m_afRayT[1] ); 
		//		
		//		record.push_back( Record );
		//	}
		//}
	}
}
//--------------------------------------------------------------------------------
//void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds )
//{
//	//Entity3D::GetIntersectingEntities( set, bounds );
//
//	for ( auto pChild : m_Leafs ) {
//		if ( pChild ) pChild->GetIntersectingEntities( set, bounds );
//	}
//
//	for ( auto node : m_Nodes ) {
//		if ( node ) node->GetIntersectingEntities( set, bounds );
//	}
//}
////--------------------------------------------------------------------------------
//void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds )
//{
//	for ( auto pChild : m_Leafs ) {
//		if ( pChild ) pChild->GetIntersectingEntities( set, bounds );
//	}
//
//	for ( auto node : m_Nodes ) {
//		if ( node ) node->GetIntersectingEntities( set, bounds );
//	}
//}
//--------------------------------------------------------------------------------
//void Node3D::BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record )
//{
//	for ( auto pChild : m_Leafs )
//	{
//		if ( pChild )
//			pChild->BuildPickRecord( ray, record );
//	}
//}
