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
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Entity3D::Entity3D() :
	m_pParent( nullptr ),
	m_Name( L"" ),
	m_bHidden( false ),
	m_bPickable( true ),
	m_bCalcLocal( true ),
	CompositeShape()
{
	m_vTranslation.MakeZero();
	m_mRotation.MakeIdentity();
	m_vScale = Vector3f( 1.0f, 1.0f, 1.0f );

	m_mWorld.MakeIdentity();
	m_mLocal.MakeIdentity();

	m_ModelBoundingSphere.Center.MakeZero();
	m_ModelBoundingSphere.Radius = 1.0f;
	m_WorldBoundingSphere.Radius = 1.0f;
}
//--------------------------------------------------------------------------------
Entity3D::~Entity3D()
{
	for ( auto pController : m_Controllers )
		delete pController;
}
//--------------------------------------------------------------------------------
Vector3f& Entity3D::Position()
{
	return( m_vTranslation );
}
//--------------------------------------------------------------------------------
Matrix3f& Entity3D::Rotation()
{
	return( m_mRotation );
}
//--------------------------------------------------------------------------------
Vector3f& Entity3D::Scale( )
{
	return( m_vScale );
}
//--------------------------------------------------------------------------------
void Entity3D::SetHidden( bool bHide )
{
	m_bHidden = bHide;
}
//--------------------------------------------------------------------------------
bool Entity3D::IsHidden() const
{
	return( m_bHidden );
}
//--------------------------------------------------------------------------------
void Entity3D::SetPickable( bool bPickable )
{
	m_bPickable = bPickable;
}
//--------------------------------------------------------------------------------
bool Entity3D::IsPickable() const
{
	return( m_bPickable );
}
//--------------------------------------------------------------------------------
Entity3D* Entity3D::GetParent()
{
	return( m_pParent );
}
//--------------------------------------------------------------------------------
Entity3D* Entity3D::GetRoot( )
{
	if ( m_pParent )
		return( m_pParent->GetRoot() );
	else
		return( this );
}
//--------------------------------------------------------------------------------
void Entity3D::AttachParent( Entity3D* Parent )
{
	m_pParent = Parent;
}
//--------------------------------------------------------------------------------
void Entity3D::DetachParent( )
{
	m_pParent = NULL;
}
//--------------------------------------------------------------------------------
bool Entity3D::IsRelated( Entity3D* Entity )
{
	if (m_pParent == Entity)
		return(true);
	else
	{
		if (m_pParent)
            if (m_pParent->IsRelated(Entity))
				return(true);
	}
	return(false);
}
//--------------------------------------------------------------------------------
void Entity3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	m_sParams.WorldMatrix = m_mWorld;

	// Give the material a chance to update itself
	if ( m_sParams.Material != nullptr )
	{
		m_sParams.Material->Update( time );
	}
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateLocal( float fTime )
{
	// Update the controllers that are attached to this entity.

	for ( auto pController : m_Controllers )
		pController->Update( fTime );

	// Load the local space matrix with the rotation and translation components.

	if ( m_bCalcLocal )
	{
		Matrix4f scale = Matrix4f::ScaleMatrix( m_vScale );
		
		m_mLocal.MakeIdentity();
		m_mLocal.SetRotation( m_mRotation );
		m_mLocal.SetTranslation( m_vTranslation );
		m_mLocal = scale * m_mLocal;
	}
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateWorld( )
{
	// If the entity has a parent, then update its world matrix accordingly.

	if (m_pParent)
        m_mWorld = m_mLocal * m_pParent->WorldMatrix();
	else
		m_mWorld = m_mLocal;

	// Update bounding sphere with the new world space position and orientation.
	// TODO: The shape classes should be transformed via matrix functions instead
	//       of manual manipulation like this!

	Vector3f center = m_ModelBoundingSphere.Center;
	Vector4f modelposition = Vector4f( center.x, center.y, center.z, 1.0f );
	Vector4f worldposition = m_mWorld * modelposition;
	m_WorldBoundingSphere.Center = Vector3f( worldposition.x, worldposition.y, worldposition.z );
}
//--------------------------------------------------------------------------------
Matrix4f& Entity3D::WorldMatrix()
{
	return( m_mWorld );
}
//--------------------------------------------------------------------------------
Matrix4f& Entity3D::LocalMatrix()
{
	return( m_mLocal );
}
//--------------------------------------------------------------------------------
Matrix4f Entity3D::GetView() const
{
	Vector3f Eye;
	Vector3f At;
	Vector3f Up;

	Eye = m_mWorld.GetTranslation();
	At = m_mWorld.GetTranslation() + m_mWorld.GetBasisZ();
	Up = m_mWorld.GetBasisY();

	return( Matrix4f::LookAtLHMatrix( Eye, At, Up ) );
}
//--------------------------------------------------------------------------------
Vector4f Entity3D::LocalToWorldSpace( const Vector4f& input )
{
	Vector4f result = WorldMatrix() * input;

    return( result );
}
//--------------------------------------------------------------------------------
Vector4f Entity3D::WorldToLocalSpace( const Vector4f& input )
{
	Vector4f result = WorldMatrix().Inverse() * input;

    return( result );
}
//--------------------------------------------------------------------------------
Vector3f Entity3D::LocalVectorToWorldSpace( const Vector3f& input )
{
	Vector4f result = LocalToWorldSpace( Vector4f( input, 0.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Entity3D::LocalPointToWorldSpace( const Vector3f& input )
{
	Vector4f result = LocalToWorldSpace( Vector4f( input, 1.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Entity3D::WorldVectorToLocalSpace( const Vector3f& input )
{
	Vector4f result = WorldToLocalSpace( Vector4f( input, 0.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Entity3D::WorldPointToLocalSpace( const Vector3f& input )
{
	Vector4f result = WorldToLocalSpace( Vector4f( input, 1.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
void Entity3D::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	if ( m_sParams.Material != NULL )
		m_sParams.Material->PreRender( pRenderer, view );
}
//--------------------------------------------------------------------------------
void Entity3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	// Test if the entity contains any geometry, and it has a material
	if ( ( m_sParams.Executor != NULL ) && ( m_sParams.Material != NULL ) )
	{
		// Only render if the material indicates that you should
		if ( m_sParams.Material->Params[view].bRender )
		{
			// Set the material render parameters.  This is done before the entity
			// render parameters so that unique values can be set by the individual
			// entities, and still allow the material to set parameters for any
			// entities that don't specialize the parameters.
			m_sParams.Material->SetRenderParams( pParamManager, view );

			// Set the entity render parameters
			this->SetRenderParams( pParamManager );

			// Configure the pipeline with the render effect supplied by the material.
			pPipelineManager->ClearPipelineResources();
			m_sParams.Material->Params[view].pEffect->ConfigurePipeline( pPipelineManager, pParamManager );
			pPipelineManager->ApplyPipelineResources();

			// Let the geometry execute its drawing operation.  This includes 
			// configuring the input to the pipeline, plus calling an appropriate
			// draw call.
			m_sParams.Executor->Execute( pPipelineManager, pParamManager );
		}
	}
}
//--------------------------------------------------------------------------------
void Entity3D::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the world matrix
	pParamManager->SetWorldMatrixParameter( &m_sParams.WorldMatrix );
	
	// Scroll through each parameter and set it in the renderer
	Parameters.SetRenderParams( pParamManager );
}
//--------------------------------------------------------------------------------
void Entity3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds )
{
	if ( bounds.Intersects( m_WorldBoundingSphere ) )
		set.push_back( this );
}
//--------------------------------------------------------------------------------
void Entity3D::GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds )
{
	if ( bounds.Intersects( m_WorldBoundingSphere ) )
		set.push_back( this );
}
//--------------------------------------------------------------------------------
int Entity3D::GraphDepth( )
{
	if ( m_pParent == NULL )
		return( 0 );
	else
		return( m_pParent->GraphDepth() + 1 );
}
//--------------------------------------------------------------------------------
std::wstring Entity3D::toString( )
{
	std::wstringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
	{
		if ( i < GraphDepth() - 1 )
			objString << L"| ";
		else
			objString << L"+-";
	}

	objString << L"Entity3D ID:" << this << L", Name:" << GetName() << L"\n";

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void Entity3D::BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record )
{
	// If the entity has a composite shape then use it, otherwise simply use
	// the world bounding sphere to check for a hit.

	if ( IsPickable() )
	{
		if ( CompositeShape.GetNumberOfShapes() > 0 )
		{
			Matrix4f InvWorld = m_mWorld.Inverse();
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
			if ( CompositeShape.RayIntersection( ObjectRay, &fT ) )
			{
				PickRecord Record;
				Record.pEntity = this;
				Record.fDistance = fT;
				record.push_back( Record );
			}
		}
		else
		{
			IntrRay3fSphere3f Intersector( ray, m_WorldBoundingSphere );
			if ( Intersector.Find() )
			{
				PickRecord Record;

				Record.pEntity = this;
				Record.fDistance = min( Intersector.m_afRayT[0], Intersector.m_afRayT[1] ); 
				
				record.push_back( Record );
			}
		}
	}
}
//--------------------------------------------------------------------------------
void Entity3D::AttachController( IController* pController )
{
	if ( pController )
	{
		m_Controllers.push_back( pController );
		pController->SetEntity( this );
	}
}
//--------------------------------------------------------------------------------
IController* Entity3D::GetController( unsigned int index )
{
	if ( m_Controllers.size() <= index )
		return( nullptr );

	return( m_Controllers[index] );
}
//--------------------------------------------------------------------------------
void Entity3D::SetMaterial( MaterialPtr pMaterial )
{
	m_sParams.Material = pMaterial;

	// Query all of the vertex shader IDs that will be used so that the pipeline
	// executor class can pre-create them.

	if ( m_sParams.Material != nullptr && m_sParams.Executor != nullptr )
	{
		std::vector<int> idlist;
		m_sParams.Material->GetAllVertexShaderIDs( idlist );

		for ( auto ID : idlist ) {
			m_sParams.Executor->GenerateInputLayout( ID );
		}
	}
}
//--------------------------------------------------------------------------------
MaterialPtr Entity3D::GetMaterial( )
{
	return( m_sParams.Material );
}
//--------------------------------------------------------------------------------
void Entity3D::GetEntities( std::vector< Entity3D* >& set )
{
	set.push_back( this );
}
//--------------------------------------------------------------------------------
void Entity3D::SetGeometry( ExecutorPtr pExecutor )
{
	m_sParams.Executor = pExecutor;

	// Query all of the vertex shader IDs that will be used so that the pipeline
	// executor class can pre-create them.

	if ( m_sParams.Material != nullptr && m_sParams.Executor != nullptr )
	{
		std::vector<int> idlist;
		m_sParams.Material->GetAllVertexShaderIDs( idlist );

		for ( auto ID : idlist ) {
			m_sParams.Executor->GenerateInputLayout( ID );
		}
	}
}
//--------------------------------------------------------------------------------
ExecutorPtr Entity3D::GetGeometry( )
{
	return( m_sParams.Executor );
}
//--------------------------------------------------------------------------------
void Entity3D::SetLocalMatrixCalculation( bool bCalc )
{
	m_bCalcLocal = bCalc;
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