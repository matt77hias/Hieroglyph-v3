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
#include "Entity3D.h"
#include "Log.h"
#include "GeometryDX11.h"
#include "IntrRay3fSphere3f.h"
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
#include "Node3D.h"
#include "SceneGraph.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Entity3D::Entity3D() :
	m_pParent( nullptr ),
	m_Name( L"" ),
	//m_bHidden( false ),
	//m_bPickable( true ),
	m_pUserData( nullptr ),
	Shape(),
	Controllers( this ),
	Visual()
{
	//Sphere3f* pBoundingSphere = new Sphere3f();
	//pBoundingSphere->Center.MakeZero();
	//pBoundingSphere->Radius = 1.0f;
	//Shape.AddShape( pBoundingSphere );
}
//--------------------------------------------------------------------------------
Entity3D::~Entity3D()
{
}
//--------------------------------------------------------------------------------
//void Entity3D::SetHidden( bool bHide )
//{
//	m_bHidden = bHide;
//}
//--------------------------------------------------------------------------------
//bool Entity3D::IsHidden() const
//{
//	return( m_bHidden );
//}
//--------------------------------------------------------------------------------
//void Entity3D::SetPickable( bool bPickable )
//{
//	m_bPickable = bPickable;
//}
//--------------------------------------------------------------------------------
//bool Entity3D::IsPickable() const
//{
//	return( m_bPickable );
//}
//--------------------------------------------------------------------------------
Node3D* Entity3D::GetParent()
{
	return( m_pParent );
}
//--------------------------------------------------------------------------------
//Node3D* Entity3D::GetRoot( )
//{
//	if ( m_pParent )
//		return( m_pParent->GetRoot() );
//	else
//		return( nullptr ); // Not attached to a node!
//}
//--------------------------------------------------------------------------------
void Entity3D::AttachParent( Node3D* Parent )
{
	m_pParent = Parent;
}
//--------------------------------------------------------------------------------
void Entity3D::DetachParent( )
{
	m_pParent = nullptr;
}
//--------------------------------------------------------------------------------
void Entity3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	// Give the material a chance to update itself
	if ( Visual.Material != nullptr ) {
		Visual.Material->Update( time );
	}
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateLocal( float fTime )
{
	// Update the controllers that are attached to this entity.

	Controllers.Update( fTime );

	// Load the local space matrix with the rotation and translation components.

	Transform.UpdateLocal();
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateWorld( )
{
	// If the entity has a parent, then update its world matrix accordingly.

	if (m_pParent)
		Transform.UpdateWorld( m_pParent->Transform.WorldMatrix() );
	else
		Transform.UpdateWorld( );
}
//--------------------------------------------------------------------------------
//CompositeShape Entity3D::GetWorldBounds() const
//{
//	// Transform the bounding sphere and return it on the fly.  This is done to 
//	// eliminate a member variable (for the world bounding sphere), and to remove
//	// the calculation from the update phase if the world sphere isn't used.
//
//	Vector4f worldposition = Transform.WorldMatrix() * Vector4f( m_ModelBoundingSphere.Center, 1.0f );
//	return( Sphere3f( worldposition.xyz(), m_ModelBoundingSphere.Radius ) );
//}
//--------------------------------------------------------------------------------
void Entity3D::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	if ( Visual.Material != NULL )
		Visual.Material->PreRender( pRenderer, view );
}
//--------------------------------------------------------------------------------
void Entity3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	// Test if the entity contains any geometry, and it has a material
	if ( ( Visual.Executor != NULL ) && ( Visual.Material != NULL ) )
	{
		// Only render if the material indicates that you should
		if ( Visual.Material->Params[view].bRender )
		{
			// Set the material render parameters.  This is done before the entity
			// render parameters so that unique values can be set by the individual
			// entities, and still allow the material to set parameters for any
			// entities that don't specialize the parameters.
			Visual.Material->SetRenderParams( pParamManager, view );

			// Set the entity render parameters
			this->SetRenderParams( pParamManager );

			// Configure the pipeline with the render effect supplied by the material.

			// This clear pipeline resources call has been removed, and is 
			// expected to be implemented in the Task that controls the pipeline
			// output configuration (i.e. ViewPerspective and his friends).
			// pPipelineManager->ClearPipelineResources();

			Visual.Material->Params[view].pEffect->ConfigurePipeline( pPipelineManager, pParamManager );
			pPipelineManager->ApplyPipelineResources();

			// Let the geometry execute its drawing operation.  This includes 
			// configuring the input to the pipeline, plus calling an appropriate
			// draw call.
			Visual.Executor->Execute( pPipelineManager, pParamManager );
		}
	}
}
//--------------------------------------------------------------------------------
void Entity3D::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the world matrix
	pParamManager->SetWorldMatrixParameter( &Transform.WorldMatrix() );
	
	// Scroll through each parameter and set it in the renderer
	Parameters.SetRenderParams( pParamManager );
}
//--------------------------------------------------------------------------------
//int Entity3D::GraphDepth( )
//{
//	if ( m_pParent == NULL )
//		return( 0 );
//	else
//		return( m_pParent->GraphDepth() + 1 );
//}
//--------------------------------------------------------------------------------
std::wstring Entity3D::toString( )
{
	std::wstringstream objString;

	for ( int i = 0; i < GraphDepth( this ); i++ )
	{
		if ( i < GraphDepth( this ) - 1 )
			objString << L"| ";
		else
			objString << L"+-";
	}

	objString << L"Entity3D ID:" << this << L", Name:" << GetName() << L"\n";

	return( objString.str() );
}
//--------------------------------------------------------------------------------

void Entity3D::SetName( const std::wstring& name )
{
	m_Name = name;
}
//--------------------------------------------------------------------------------
std::wstring Entity3D::GetName() const
{
	return( m_Name );
}
//--------------------------------------------------------------------------------
void Entity3D::SetUserData( void* pData )
{
	m_pUserData = pData;
}
//--------------------------------------------------------------------------------
void* Entity3D::GetUserData() const
{
	return( m_pUserData );
}
//--------------------------------------------------------------------------------