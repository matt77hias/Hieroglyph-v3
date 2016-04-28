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
// Node3D
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef Node3D_h
#define Node3D_h
//--------------------------------------------------------------------------------
#include "Transform3D.h"
#include "Ray3f.h"
#include "Frustum3f.h"
#include "PickRecord.h"
#include "IController.h"
#include "ParameterContainer.h"
#include "SceneRenderTask.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class Node3D
	{
	public:
		Node3D();
		~Node3D();


		void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );
		
		void Update( float time );
		void UpdateLocal( float time );
		void UpdateWorld( );

		void AttachChild( Entity3D* Child );
		void AttachChild( Node3D* Child );
		void DetachChild( Entity3D* Child );
		void DetachChild( Node3D* Child );

		void AttachParent( Node3D* Parent );
		void DetachParent( );
		Node3D* GetParent( );

		//Entity3D* GetChild( unsigned int index );

		std::wstring toString( );
		void SetName( const std::wstring& name );
		std::wstring GetName() const;

		const std::vector<Entity3D*>& Leafs();
		const std::vector<Node3D*>& Nodes();

		Transform3D Transform;
		ControllerPack<Node3D> Controllers;
	
	protected:
		std::wstring m_Name;

		std::vector< Entity3D* > m_Leafs;
		std::vector< Node3D* > m_Nodes;

		Node3D* m_pParent;
	};
};
//--------------------------------------------------------------------------------
#endif // Node3D_h
//--------------------------------------------------------------------------------