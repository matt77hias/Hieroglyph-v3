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
#include "Entity3D.h"
#include "Log.h"
#include "GeometryDX11.h"
#include "IntrRay3fSphere3f.h"
#include <sstream>
#include <algorithm>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
int Entity3D::m_iEntityCount = 0;
Entity3D* Entity3D::m_pRoot = NULL;
//--------------------------------------------------------------------------------
Entity3D::Entity3D()
{
	// Remember who is the first created - this should always be the root node!!!
	if ( Entity3D::m_pRoot == NULL )
		Entity3D::m_pRoot = this;

	// Record the entity ID
	m_iEntityID = Entity3D::m_iEntityCount++;

	m_pParent = 0;

	m_vTranslation.MakeZero();
	m_mRotation.MakeIdentity();

	m_mWorld.MakeIdentity();
	m_mLocal.MakeIdentity();

	m_ModelBoundingSphere.Center.MakeZero();
	m_ModelBoundingSphere.Radius = 1.0f;
	m_WorldBoundingSphere.Radius = 1.0f;

	m_pComposite = new CompositeShape();

	UpdateLocal( 0.0f );
	UpdateWorld( );
 
	m_bHidden = false;
}
//--------------------------------------------------------------------------------
Entity3D::~Entity3D()
{
	for ( int i = 0; i < m_Controllers.count(); i++ )
		delete m_Controllers[i];

	if ( m_pComposite )
		delete m_pComposite;
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
bool Entity3D::IsHidden()
{
	return( m_bHidden );
}
//--------------------------------------------------------------------------------
int Entity3D::GetEntityID()
{
	return( m_iEntityID );
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
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateLocal( float fTime )
{
	for ( int i = 0; i < m_Controllers.count(); i++ )
		m_Controllers[i]->Update( fTime );

//	m_vTranslation = m_vTranslation + (m_PosChange * fTime);
	
//	CMatrix3f Rot;
//	Rot.RotationX(m_RotChange.x());
//	m_mRotation = Rot * m_mRotation;
//	Rot.RotationY(m_RotChange.y());
//	m_mRotation = m_mRotation * Rot;

//	m_PosChange.MakeZero(); 
//	m_RotChange.MakeZero();

	m_mLocal.MakeIdentity( );
	m_mLocal.SetRotation( m_mRotation );
	m_mLocal.SetTranslation( m_vTranslation );
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateWorld( )
{
	if (m_pParent)
        m_mWorld = m_mLocal * m_pParent->WorldMatrix();
	else
		m_mWorld = m_mLocal;

	// Update bounding sphere with the new world space position and orientation.
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
Matrix4f Entity3D::GetView()
{
	Matrix4f Ret;

	Vector3f Eye;
	Vector3f At;
	Vector3f Up;

	Eye = m_mWorld.GetTranslation();
	At = m_mWorld.GetTranslation() + m_mWorld.GetBasisZ();
	Up = m_mWorld.GetBasisY();

	D3DXMatrixLookAtLH((D3DXMATRIX*)&Ret, 
		(D3DXVECTOR3*)&Eye,
		(D3DXVECTOR3*)&At,
		(D3DXVECTOR3*)&Up);

	return( Ret );
}
//--------------------------------------------------------------------------------
void Entity3D::PreRender( RendererDX11& Renderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	//if ( m_pMaterial )
	//	m_pMaterial->PreRender( Renderer, view );
}
//--------------------------------------------------------------------------------
void Entity3D::Render( RendererDX11& Renderer, VIEWTYPE view )
{
	// Test if the entity contains any geometry, and it has a material
	if ( ( m_sParams.pGeometry ) && ( m_sParams.pMaterial ) )
	{
		// Only render if the material indicates that you should
		//if ( m_pMaterial->Params[view].bRender )
		//{
		//	// Set the world matrix
		//	Renderer.SetWorldMatrix( m_sParams.mWorldMatrix );

		//	// Set the material parameters
		//	m_pMaterial->SetRenderParams( Renderer, view );

		//	// Send the geometry to the renderer using the appropriate
		//	// material view effect.
		//	Renderer.Draw( m_pMaterial->Params[view].iEffect, *m_sParams.pGC );
		//}
	}
}
//--------------------------------------------------------------------------------
void Entity3D::Hide( bool bHide )
{
	m_bHidden = bHide;
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
std::string Entity3D::toString( )
{
	std::stringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
	{
		if ( i < GraphDepth() - 1 )
			objString << "| ";
		else
			objString << "+-";
	}

	objString << "Entity3D : ID : " << m_iEntityID << "\n";

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void Entity3D::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	// If the entity has a composite shape then use it, otherwise simply use
	// the world bounding sphere to check for a hit.

	if ( m_pComposite && ( m_pComposite->GetNumberOfShapes() > 0 ) )
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
		if ( m_pComposite->RayIntersection( ObjectRay, &fT ) )
		{
			PickRecord Record;
			Record.pEntity = this;
			Record.fDistance = fT;
			record.add( Record );
		}
	}
	else
	{
		IntrRay3fSphere3f Intersector( ray, m_WorldBoundingSphere );
		if ( Intersector.Find() )
		{
			PickRecord Record;
			Record.pEntity = this;
			//Record.fDistance = std::min( Intersector.m_afRayT[0], Intersector.m_afRayT[1] ); 
			Record.fDistance = min( Intersector.m_afRayT[0], Intersector.m_afRayT[1] ); 
			record.add( Record );
		}
	}
}
//--------------------------------------------------------------------------------
void Entity3D::AttachController( IController* pController )
{
	if ( pController )
	{
		m_Controllers.add( pController );
		pController->SetEntity( this );
	}
}
//--------------------------------------------------------------------------------
void Entity3D::UpdateControllers( float fTime )
{
	for ( int i = 0; i < m_Controllers.count(); i++ )
	{
		m_Controllers[i]->Update( fTime );
	}
}
//--------------------------------------------------------------------------------
IController* Entity3D::GetController( int index )
{
	if ( m_Controllers.count() <= index )
		return( 0 );

	return( m_Controllers[index] );
}
//--------------------------------------------------------------------------------
void Entity3D::SetCompositeShape( CompositeShape* pShape )
{
	// Delete the existing shape before setting the new one.

	if ( m_pComposite )
		delete m_pComposite;

	m_pComposite = pShape;
}
//--------------------------------------------------------------------------------
CompositeShape* Entity3D::GetCompositeShape( )
{
	return( m_pComposite );
}
//--------------------------------------------------------------------------------
void Entity3D::SetMaterial( MaterialDX11* pMaterial, bool bSingleEntity )
{
	if ( pMaterial )
	{
		//m_pMaterial = pMaterial;
		//m_pMaterial->SetEntity( this );
	}
}
//--------------------------------------------------------------------------------
MaterialDX11* Entity3D::GetMaterial( )
{
	return( m_sParams.pMaterial );
}
//--------------------------------------------------------------------------------
void Entity3D::GetEntities( TArray< Entity3D* >& set )
{
	set.add( this );
}
//--------------------------------------------------------------------------------
void Entity3D::SetGeometry( GeometryDX11* pGeometry )
{
	if ( m_sParams.pGeometry )
		m_sParams.pGeometry->Release();

	pGeometry->AddReference();

	m_sParams.pGeometry = pGeometry;
}
//--------------------------------------------------------------------------------