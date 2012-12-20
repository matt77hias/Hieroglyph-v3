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
#include "Node3D.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Node3D::Node3D()
{
}
//--------------------------------------------------------------------------------
Node3D::~Node3D()
{

}
//--------------------------------------------------------------------------------
void Node3D::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
            pChild->PreRender( pRenderer, view );
	}
}
//--------------------------------------------------------------------------------
void Node3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			pChild->Render( pPipelineManager, pParamManager, view );
	}
}
//--------------------------------------------------------------------------------
void Node3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
            pChild->Update( time );
	}
}
//--------------------------------------------------------------------------------
void Node3D::AttachChild( Entity3D* Child )
{
	// Check for open spots in the vector
	for ( auto pChild : m_vChildren )
	{
		if ( pChild == nullptr )
		{
			pChild = Child;
			Child->AttachParent( this );
			return;
		}
	}

	// If no open spots then add a new one
	m_vChildren.push_back( Child );
	Child->AttachParent( this );
}
//--------------------------------------------------------------------------------
void Node3D::DetachChild( Entity3D* Child )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild == Child )
		{
			pChild->DetachParent();
			pChild = nullptr;
		}
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds )
{
	Entity3D::GetIntersectingEntities( set, bounds );

	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			pChild->GetIntersectingEntities( set, bounds );
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			pChild->GetIntersectingEntities( set, bounds );
	}
}
//--------------------------------------------------------------------------------
std::string Node3D::toString( )
{
	std::stringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
		objString << "  ";

	objString << "Node3D : ID : " << m_iEntityID << "\n";

	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			objString << pChild->toString();
	}

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void Node3D::BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			pChild->BuildPickRecord( ray, record );
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetEntities( std::vector< Entity3D* >& set )
{
	for ( auto pChild : m_vChildren )
	{
		if ( pChild )
			pChild->GetEntities( set );
	}
}
//--------------------------------------------------------------------------------
Entity3D* Node3D::GetChild( unsigned int index )
{
	if ( index >= m_vChildren.size() )
		return( nullptr );
	else
		return( m_vChildren[index] );
}
//--------------------------------------------------------------------------------