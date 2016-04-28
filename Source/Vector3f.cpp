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
#include "Vector3f.h"
//----------------------------------------------------------------------------------------------------
using namespace Glyph3;
//----------------------------------------------------------------------------------------------------
Vector3f::Vector3f( )
{
}
//----------------------------------------------------------------------------------------------------
Vector3f::Vector3f( float X, float Y, float Z )
{
	x = X;
	y = Y;
	z = Z;
}
//----------------------------------------------------------------------------------------------------
Vector3f::Vector3f( const Vector3f& Vector )
{
	x = Vector.x;
	y = Vector.y;
	z = Vector.z;
}
//----------------------------------------------------------------------------------------------------
void Vector3f::MakeZero( )
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}
//----------------------------------------------------------------------------------------------------
void Vector3f::Normalize( )
{
	float Mag = Magnitude();
	if ( 0.0f == Mag ) Mag = 0.0001f;

	float fInvMag = ( 1.0f / Mag );

	x *= fInvMag;
	y *= fInvMag;
	z *= fInvMag;
}
//----------------------------------------------------------------------------------------------------
float Vector3f::Magnitude( )
{
	float fLength = 0.0f;

	fLength += x * x;
	fLength += y * y;
	fLength += z * z;

	return( sqrt( fLength ) );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Perpendicular( )
{
    float xAbs = fabs( x );
    float yAbs = fabs( y );
    float zAbs = fabs( z );
    float minVal = min( min( xAbs, yAbs ), zAbs );

    if ( xAbs == minVal )
        return Cross( Vector3f( 1.0f, 0.0f, 0.0f ) );
    else if ( yAbs == minVal )
        return Cross( Vector3f( 0.0f, 1.0f, 0.0f ) );
    else
        return Cross( Vector3f( 0.0f, 0.0f, 1.0f ) );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Cross( const Vector3f& Vector ) const
{
	Vector3f vRet; 
	
	vRet.x = y * Vector.z - z * Vector.y;
	vRet.y = z * Vector.x - x * Vector.z;
	vRet.z = x * Vector.y - y * Vector.x;
	
	return( vRet );
}
//----------------------------------------------------------------------------------------------------
float Vector3f::Dot( const Vector3f& Vector ) const
{
	float ret = 0.0f;
	
	ret  = x * Vector.x;
	ret += y * Vector.y;
	ret += z * Vector.z;

	return ret;
}
//----------------------------------------------------------------------------------------------------
void Vector3f::Clamp()
{
	if ( x > 1.0f ) x = 1.0f;
	if ( x < 0.0f ) x = 0.0f;

	if ( y > 1.0f ) y = 1.0f;
	if ( y < 0.0f ) y = 0.0f;

	if ( z > 1.0f ) z = 1.0f;
	if ( z < 0.0f ) z = 0.0f;
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Random( )
{
	float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
	
	Vector3f random = Vector3f( x, y, z );
	random.Normalize();

	return( random );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator= ( const Vector3f& Vector )
{
	x = Vector.x;
	y = Vector.y;
	z = Vector.z;

    return( *this );
}
//----------------------------------------------------------------------------------------------------
float Vector3f::operator[] ( int iPos ) const
{
	if ( iPos == 0 ) return( x );
	if ( iPos == 1 ) return( y );
	return( z );
}
//----------------------------------------------------------------------------
float& Vector3f::operator[] ( int iPos )
{
	if ( iPos == 0 ) return( x );
	if ( iPos == 1 ) return( y );
	return( z );
}
//----------------------------------------------------------------------------
bool Vector3f::operator== ( const Vector3f& Vector ) const
{

	if ( ( x - Vector.x ) * ( x - Vector.x ) > 0.01f )
		return false;
	if ( ( y - Vector.y ) * ( y - Vector.y ) > 0.01f )
		return false;
	if ( ( z - Vector.z ) * ( z - Vector.z ) > 0.01f )
		return false;

	return true;
}
//----------------------------------------------------------------------------------------------------
bool Vector3f::operator!= ( const Vector3f& Vector ) const
{
    return( !( *this == Vector ) );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator+ ( const Vector3f& Vector ) const
{
	Vector3f sum;

	sum.x = x + Vector.x;
	sum.y = y + Vector.y;
	sum.z = z + Vector.z;

	return( sum );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator- ( const Vector3f& Vector ) const
{
	Vector3f diff;

	diff.x = x - Vector.x;
	diff.y = y - Vector.y;
	diff.z = z - Vector.z;

	return( diff );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator* ( float fScalar ) const
{
	Vector3f prod;

	prod.x = x * fScalar;
	prod.y = y * fScalar;
	prod.z = z * fScalar;

	return( prod );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator* ( const Vector3f& Vector ) const
{
    Vector3f prod;

    prod.x = x * Vector.x;
    prod.y = y * Vector.y;
    prod.z = z * Vector.z;

    return( prod );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator/ ( float fScalar ) const
{
	Vector3f quot;
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;
		quot.x = x * fInvScalar;
		quot.y = y * fInvScalar;
		quot.z = z * fInvScalar;
	}
	else
	{
		quot.MakeZero();
	}

	return( quot );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator/ ( const Vector3f& Vector ) const
{
    Vector3f quot;
    quot.x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
    quot.y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
    quot.z = Vector.z != 0.0f ? z / Vector.z : 0.0f;

    return( quot );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::operator- ( ) const
{
	Vector3f neg;

	neg.x = -x;
	neg.y = -y;
	neg.z = -z;

	return( neg );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator+= ( const Vector3f& Vector )
{
	x += Vector.x;
	y += Vector.y;
	z += Vector.z;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator-= ( const Vector3f& Vector )
{
	x -= Vector.x;
	y -= Vector.y;
	z -= Vector.z;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator*= ( float fScalar )
{
	x *= fScalar;
	y *= fScalar;
	z *= fScalar;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator*= ( const Vector3f& Vector )
{
    x *= Vector.x;
    y *= Vector.y;
    z *= Vector.z;

    return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator/= ( float fScalar )
{
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;	
		x *= fInvScalar;
		y *= fInvScalar;
		z *= fInvScalar;
	}
	else
	{
		MakeZero();
	}

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f& Vector3f::operator/= ( const Vector3f& Vector )
{
    x = Vector.x != 0.0f ? x / Vector.x : 0.0f;
    y = Vector.y != 0.0f ? y / Vector.y : 0.0f;
    z = Vector.z != 0.0f ? z / Vector.z : 0.0f;

    return( *this );
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Clamp( const Vector3f& A )
{
    Vector3f vec = A;
    vec.Clamp();
    return vec;
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Cross( const Vector3f& A, const Vector3f& B )
{    
    return A.Cross( B );
}
//----------------------------------------------------------------------------------------------------
float Vector3f::Dot( const Vector3f& A, const Vector3f& B )
{    
    return A.Dot( B );
}
//----------------------------------------------------------------------------------------------------
float Vector3f::Magnitude(const Vector3f& A)
{
	return sqrt( Dot(A, A) );
}
//----------------------------------------------------------------------------------------------------
float Vector3f::LengthSq( const Vector3f& A )
{
    return Dot(A, A);
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Normalize( const Vector3f& A )
{
    Vector3f vec = A;
    vec.Normalize();
    return vec;
}
//----------------------------------------------------------------------------------------------------
Vector3f Vector3f::Perpendicular( const Vector3f& A )
{
    Vector3f vec = A;
    return vec.Perpendicular();    
}
//----------------------------------------------------------------------------------------------------
