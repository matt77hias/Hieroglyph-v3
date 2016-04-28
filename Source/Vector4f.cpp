//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "Vector2f.h"
//----------------------------------------------------------------------------------------------------
using namespace Glyph3;
//----------------------------------------------------------------------------------------------------
Vector4f::Vector4f( )
{
}
//----------------------------------------------------------------------------------------------------
Vector4f::Vector4f( float X, float Y, float Z, float W )
{
	x = X;
	y = Y;
	z = Z;
	w = W;
}
//----------------------------------------------------------------------------------------------------
Vector4f::Vector4f( const Vector3f& Vector, float W )
{
    x = Vector.x;
    y = Vector.y;
    z = Vector.z;
    w = W;
}
//----------------------------------------------------------------------------------------------------
Vector4f::Vector4f( const Vector4f& Vector )
{
	x = Vector.x;
	y = Vector.y;
	z = Vector.z;
	w = Vector.w;
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator= ( const Vector4f& Vector )
{
	x = Vector.x;
	y = Vector.y;
	z = Vector.z;
	w = Vector.w;

    return *this;
}
//----------------------------------------------------------------------------------------------------
void Vector4f::MakeZero( )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}
//----------------------------------------------------------------------------------------------------
void Vector4f::Normalize( )
{
	float fInvMag = ( 1.0f / Magnitude() );

	x *= fInvMag;
	y *= fInvMag;
	z *= fInvMag;
	w *= fInvMag;
}
//----------------------------------------------------------------------------------------------------
float Vector4f::Magnitude( )
{
	float fLength = 0.0f;

	fLength += x * x;
	fLength += y * y;
	fLength += z * z;
	fLength += w * w;

	return( sqrt(fLength) );
}
//----------------------------------------------------------------------------------------------------
float Vector4f::Dot( Vector4f& Vector )
{
	float ret = 0.0f;
	
	ret += x * Vector.x;
	ret += y * Vector.y;
	ret += z * Vector.z;
	ret += w * Vector.w;

	return ret;
}
//----------------------------------------------------------------------------------------------------
float Vector4f::operator[] ( int iPos ) const
{
	if ( iPos == 0 ) return( x );
	if ( iPos == 1 ) return( y );
	if ( iPos == 2 ) return( z );
	return( w );
}
//----------------------------------------------------------------------------
float& Vector4f::operator[] ( int iPos )
{
	if ( iPos == 0 ) return( x );
	if ( iPos == 1 ) return( y );
	if ( iPos == 2 ) return( z );
	return( w );
}
//----------------------------------------------------------------------------
bool Vector4f::operator== ( const Vector4f& Vector ) const
{

	if ( ( x - Vector.x ) * ( x - Vector.x ) > 0.01f )
		return false;
	if ( ( y - Vector.y ) * ( y - Vector.y ) > 0.01f )
		return false;
	if ( ( z - Vector.z ) * ( z - Vector.z ) > 0.01f )
		return false;
	if ( ( w - Vector.w ) * ( w - Vector.w ) > 0.01f )
		return false;

	return true;
}
//----------------------------------------------------------------------------------------------------
bool Vector4f::operator!= ( const Vector4f& Vector ) const
{
    return( !( *this == Vector ) );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator+ ( const Vector4f& Vector ) const
{
	Vector4f sum;

	sum.x = x + Vector.x;
	sum.y = y + Vector.y;
	sum.z = z + Vector.z;
	sum.w = w + Vector.w;

	return( sum );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator- ( const Vector4f& Vector ) const
{
	Vector4f diff;

	diff.x = x - Vector.x;
	diff.y = y - Vector.y;
	diff.z = z - Vector.z;
	diff.w = w - Vector.w;

	return( diff );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator* ( float fScalar ) const
{
	Vector4f prod;

	prod.x = x * fScalar;
	prod.y = y * fScalar;
	prod.z = z * fScalar;
	prod.w = w * fScalar;

	return( prod );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator* ( const Vector4f& Vector ) const
{
    Vector4f prod;

    prod.x = x * Vector.x;
    prod.y = y * Vector.y;
    prod.z = z * Vector.z;
    prod.w = w * Vector.w;

    return( prod );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator/ ( float fScalar ) const
{
	Vector4f quot;
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;
		quot.x = x * fInvScalar;
		quot.y = y * fInvScalar;
		quot.z = z * fInvScalar;
		quot.w = w * fInvScalar;
	}
	else
	{
		quot.MakeZero();
	}

	return( quot );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator/ ( const Vector4f& Vector ) const
{
    Vector4f quot;
    quot.x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
    quot.y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
    quot.z = Vector.z != 0.0f ? z / Vector.z : 0.0f;
    quot.w = Vector.w != 0.0f ? w / Vector.w : 0.0f;

    return( quot );
}
//----------------------------------------------------------------------------------------------------
Vector4f Vector4f::operator- ( ) const
{
	Vector4f neg;

	neg.x = -x;
	neg.y = -y;
	neg.z = -z;
	neg.w = -w;

	return( neg );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator+= ( const Vector4f& Vector )
{
	x += Vector.x;
	y += Vector.y;
	z += Vector.z;
	w += Vector.w;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator-= ( const Vector4f& Vector )
{
	x -= Vector.x;
	y -= Vector.y;
	z -= Vector.z;
	w -= Vector.w;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator*= ( float fScalar )
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;
	w *= fScalar;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator*= ( const Vector4f& Vector )
{
    x *= Vector.x;
    y *= Vector.y;
    z *= Vector.z;
    w *= Vector.w;

    return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator/= ( float fScalar )
{
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;	
		x *= fInvScalar;
		y *= fInvScalar;
		z *= fInvScalar;
		w *= fInvScalar;
	}
	else
	{
		MakeZero();
	}

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector4f& Vector4f::operator/= ( const Vector4f& Vector )
{
    x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
    y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
    z = Vector.z != 0.0f ? z / Vector.z : 0.0f;
    w = Vector.w != 0.0f ? w / Vector.w : 0.0f;

    return( *this );
}
//----------------------------------------------------------------------------------------------------
void Vector4f::Clamp()
{
	if ( x > 1.0f ) x = 1.0f;
	if ( x < 0.0f ) x = 0.0f;

	if ( y > 1.0f ) y = 1.0f;
	if ( y < 0.0f ) y = 0.0f;

	if ( z > 1.0f ) z = 1.0f;
	if ( z < 0.0f ) z = 0.0f;

	if ( w > 1.0f ) w = 1.0f;
	if ( w < 0.0f ) w = 0.0f;
}
//----------------------------------------------------------------------------------------------------
unsigned int Vector4f::toARGB( )
{
	unsigned int result = 0;

	Clamp();

	result += (unsigned int)(255 * z);
	result += ((unsigned int)(255 * y) << 8);
	result += ((unsigned int)(255 * x) << 16);
	result += ((unsigned int)(255 * w) << 24);

	return( result );
}
//----------------------------------------------------------------------------------------------------
unsigned int Vector4f::toRGBA( )
{
	unsigned int result = 0;

	Clamp();

	result += (unsigned int)(255 * w);
	result += ((unsigned int)(255 * z) << 8);
	result += ((unsigned int)(255 * y) << 16);
	result += ((unsigned int)(255 * x) << 24);
	
	return( result );
}
//----------------------------------------------------------------------------------------------------
void Vector4f::fromARGB( unsigned int color )
{
	x = (float)((color & 0x00ff0000) >> 16)/(255.0f);	// red channel
	y = (float)((color & 0x0000ff00) >> 8)/(255.0f);	// green channel
	z = (float)((color & 0x000000ff))/(255.0f);		// blue channel
	w = (float)((color & 0xff000000) >> 24)/(255.0f);	// alpha channel
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector4f::xyz() const
{
	return( Vector3f( x, y, z ) );
}
//----------------------------------------------------------------------------------------------------
Vector2f Vector4f::xy() const
{
	return( Vector2f( x, y ) );
}
//----------------------------------------------------------------------------------------------------