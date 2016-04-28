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
#include "Matrix3f.h"
//----------------------------------------------------------------------------------------------------
using namespace Glyph3;
//----------------------------------------------------------------------------------------------------
Matrix3f::Matrix3f()
{
}
//----------------------------------------------------------------------------------------------------
Matrix3f::Matrix3f( bool bZero )
{
	if ( bZero )
		memset( m_afEntry, 0, 3*3*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
Matrix3f::Matrix3f( const Matrix3f& Matrix )
{
    memcpy( m_afEntry, (void*)&Matrix, 9*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
Matrix3f::Matrix3f(	float fM11, float fM12, float fM13,
						float fM21, float fM22, float fM23,
						float fM31, float fM32, float fM33 )
{
	m_afEntry[0] = fM11;
	m_afEntry[1] = fM12;
	m_afEntry[2] = fM13;

	m_afEntry[3] = fM21;
	m_afEntry[4] = fM22;
	m_afEntry[5] = fM23;

	m_afEntry[6] = fM31;
	m_afEntry[7] = fM32;
	m_afEntry[8] = fM33;
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator= ( const Matrix3f& Matrix )
{
    memcpy( m_afEntry, Matrix.m_afEntry, 9*sizeof(float) );
    return *this;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::RotationX( float fRadians )
{
    float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = 1.0f;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = 0.0f;

	m_afEntry[ 3] = 0.0f;
	m_afEntry[ 4] = fCos;
	m_afEntry[ 5] = fSin;

	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = -fSin;
	m_afEntry[ 8] = fCos;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::RotationY( float fRadians )
{
	float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = fCos;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = -fSin;

	m_afEntry[ 3] = 0.0f;
	m_afEntry[ 4] = 1.0f;
	m_afEntry[ 5] = 0.0f;

	m_afEntry[ 6] = fSin;
	m_afEntry[ 7] = 0.0f;
	m_afEntry[ 8] = fCos;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::RotationZ( float fRadians )
{
	float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = fCos;
	m_afEntry[ 1] = fSin;
	m_afEntry[ 2] = 0.0f;

	m_afEntry[ 3] = -fSin;
	m_afEntry[ 4] = fCos;
	m_afEntry[ 5] = 0.0f;

	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = 0.0f;
	m_afEntry[ 8] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::Rotation( Vector3f& Rot )
{
	Matrix3f mRot1;
	Matrix3f mRot2;
	
	mRot1.RotationZ(Rot.z);
	mRot2.RotationX(Rot.x);
	mRot1 *= mRot2;
	mRot2.RotationY(Rot.y);
	mRot1 *= mRot2;
	*this = mRot1;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::RotationZYX( Vector3f& Rot )
{
	Matrix3f mRot1;
	Matrix3f mRot2;
	
	mRot1.RotationZ(Rot.z);
	mRot2.RotationY(Rot.y);
	mRot1 *= mRot2;
	mRot2.RotationX(Rot.x);
	mRot1 *= mRot2;
	*this = mRot1;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::RotationEuler( Vector3f& Axis, float Angle )
{
	float s = sinf( Angle );
	float c = cosf( Angle );
	float t = 1 - c;

	m_afEntry[0] = t*Axis.x*Axis.x + c;
	m_afEntry[1] = t*Axis.x*Axis.y + s*Axis.z;
	m_afEntry[2] = t*Axis.x*Axis.z - s*Axis.y;

	m_afEntry[3] = t*Axis.x*Axis.y - s*Axis.z;
	m_afEntry[4] = t*Axis.y*Axis.y + c;
	m_afEntry[5] = t*Axis.y*Axis.z + s*Axis.x;

	m_afEntry[6] = t*Axis.x*Axis.y + s*Axis.y;
	m_afEntry[7] = t*Axis.y*Axis.z - s*Axis.x;
	m_afEntry[8] = t*Axis.z*Axis.z + c;
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::Orthonormalize()
{
	// This method is taken from the Wild Magic library v3.11, available at
	// http://www.geometrictools.com.

    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

	//(1.0/sqrt((double)fValue))

    // compute q0
    float fInvLength = static_cast<float>(1.0/sqrt((double)(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[3]*m_afEntry[3] + m_afEntry[6]*m_afEntry[6])));

    m_afEntry[0] *= fInvLength;
    m_afEntry[3] *= fInvLength;
    m_afEntry[6] *= fInvLength;

    // compute q1
    float fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[3]*m_afEntry[4] +
        m_afEntry[6]*m_afEntry[7];

    m_afEntry[1] -= fDot0*m_afEntry[0];
    m_afEntry[4] -= fDot0*m_afEntry[3];
    m_afEntry[7] -= fDot0*m_afEntry[6];

    fInvLength = static_cast<float>(1.0/sqrt((double)(m_afEntry[1]*m_afEntry[1] +
        m_afEntry[4]*m_afEntry[4] + m_afEntry[7]*m_afEntry[7])));

    m_afEntry[1] *= fInvLength;
    m_afEntry[4] *= fInvLength;
    m_afEntry[7] *= fInvLength;

    // compute q2
    float fDot1 = m_afEntry[1]*m_afEntry[2] + m_afEntry[4]*m_afEntry[5] +
        m_afEntry[7]*m_afEntry[8];

    fDot0 = m_afEntry[0]*m_afEntry[2] + m_afEntry[3]*m_afEntry[5] +
        m_afEntry[6]*m_afEntry[8];

    m_afEntry[2] -= fDot0*m_afEntry[0] + fDot1*m_afEntry[1];
    m_afEntry[5] -= fDot0*m_afEntry[3] + fDot1*m_afEntry[4];
    m_afEntry[8] -= fDot0*m_afEntry[6] + fDot1*m_afEntry[7];

    fInvLength = static_cast<float>(1.0/sqrt((double)(m_afEntry[2]*m_afEntry[2] +
        m_afEntry[5]*m_afEntry[5] + m_afEntry[8]*m_afEntry[8])));

    m_afEntry[2] *= fInvLength;
    m_afEntry[5] *= fInvLength;
    m_afEntry[8] *= fInvLength;
}
//----------------------------------------------------------------------------
float Matrix3f::operator[] ( int iPos ) const
{
    return( m_afEntry[iPos] );
}
//----------------------------------------------------------------------------
float& Matrix3f::operator[] ( int iPos )
{
    return( m_afEntry[iPos] );
}
//----------------------------------------------------------------------------
float Matrix3f::operator() ( int iRow, int iCol ) const
{
    return( m_afEntry[I(iRow,iCol)] );
}
//----------------------------------------------------------------------------
float& Matrix3f::operator() ( int iRow, int iCol )
{
    return( m_afEntry[I(iRow,iCol)] );
}
//----------------------------------------------------------------------------
bool Matrix3f::operator== ( const Matrix3f& Matrix ) const
{
    return( memcmp( m_afEntry, Matrix.m_afEntry, 3*3*sizeof(float) ) == 0 );
}
//----------------------------------------------------------------------------------------------------
bool Matrix3f::operator!= ( const Matrix3f& Matrix ) const
{
    return( memcmp( m_afEntry, Matrix.m_afEntry, 3*3*sizeof(float) ) != 0 );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator* ( const Matrix3f& Matrix ) const
{
	Matrix3f mProd;

	for ( int iRow = 0; iRow < 3; iRow++ ) 
	{
		for ( int iCol = 0; iCol < 3; iCol++ )
		{
			int i = I( iRow, iCol );
			mProd.m_afEntry[i] = 0.0f;
			for ( int iMid = 0; iMid < 3; iMid++ )
			{
				mProd.m_afEntry[i] += 
					m_afEntry[I(iRow, iMid)] * Matrix.m_afEntry[I(iMid, iCol)];
			}
		}
	}
	return( mProd );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator+ ( const Matrix3f& Matrix ) const
{
	Matrix3f mSum;

	for ( int i = 0; i < 3*3; i++ )
		mSum.m_afEntry[i] = m_afEntry[i] + Matrix.m_afEntry[i];

	return( mSum );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator- ( const Matrix3f& Matrix ) const
{
	Matrix3f mDiff;

	for ( int i = 0; i < 3*3; i++ )
		mDiff.m_afEntry[i] = m_afEntry[i] - Matrix.m_afEntry[i];

	return( mDiff );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator* ( float fScalar ) const
{
	Matrix3f mProd;

	for ( int i = 0; i < 3*3; i++ )
		mProd.m_afEntry[i] = m_afEntry[i] * fScalar;

	return( mProd );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator/ ( float fScalar ) const
{
	Matrix3f mQuot;
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;
		for ( int i = 0; i < 3*3; i++ )
			mQuot.m_afEntry[i] = m_afEntry[i] * fInvScalar;
	}
	else
	{
		for ( int i = 0; i < 3*3; i++ )
			mQuot.m_afEntry[i] = 0;
	}

	return( mQuot );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::operator- () const
{
	Matrix3f mNeg;

	for ( int i = 0; i < 3*3; i++ )
		mNeg.m_afEntry[i] = -m_afEntry[i];

	return( mNeg );
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator+= ( const Matrix3f& Matrix )
{
	for ( int i = 0; i < 3*3; i++ )
		m_afEntry[i] += Matrix.m_afEntry[i];

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator-= ( const Matrix3f& Matrix )
{
	for ( int i = 0; i < 3*3; i++ )
		m_afEntry[i] -= Matrix.m_afEntry[i];

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator*= ( float fScalar )
{
	for ( int i = 0; i < 3*3; i++ )
		m_afEntry[i] *= fScalar;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator*= ( const Matrix3f& Matrix )
{
	Matrix3f mProd = *this; 
	
	for ( int iRow = 0; iRow < 3; iRow++ )
	{
		for ( int iCol = 0; iCol < 3; iCol++ )
		{
			int i = I( iRow, iCol );
			m_afEntry[i] = 0.0f;
			for ( int iMid = 0; iMid < 3; iMid++ )
			{
				m_afEntry[i] += 
					mProd.m_afEntry[I(iRow, iMid)] * Matrix.m_afEntry[I(iMid, iCol)];
			}
		}
	}
	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix3f& Matrix3f::operator/= ( float fScalar )
{
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;	
		for ( int i = 0; i < 3*3; i++ )
			m_afEntry[i] *= fInvScalar;
	}
	else
	{
		for ( int i = 0; i < 3*3; i++ )
			m_afEntry[i] = 0;
	}

	return( *this );
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::MakeZero()
{
	memset( m_afEntry,0,3*3*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::MakeIdentity()
{
	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
			if (iRow == iCol)
                m_afEntry[I(iRow,iCol)] = 1.0f;
			else
				m_afEntry[I(iRow,iCol)] = 0.0f;
		}
	}
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::MakeTranspose()
{
	Matrix3f mTranspose;

	for ( int iRow = 0; iRow < 3; iRow++ )
	{
		for ( int iCol = 0; iCol < 3; iCol++ )
			mTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
	}
    
	memcpy( m_afEntry, mTranspose.m_afEntry, 3*3*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::Zero()
{
	Matrix3f mReturn = Matrix3f( true );

	return( mReturn );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::Identity()
{
	Matrix3f mIdent;

	for ( int iRow = 0; iRow < 3; iRow++ )
	{
		for ( int iCol = 0; iCol < 3; iCol++ )
		{
			if ( iRow == iCol )
                mIdent.m_afEntry[I(iRow,iCol)] = 1.0f;
			else
				mIdent.m_afEntry[I(iRow,iCol)] = 0.0f;
		}
	}
	return( mIdent );
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix3f::Transpose()
{
	Matrix3f mTranspose;

	for ( int iRow = 0; iRow < 3; iRow++ )
	{
		for ( int iCol = 0; iCol < 3; iCol++ )
			mTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
	}
	return( mTranspose );
}
//----------------------------------------------------------------------------------------------------
int Matrix3f::I( int iRow, int iCol )
{
    return( 3*iRow + iCol );
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix3f::operator* ( const Vector3f& Vector ) const
{
    Vector3f vProd;
    for (int iCol = 0; iCol < 3; iCol++)
    {
        vProd[iCol] = 0.0f;
        for (int iRow = 0; iRow < 3; iRow++)
            vProd[iCol] += m_afEntry[I(iRow,iCol)] * Vector[iRow];
    }
    return( vProd );
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::SetRow( int iRow, const Vector3f& Vector )
{
	for ( int iCol = 0; iCol < 3; iCol++ )
        m_afEntry[I(iRow,iCol)] = Vector[iCol];
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix3f::GetRow( int iRow ) const
{
	Vector3f vRow;
	for ( int iCol = 0; iCol < 3; iCol++ )
        vRow[iCol] = m_afEntry[I(iRow,iCol)];

	return( vRow );
}
//----------------------------------------------------------------------------------------------------
void Matrix3f::SetColumn( int iCol, const Vector3f& Vector )
{
	for ( int iRow = 0; iRow < 3; iRow++ )
        m_afEntry[I(iRow,iCol)] = Vector[iRow];
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix3f::GetColumn( int iCol ) const
{
	Vector3f vCol;
	for ( int iRow = 0; iRow < 3; iRow++ )
		vCol[iRow] = m_afEntry[I(iRow,iCol)];

	return( vCol );
}
//----------------------------------------------------------------------------------------------------
