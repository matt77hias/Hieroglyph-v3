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
// Entity3D
//
//--------------------------------------------------------------------------------
#ifndef Entity3D_h
#define Entity3D_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Sphere3f.h"
#include "Matrix3f.h"
#include "Matrix4f.h"
#include "Ray3f.h"
#include "Frustum3f.h"
#include "RendererDX11.h"
#include "PickRecord.h"
#include "MaterialDX11.h"
#include "IController.h"
#include "CompositeShape.h"
#include "EntityRenderParams.h"
#include "ParameterContainer.h"

#include <string>
#include <vector>
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class Entity3D
	{
	public:
		Entity3D( );
		virtual ~Entity3D( );

		Vector3f& Position( );
		Matrix3f& Rotation( );
		Vector3f& Scale( );

		virtual void Update( float time );
		virtual void UpdateLocal( float time );
		virtual void UpdateWorld( );

		void SetRenderParams( IParameterManager* pParamManager );
		virtual void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		virtual void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );

		Matrix4f& LocalMatrix( );
		Matrix4f& WorldMatrix( );
		Matrix4f GetView( ) const;

		Vector4f LocalToWorldSpace( const Vector4f& input );
		Vector4f WorldToLocalSpace( const Vector4f& input );
		Vector3f LocalVectorToWorldSpace( const Vector3f& input );
		Vector3f LocalPointToWorldSpace( const Vector3f& input );
		Vector3f WorldVectorToLocalSpace( const Vector3f& input );
		Vector3f WorldPointToLocalSpace( const Vector3f& input );


		void SetMaterial( MaterialPtr pMaterial );
		MaterialPtr GetMaterial( );

		void SetGeometry( ExecutorPtr pExecutor );
		ExecutorPtr GetGeometry( );


		// Various entity properties are accessed here.

		bool IsHidden( ) const;
		void SetHidden( bool bHide );
		bool IsPickable() const;
		void SetPickable( bool bPickable );

		void SetLocalMatrixCalculation( bool bCalc );


		// Controller support is added here.  There can be multiple controllers which
		// will be updated in the order that they are added to the entity.

		void AttachController( IController* pController );
		IController* GetController( unsigned int index );

		virtual std::wstring toString( );

		// Scene graph support is added by the following functions.  The graph is enforced
		// by allowing only a single parent, and only Entity3D will be leaf nodes.

		Entity3D* GetParent( );
		Entity3D* GetRoot( );

		void AttachParent( Entity3D* Parent );
		void DetachParent( );

		// Some simple graph structure information is available through these recursive
		// function calls.

		bool IsRelated( Entity3D* Entity );
		int GraphDepth( );

		// The pick record is the correct way to build a list of the entities that are 
		// intersecting the ray.  The other two methods are just as valid, but perform
		// a different type of query than the pick record.

		virtual void BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record );
		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds );
		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds );
		virtual void GetEntities( std::vector< Entity3D* >& set );
		

		void SetName( const std::wstring& name );
		std::wstring GetName() const;

		// Provide the ability for a user to add custom data to this entity.  There is no
		// usage of this pointer from within the entity - it is purely a convenience for
		// users.  This also means that the user has to ensure that the object pointed to
		// by pData must be properly managed (i.e. deleted or protected appropriately).

		void SetUserData( void* pData );
		void* GetUserData() const;

	protected:

		Vector3f m_vTranslation;	// The translation and rotation varaibles are updated
		Matrix3f m_mRotation;		// during the update phase and used to generate the
		Vector3f m_vScale;			// local matrix.  Then, the world matrix is updated
		Matrix4f m_mWorld;			// with the new local matrix and the entity's parent
		Matrix4f m_mLocal;			// world matrix.

		// Entity flags
		bool m_bPickable;
		bool m_bHidden;
		bool m_bCalcLocal;
	
		Entity3D* m_pParent;
		//int m_iEntityID;
		std::vector< IController* > m_Controllers;

		std::wstring m_Name;

	public:
		EntityRenderParams m_sParams;
		ParameterContainer Parameters;

		Sphere3f m_ModelBoundingSphere;
		Sphere3f m_WorldBoundingSphere;

		// The composite shape consists of multiple shapes to represent the entity.
		// It is currently used for ray picking, but will eventually be added to visibility
		// tests, and possibly used to drive the physics shape generation.

		CompositeShape CompositeShape;
		
		// Storage for the custom user data
		void* m_pUserData;

		friend IController;
	};
};
//--------------------------------------------------------------------------------
#endif // Entity3D_h
