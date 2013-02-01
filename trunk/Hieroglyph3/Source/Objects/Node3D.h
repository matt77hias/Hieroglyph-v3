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
#include "Entity3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Node3D : public Entity3D
	{
	public:
		Node3D();
		virtual ~Node3D();

		virtual void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		virtual void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );
		
		virtual void Update( float time );

		void AttachChild( Entity3D* Child );
		void DetachChild( Entity3D* Child );
		Entity3D* GetChild( unsigned int index );

		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Sphere3f& bounds );
		virtual void GetIntersectingEntities( std::vector< Entity3D* >& set, Frustum3f& bounds );
		
		virtual void GetEntities( std::vector< Entity3D* >& set );

		virtual void BuildPickRecord( Ray3f& ray, std::vector<PickRecord>& record );

		virtual std::string toString( );

	protected:
		std::vector< Entity3D* > m_vChildren;

	};
};
//--------------------------------------------------------------------------------
#endif // Node3D_h
