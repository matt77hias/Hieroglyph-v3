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
Sphere3f::Sphere3f( )
{
	Center = Vector3f( 0.0f, 0.0f, 0.0f );
	Radius = 0.0f;
}
//--------------------------------------------------------------------------------
Sphere3f::Sphere3f( const Vector3f& center, float radius )
{
	Update( center, radius );
}
//--------------------------------------------------------------------------------
Sphere3f::~Sphere3f()
{
}
//--------------------------------------------------------------------------------
void Sphere3f::Update( const Vector3f& center, float radius ) 
{
	Center = center;
	Radius = radius;
}
//--------------------------------------------------------------------------------
bool Sphere3f::Intersects( const Sphere3f& Test ) const
{
	Vector3f Dist;
	Dist = ( Center - Test.Center ); 
	
	return( Dist.Magnitude( ) < ( Radius + Test.Radius ) );
}
//--------------------------------------------------------------------------------
bool Sphere3f::Envelops( const Sphere3f& test ) const
{
	Vector3f Dist;
	Dist = ( Center - test.Center ); 
	
	return( Radius > test.Radius + Dist.Magnitude( ) );
}
//--------------------------------------------------------------------------------
void Sphere3f::UpdateCenter( const Vector3f& center )
{
	Center = center;
}
//--------------------------------------------------------------------------------
void Sphere3f::UpdateRadius( float radius )
{
	Radius = radius;
}
//--------------------------------------------------------------------------------
eSHAPE Sphere3f::GetShapeType( ) const
{
	return( SPHERE );
}
//--------------------------------------------------------------------------------
void Sphere3f::SamplePosition( Vector3f& position, float theta, float phi ) const
{
	position.x = Radius * sinf( phi ) * cosf( theta ) + Center.x;
	position.y = Radius * cosf( phi ) + Center.y;
	position.z = Radius * sinf( phi ) * sinf( theta ) + Center.z;
}
//--------------------------------------------------------------------------------
void Sphere3f::SampleNormal( Vector3f& normal, float theta, float phi ) const
{
	normal.x = Radius * sinf( phi ) * cosf( theta );
	normal.y = Radius * cosf( phi );
	normal.z = Radius * sinf( phi ) * sinf( theta );

	normal = Vector3f::Normalize( normal );
}
//--------------------------------------------------------------------------------
void Sphere3f::SamplePositionAndNormal( Vector3f& position, Vector3f& normal, float theta, float phi ) const
{
	normal.x = Radius * sinf( phi ) * cosf( theta );
	normal.y = Radius * cosf( phi );
	normal.z = Radius * sinf( phi ) * sinf( theta );

	position = normal + Center;
	normal = Vector3f::Normalize( normal );
}
//--------------------------------------------------------------------------------