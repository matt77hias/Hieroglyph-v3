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
// GeometryActor
//
//--------------------------------------------------------------------------------
#ifndef GeometryActor_h
#define GeometryActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "IndexedImmediateGeometryDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GeometryActor : public Actor
	{
	public:
		GeometryActor();
		virtual ~GeometryActor();

		void ResetGeometry();

		void SetColor( Vector4f& color );
		
		void DrawSphere( Vector3f& center, float radius, unsigned int stacks = 6, unsigned int slices = 12 );

	protected:
		IndexedImmediateGeometryPtr				m_pGeometry;
	};
};
//--------------------------------------------------------------------------------
#endif // GeometryActor_h
