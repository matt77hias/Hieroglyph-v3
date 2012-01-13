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
// Entity3D
//
//--------------------------------------------------------------------------------
#ifndef Entity3D_h
#define Entity3D_h
//--------------------------------------------------------------------------------
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
#include "TArray.h"
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
		float& Scale( );

		virtual void Update( float time );
		virtual void UpdateLocal( float time );
		virtual void UpdateWorld( );

		void SetRenderParams( IParameterManager* pParamManager );
		virtual void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		virtual void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );

		Matrix4f& LocalMatrix( );
		Matrix4f& WorldMatrix( );
		Matrix4f GetView( );

		void SetMaterial( MaterialPtr pMaterial );
		MaterialPtr GetMaterial( );

		void SetGeometry( ExecutorPtr executor );


		// Various entity properties are accessed here.

		int GetEntityID( );

		bool IsHidden( );
		void Hide( bool bHide );
		void SetLocalMatrixCalculation( bool bCalc );


		// Controller support is added here.  There can be multiple controllers which
		// will be updated in the order that they are added to the entity.

		void AttachController( IController* pController );
		void UpdateControllers( float fTime );
		IController* GetController( int index );

		virtual std::string toString( );


		// Static entity population variables

		static int m_iEntityCount;
		static Entity3D* m_pRoot;

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

		virtual void BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record );
		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds );
		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds );
		virtual void GetEntities( TArray< Entity3D* >& set );
		
		// The composite shape consists of multiple shapes to represent the entity.
		// It is currently used for ray picking, but will eventually be added to visibility
		// tests, and possibly used to drive the physics shape generation.

		void SetCompositeShape( CompositeShape* pShape );
		CompositeShape* GetCompositeShape( );


	protected:

		Vector3f m_vTranslation;	// The translation and rotation varaibles are updated
		Matrix3f m_mRotation;		// during the update phase and used to generate the
		float    m_fScale;			// local matrix.  Then, the world matrix is updated
		Matrix4f m_mWorld;			// with the new local matrix and the entity's parent
		Matrix4f m_mLocal;			// world matrix.

		// Entity flags
		bool m_bHidden;
		bool m_bCalcLocal;
	
		Entity3D* m_pParent;
		int m_iEntityID;
		TArray< IController* > m_Controllers;

	public:
		EntityRenderParams m_sParams;
		ParameterContainer Parameters;

		Sphere3f m_ModelBoundingSphere;
		Sphere3f m_WorldBoundingSphere;

		CompositeShape* m_pComposite;

		friend IController;
	};
};
//--------------------------------------------------------------------------------
#endif // Entity3D_h
