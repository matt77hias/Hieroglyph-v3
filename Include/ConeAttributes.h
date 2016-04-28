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
// ConeAttributes
//
//--------------------------------------------------------------------------------
#ifndef ConeAttributes_h
#define ConeAttributes_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "AttributeEvaluator2f.h"
#include "Cone3f.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// Produces the surface position from a cone at the given coordinates.  

	template <class TVertex>
	class SurfacePositionFromCone //: public AttributeEvaluator2f<Cone3f,TVertex>
	{
	public:
		virtual void evaluate( Cone3f& cone, TVertex& v, float theta, float height ) const
		{
			cone.SamplePosition( v.position, theta, height );
		};
	};


	// Produces the surface normal vector from a cone at the given coordinates.
	
	template <class TVertex>
	class SurfaceNormalFromCone //: public AttributeEvaluator2f<Cone3f,TVertex>
	{
	public:
		virtual void evaluate( Cone3f& cone, TVertex& v, float theta, float height ) const
		{
			cone.SampleNormal( v.normal, theta, height );
		};
	};


	// Produces a set of texture coordinates that map the complete texture address
	// to the theta and height arguments.

	template <class TVertex>
	class TexcoordsFromCone //: public AttributeEvaluator2f<Cone3f,TVertex>
	{
	public:
		virtual void evaluate( Cone3f& cone, TVertex& v, float theta, float height ) const
		{
			v.texcoords.x = height / 1.0f;
			v.texcoords.y = theta / 1.0f;
		};
	};


	// Produces a constant color that is specified when this object is created.

	template <class TVertex>
	class ConstantColorFromCone //: public AttributeEvaluator2f<Cone3f,TVertex>
	{
	public:
		ConstantColorFromCone( const Vector4f& color ) : Color( color ) {};

		virtual void evaluate( Cone3f& cone, TVertex& v, float theta, float height ) const
		{
			v.color = Color;
		};

		Vector4f Color;
	};

};
//--------------------------------------------------------------------------------
#endif // ConeAttributes_h
//--------------------------------------------------------------------------------