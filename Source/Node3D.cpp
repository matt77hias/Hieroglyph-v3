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
#include "Node3D.h"
#include "Entity3D.h"
#include "SceneGraph.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Node3D::Node3D() :
	m_pParent( nullptr ),
	Controllers( this )
{
}
//--------------------------------------------------------------------------------
Node3D::~Node3D()
{
}
//--------------------------------------------------------------------------------
void Node3D::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	for ( auto pChild : m_Leafs ) {
		if ( pChild ) pChild->PreRender( pRenderer, view );
	}

	for ( auto node : m_Nodes ) {
		if ( node ) node->PreRender( pRenderer, view );
	}
}
//--------------------------------------------------------------------------------
void Node3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	for ( auto pChild : m_Leafs ) {
		if ( pChild ) pChild->Render( pPipelineManager, pParamManager, view );
	}

	for ( auto node : m_Nodes ) {
		if ( node ) node->Render( pPipelineManager, pParamManager, view );
	}

}
//--------------------------------------------------------------------------------
void Node3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	for ( auto pChild : m_Leafs ) {
		if ( pChild ) pChild->Update( time );
	}

	for ( auto node : m_Nodes ) {
		if ( node ) node->Update( time );
	}
}
//--------------------------------------------------------------------------------
void Node3D::UpdateLocal( float fTime )
{
	// Update the controllers that are attached to this entity.

	Controllers.Update( fTime );

	// Load the local space matrix with the rotation and translation components.

	Transform.UpdateLocal();
}
//--------------------------------------------------------------------------------
void Node3D::UpdateWorld( )
{
	// If the entity has a parent, then update its world matrix accordingly.

	if ( m_pParent )
		Transform.UpdateWorld( m_pParent->Transform.WorldMatrix() );
	else
		Transform.UpdateWorld( );
}
//--------------------------------------------------------------------------------
void Node3D::AttachChild( Entity3D* Child )
{
	// Check for open spots in the vector
	for ( auto& pChild : m_Leafs )
	{
		if ( pChild == nullptr )
		{
			pChild = Child;
			Child->AttachParent( this );
			return;
		}
	}

	// If no open spots then add a new one
	m_Leafs.push_back( Child );
	Child->AttachParent( this );
}
//--------------------------------------------------------------------------------
void Node3D::AttachChild( Node3D* Child )
{
	// Check for open spots in the vector
	for ( auto& pChild : m_Nodes )
	{
		if ( pChild == nullptr )
		{
			pChild = Child;
			Child->AttachParent( this );
			return;
		}
	}

	// If no open spots then add a new one
	m_Nodes.push_back( Child );
	Child->AttachParent( this );
}
//--------------------------------------------------------------------------------
void Node3D::DetachChild( Entity3D* Child )
{
	for ( auto& pChild : m_Leafs )
	{
		if ( pChild == Child )
		{
			pChild->DetachParent();
			pChild = nullptr;
		}
	}
}
//--------------------------------------------------------------------------------
void Node3D::DetachChild( Node3D* Child )
{
	for ( auto& pChild : m_Nodes )
	{
		if ( pChild == Child )
		{
			pChild->DetachParent();
			pChild = nullptr;
		}
	}
}
//--------------------------------------------------------------------------------
void Node3D::AttachParent( Node3D* Parent )
{
	m_pParent = Parent;
}
//--------------------------------------------------------------------------------
void Node3D::DetachParent( )
{
	m_pParent = nullptr;
}
//--------------------------------------------------------------------------------
Node3D* Node3D::GetParent()
{
	return( m_pParent );
}
//--------------------------------------------------------------------------------
std::wstring Node3D::toString( )
{
	std::wstringstream objString;

	for ( int i = 0; i < GraphDepth( this ); i++ )
		objString << L"  ";

	objString << L"Node3D ID: " << this << L", Name:" << GetName() << L"\n";

	for ( auto pChild : m_Leafs )
	{
		if ( pChild )
			objString << pChild->toString();
	}

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void Node3D::SetName( const std::wstring& name )
{
	m_Name = name;
}
//--------------------------------------------------------------------------------
std::wstring Node3D::GetName() const
{
	return( m_Name );
}
//--------------------------------------------------------------------------------
//Entity3D* Node3D::GetChild( unsigned int index )
//{
//	if ( index >= m_Leafs.size() )
//		return( nullptr );
//	else
//		return( m_Leafs[index] );
//}
//--------------------------------------------------------------------------------
const std::vector<Entity3D*>& Node3D::Leafs()
{
	return m_Leafs;
}
//--------------------------------------------------------------------------------
const std::vector<Node3D*>& Node3D::Nodes()
{
	return m_Nodes;
}
//--------------------------------------------------------------------------------
