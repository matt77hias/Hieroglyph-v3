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
template <typename Real>
Quaternion<Real>::Quaternion( ) :
	w( 0 ),
	x( 0 ),
	y( 0 ),
	z( 0 )	
{
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::Quaternion( Real W, Real X, Real Y, Real Z ) :
	w( W ),
	x( X ),
	y( Y ),
	z( Z )
{
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real>::~Quaternion( )
{
}
//--------------------------------------------------------------------------------
template <typename Real>
Real Quaternion<Real>::length() const
{
	return( sqrt( w*w + x*x + y*y + z*z ) );
}
//--------------------------------------------------------------------------------
template <typename Real>
Real Quaternion<Real>::lengthSquared() const
{
	return( w*w + x*x + y*y + z*z );
}
//--------------------------------------------------------------------------------
template <typename Real>
Real Quaternion<Real>::dot( const Quaternion<Real>& a ) const
{
	return(	w*a.w + x*a.x + y*a.y + z*a.z );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::conjugate() const
{
	return( Quaternion<Real>( w, -x, -y, -z ) );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::inverse() const
{
	return( conjugate() / lengthSquared() );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator+( const Quaternion<Real>& a ) const
{
	return( Quaternion<Real>( a.w+w, a.x+x, a.y+y, a.z+z ) );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator-( const Quaternion& a ) const
{
	return( Quaternion<Real>( a.w-w, a.x-x, a.y-y, a.z-z ) );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator*( const Quaternion<Real>& a ) const
{
	Quaternion q;

	q.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	q.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
	q.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
	q.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;

	return( q );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator/( const Quaternion<Real>& a ) const
{
	return( *this * a.inverse() );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator*( const Real& real ) const
{
	return( Quaternion<Real>( w*real, x*real, y*real, z*real ) );
}
//--------------------------------------------------------------------------------
template <typename Real>
Quaternion<Real> Quaternion<Real>::operator/( const Real& real ) const
{
	assert( real != 0.0 );

	return( Quaternion<Real>( w/real, x/real, y/real, z/real ) );
}
//--------------------------------------------------------------------------------
