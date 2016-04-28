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
#include "Transform3D.h"
#include "Ray3f.h"
#include "Frustum3f.h"
#include "RendererDX11.h"
#include "PickRecord.h"
#include "MaterialDX11.h"
#include "IController.h"
#include "CompositeShape.h"
#include "Renderable.h"
#include "ParameterContainer.h"

#include <string>
#include <vector>
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class Node3D;

	class Entity3D
	{
	public:
		Entity3D( );
		~Entity3D( );

		// Spatial related data and functionality

		void Update( float time );
		void UpdateLocal( float time );
		void UpdateWorld( );


		// Rendering related data and functionality
		void SetRenderParams( IParameterManager* pParamManager );
		void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );

		std::wstring toString( );

		// Scene graph support is added by the following functions.  The graph is enforced
		// by allowing only a single parent, and only Entity3D will be leaf nodes.

		void AttachParent( Node3D* Parent );
		void DetachParent( );
		Node3D* GetParent( );

		

		void SetName( const std::wstring& name );
		std::wstring GetName() const;

		// Provide the ability for a user to add custom data to this entity.  There is no
		// usage of this pointer from within the entity - it is purely a convenience for
		// users.  This also means that the user has to ensure that the object pointed to
		// by pData must be properly managed (i.e. deleted or protected appropriately).

		void SetUserData( void* pData );
		void* GetUserData() const;

	protected:

		Node3D* m_pParent;
		std::wstring m_Name;

	public:
		Transform3D					Transform;
		ControllerPack<Entity3D>	Controllers;
		Renderable					Visual;
		ParameterContainer			Parameters;

		//Sphere3f m_ModelBoundingSphere;

		// The composite shape consists of multiple shapes to represent the entity.
		// It is currently used for ray picking, but will eventually be added to visibility
		// tests, and possibly used to drive the physics shape generation.

		CompositeShape				Shape;

		// Storage for the custom user data
		void* m_pUserData;

		// Entity flags
		//bool m_bPickable;
	};
};
//--------------------------------------------------------------------------------
#endif // Entity3D_h
//--------------------------------------------------------------------------------
