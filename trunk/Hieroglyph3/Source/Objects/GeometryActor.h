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

		void SetColor( const Vector4f& color );
		
		void DrawSphere( const Vector3f& center, float radius, unsigned int stacks = 6, unsigned int slices = 12 );
		void DrawDisc( const Vector3f& center, const Vector3f& normal, float radius, unsigned int slices = 12 );
		void DrawCylinder( const Vector3f& p1, const Vector3f& p2, float r1, float r2, unsigned int stacks = 2, unsigned int slices = 10 );

		void DrawBox( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector3f& zdir, const Vector3f& extents );
		void DrawBox( const Vector3f& center, const Vector3f& extents );
		void DrawRect( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector2f& extents );

		void UseSolidMaterial();
		void UseTexturedMaterial( ResourcePtr texture = nullptr );

	protected:
		IndexedImmediateGeometryPtr				m_pGeometry;
		MaterialPtr								m_pSolidMaterial;
		MaterialPtr								m_pTexturedMaterial;
	};
};
//--------------------------------------------------------------------------------
#endif // GeometryActor_h
