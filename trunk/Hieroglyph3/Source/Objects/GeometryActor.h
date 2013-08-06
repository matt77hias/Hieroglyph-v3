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
// GeometryActor
//
//--------------------------------------------------------------------------------
#ifndef GeometryActor_h
#define GeometryActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "BasicVertexDX11.h"
#include "DrawIndexedExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	typedef std::shared_ptr<DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>> IndexedImmediateGeometryPtr;

	class GeometryActor : public Actor
	{
	public:
		GeometryActor();
		virtual ~GeometryActor();

		void ResetGeometry();

		// Low level vertex manipulation can be performed with these methods.

		void AddVertex( const Vector3f& position );
		void AddVertex( const Vector3f& position, const Vector4f& color );
		void AddVertex( const Vector3f& position, const Vector2f& texcoords );
		void AddVertex( const Vector3f& position, const Vector4f& color, const Vector2f& texcoords );

		void AddVertex( const Vector3f& position, const Vector3f& normal );
		void AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color );
		void AddVertex( const Vector3f& position, const Vector3f& normal, const Vector2f& texcoords );
		void AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color, const Vector2f& texcoords );

		void AddIndex( const unsigned int index );
		void AddIndices( const unsigned int i1, const unsigned int i2 );
		void AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 );

		// Higher level geometric object manipulation can be performed with these methods.

		void DrawSphere( const Vector3f& center, float radius, unsigned int stacks = 6, unsigned int slices = 12 );
		void DrawDisc( const Vector3f& center, const Vector3f& normal, float radius, unsigned int slices = 12 );
		void DrawCylinder( const Vector3f& p1, const Vector3f& p2, float r1, float r2, unsigned int stacks = 2, unsigned int slices = 10 );

		void DrawBox( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector3f& zdir, const Vector3f& extents );
		void DrawBox( const Vector3f& center, const Vector3f& extents );
		void DrawAxisAlignedBox( const Vector3f& pt1, const Vector3f& pt2 );

		void DrawRect( const Vector3f& center, const Vector3f& xdir, const Vector3f& ydir, const Vector2f& extents );

		void UseSolidMaterial();
		void UseTexturedMaterial( ResourcePtr texture = nullptr );

		void SetSpecular( const Vector4f& color );
		Vector4f GetSpecular( ) const;
		
		void SetColor( const Vector4f& color );
		Vector4f GetColor( );

	protected:
		IndexedImmediateGeometryPtr				m_pGeometry;
		MaterialPtr								m_pSolidMaterial;
		MaterialPtr								m_pTexturedMaterial;

		Vector4f								m_Color;
	};
};
//--------------------------------------------------------------------------------
#endif // GeometryActor_h
