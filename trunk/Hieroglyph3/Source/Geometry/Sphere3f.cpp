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
Sphere3f::Sphere3f( Vector3f& center, float radius )
{
	Update( center, radius );
}
//--------------------------------------------------------------------------------
Sphere3f::~Sphere3f()
{
}
//--------------------------------------------------------------------------------
void Sphere3f::Update( Vector3f& center, float radius ) 
{
	Center = center;
	Radius = radius;
}
//--------------------------------------------------------------------------------
//Vector3f& Sphere3f::Center( )
//{
//	return( C );
//}
////--------------------------------------------------------------------------------
//float Sphere3f::Radius( )
//{
//	return( r );
//}
//--------------------------------------------------------------------------------
bool Sphere3f::Intersects( Sphere3f& Test )
{
	Vector3f Dist;
	Dist = ( Center - Test.Center ); 
	
	return( Dist.Magnitude( ) < ( Radius + Test.Radius ) );
}
//--------------------------------------------------------------------------------
bool Sphere3f::Envelops( Sphere3f& test )
{
	Vector3f Dist;
	Dist = ( Center - test.Center ); 
	
	return( Radius > test.Radius + Dist.Magnitude( ) );
}
//--------------------------------------------------------------------------------
void Sphere3f::UpdateCenter( Vector3f& center )
{
	Center = center;
}
//--------------------------------------------------------------------------------
void Sphere3f::UpdateRadius( float radius )
{
	Radius = radius;
}
//--------------------------------------------------------------------------------
eSHAPE Sphere3f::GetShapeType( )
{
	return( SPHERE );
}
//--------------------------------------------------------------------------------