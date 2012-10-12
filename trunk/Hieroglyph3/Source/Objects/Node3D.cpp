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
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL )
            m_vChildren[i]->PreRender( pRenderer, view );
	}
}
//--------------------------------------------------------------------------------
void Node3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL )
			m_vChildren[i]->Render( pPipelineManager, pParamManager, view );
	}
}
//--------------------------------------------------------------------------------
void Node3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL )
            m_vChildren[i]->Update( time );
	}
}
//--------------------------------------------------------------------------------
void Node3D::AttachChild( Entity3D* Child )
{
	// Check for open spots in the vector
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] == NULL )
		{
			m_vChildren[i] = Child;
			Child->AttachParent( this );
			return;
		}
	}

	// If no open spots then add a new one
	m_vChildren.add( Child );
	Child->AttachParent( this );
}
//--------------------------------------------------------------------------------
void Node3D::DetachChild( Entity3D* Child )
{
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] == Child )
		{
			m_vChildren[i]->DetachParent();
			m_vChildren[i] = 0;
		}
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds )
{
	Entity3D::GetIntersectingEntities( set, bounds );

	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL)
			m_vChildren[i]->GetIntersectingEntities( set, bounds );
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds )
{
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL)
			m_vChildren[i]->GetIntersectingEntities( set, bounds );
	}
}
//--------------------------------------------------------------------------------
std::string Node3D::toString( )
{
	std::stringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
		objString << "  ";

	objString << "Node3D : ID : " << m_iEntityID << "\n";

	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL)
			objString << m_vChildren[i]->toString();
	}

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void Node3D::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL )
			m_vChildren[i]->BuildPickRecord( ray, record );
	}
}
//--------------------------------------------------------------------------------
void Node3D::GetEntities( TArray< Entity3D* >& set )
{
	for ( int i = 0; i < m_vChildren.count(); i++ )
	{
		if ( m_vChildren[i] != NULL )
			m_vChildren[i]->GetEntities( set );
	}
}
//--------------------------------------------------------------------------------
Entity3D* Node3D::GetChild( int index )
{
	if ( ( index < 0 ) || ( index >= m_vChildren.count() ) )
		return( 0 );
	else
		return( m_vChildren[index] );
}
//--------------------------------------------------------------------------------