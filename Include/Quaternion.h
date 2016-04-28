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
// Quaternion
//
//--------------------------------------------------------------------------------
#ifndef Quaternion_h
#define Quaternion_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include <cmath>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename Real>
	class Quaternion
	{
	public:
		Quaternion( );
		Quaternion( Real w, Real x, Real y, Real z );
		~Quaternion( );

		Real length() const;
		Real lengthSquared() const;
		Real dot( const Quaternion& a ) const;

		Quaternion conjugate() const;
		Quaternion inverse() const;

		Quaternion operator+( const Quaternion& a ) const;
		Quaternion operator-( const Quaternion& a ) const;
		Quaternion operator*( const Quaternion& a ) const;
		Quaternion operator/( const Quaternion& a ) const;
		Quaternion operator*( const Real& real ) const;
		Quaternion operator/( const Real& real ) const;

	public:
		Real w;
		Real x;
		Real y;
		Real z;
	};

	#include "Quaternion.inl"
};
//--------------------------------------------------------------------------------
#endif // Quaternion_h
//--------------------------------------------------------------------------------