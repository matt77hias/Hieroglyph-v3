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
#include "PCH.h"
#include "Sphere3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Sphere3f::Sphere3f( ) :
	center(0.0f,0.0f,0.0f),
	radius(0.0f)
{
}
//--------------------------------------------------------------------------------
Sphere3f::Sphere3f( const Vector3f& Center, float Radius ) :
	center( Center ),
	radius( Radius )
{
}
//--------------------------------------------------------------------------------
Sphere3f::~Sphere3f()
{
}
//--------------------------------------------------------------------------------
bool Sphere3f::Intersects( const Sphere3f& Test ) const
{
	Vector3f Dist;
	Dist = ( center - Test.center ); 
	
	return( Dist.Magnitude( ) < ( radius + Test.radius ) );
}
//--------------------------------------------------------------------------------
bool Sphere3f::Envelops( const Sphere3f& test ) const
{
	Vector3f Dist;
	Dist = ( center - test.center ); 
	
	return( radius > test.radius + Dist.Magnitude( ) );
}
//--------------------------------------------------------------------------------
void Sphere3f::SamplePosition( Vector3f& position, float theta, float phi ) const
{
	position.x = radius * sinf( phi ) * cosf( theta ) + center.x;
	position.y = radius * cosf( phi ) + center.y;
	position.z = radius * sinf( phi ) * sinf( theta ) + center.z;
}
//--------------------------------------------------------------------------------
void Sphere3f::SampleNormal( Vector3f& normal, float theta, float phi ) const
{
	normal.x = radius * sinf( phi ) * cosf( theta );
	normal.y = radius * cosf( phi );
	normal.z = radius * sinf( phi ) * sinf( theta );

	normal = Vector3f::Normalize( normal );
}
//--------------------------------------------------------------------------------
void Sphere3f::SamplePositionAndNormal( Vector3f& position, Vector3f& normal, float theta, float phi ) const
{
	normal.x = radius * sinf( phi ) * cosf( theta );
	normal.y = radius * cosf( phi );
	normal.z = radius * sinf( phi ) * sinf( theta );

	position = normal + center;
	normal = Vector3f::Normalize( normal );
}
//--------------------------------------------------------------------------------