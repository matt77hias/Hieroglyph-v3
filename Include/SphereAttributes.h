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
// SphereAttributes
//
//--------------------------------------------------------------------------------
#ifndef SphereAttributes_h
#define SphereAttributes_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "AttributeEvaluator2f.h"
#include "Sphere3f.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// Produces the surface position from a sphere at the given coordinates.  

	template <typename TVertex>
	class SurfacePositionFromSphere // : public AttributeEvaluator2f<Sphere3f,TVertex>
	{
	public:
		void evaluate( Sphere3f& sphere, TVertex& v, float theta, float phi ) const
		{
			sphere.SamplePosition( v.position, theta, phi );
		};
	};

	// Produces the surface normal vector from a sphere at the given coordinates.
	
	template <class TVertex>
	class SurfaceNormalFromSphere //: public AttributeEvaluator2f<Sphere3f,TVertex>
	{
	public:
		virtual void evaluate( Sphere3f& sphere, TVertex& v, float theta, float phi ) const
		{
			sphere.SampleNormal( v.normal, theta, phi );
		};
	};

	// Produces a set of texture coordinates that map the complete texture address
	// to the range of [0,2*pi] for theta and [0,pi] for phi.

	template <class TVertex>
	class TexcoordsFromSphere //: public AttributeEvaluator2f<Sphere3f,TVertex>
	{
	public:
		virtual void evaluate( Sphere3f& sphere, TVertex& v, float theta, float phi ) const
		{
			v.texcoords.x = theta / static_cast<float>(( GLYPH_PI * 2.0 ));
			v.texcoords.y = phi / static_cast<float>(( GLYPH_PI ));
		};
	};

	// Produces a constant color that is specified when the object is created.

	template <class TVertex>
	class ConstantColorFromSphere //: public AttributeEvaluator2f<Sphere3f,TVertex>
	{
	public:
		ConstantColorFromSphere( const Vector4f& color ) : Color( color ) {};

		virtual void evaluate( Sphere3f& sphere, TVertex& v, float theta, float phi ) const
		{
			v.color = Color;
		};

		Vector4f Color;
	};

};
//--------------------------------------------------------------------------------
#endif // SphereAttributes_h
//--------------------------------------------------------------------------------