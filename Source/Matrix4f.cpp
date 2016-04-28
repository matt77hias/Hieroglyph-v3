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
#include "Matrix4f.h"
//----------------------------------------------------------------------------------------------------
using namespace Glyph3;
//----------------------------------------------------------------------------------------------------
Matrix4f::Matrix4f()
{
}
//----------------------------------------------------------------------------------------------------
Matrix4f::Matrix4f( bool bZero )
{
	if (bZero)
		memset(m_afEntry, 0, 4*4*sizeof(float));
}
//----------------------------------------------------------------------------------------------------
Matrix4f::Matrix4f( const Matrix4f& Matrix )
{
    memcpy( m_afEntry, (void*)&Matrix, 16*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
Matrix4f::Matrix4f(	float fM11, float fM12, float fM13, float fM14,
						float fM21, float fM22, float fM23, float fM24,
						float fM31, float fM32, float fM33, float fM34,
						float fM41, float fM42, float fM43, float fM44 )
{
	m_afEntry[ 0] = fM11;
	m_afEntry[ 1] = fM12;
	m_afEntry[ 2] = fM13;
	m_afEntry[ 3] = fM14;

	m_afEntry[ 4] = fM21;
	m_afEntry[ 5] = fM22;
	m_afEntry[ 6] = fM23;
	m_afEntry[ 7] = fM24;

	m_afEntry[ 8] = fM31;
	m_afEntry[ 9] = fM32;
	m_afEntry[10] = fM33;
	m_afEntry[11] = fM34;

	m_afEntry[12] = fM41;
	m_afEntry[13] = fM42;
	m_afEntry[14] = fM43;
	m_afEntry[15] = fM44;
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator= ( const Matrix4f& Matrix )
{
    memcpy( m_afEntry, Matrix.m_afEntry, 16*sizeof(float) );
    return( *this );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::RotationX( float fRadians )
{
    float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = 1.0f;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = 0.0f;
	m_afEntry[ 3] = 0.0f;

	m_afEntry[ 4] = 0.0f;
	m_afEntry[ 5] = fCos;
	m_afEntry[ 6] = fSin;
	m_afEntry[ 7] = 0.0f;

	m_afEntry[ 8] = 0.0f;
	m_afEntry[ 9] = -fSin;
	m_afEntry[10] = fCos;
	m_afEntry[11] = 0.0f;

	m_afEntry[12] = 0.0f;
	m_afEntry[13] = 0.0f;
	m_afEntry[14] = 0.0f;
	m_afEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::RotationY( float fRadians )
{
	float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = fCos;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = -fSin;
	m_afEntry[ 3] = 0.0f;

	m_afEntry[ 4] = 0.0f;
	m_afEntry[ 5] = 1.0f;
	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = 0.0f;

	m_afEntry[ 8] = fSin;
	m_afEntry[ 9] = 0.0f;
	m_afEntry[10] = fCos;
	m_afEntry[11] = 0.0f;

	m_afEntry[12] = 0.0f;
	m_afEntry[13] = 0.0f;
	m_afEntry[14] = 0.0f;
	m_afEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::RotationZ( float fRadians )
{
	float fSin = sinf( fRadians );
	float fCos = cosf( fRadians );

	m_afEntry[ 0] = fCos;
	m_afEntry[ 1] = fSin;
	m_afEntry[ 2] = 0.0f;
	m_afEntry[ 3] = 0.0f;

	m_afEntry[ 4] = -fSin;
	m_afEntry[ 5] = fCos;
	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = 0.0f;

	m_afEntry[ 8] = 0.0f;
	m_afEntry[ 9] = 0.0f;
	m_afEntry[10] = 1.0f;
	m_afEntry[11] = 0.0f;

	m_afEntry[12] = 0.0f;
	m_afEntry[13] = 0.0f;
	m_afEntry[14] = 0.0f;
	m_afEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::Scale( float fScale )
{
	m_afEntry[ 0] = fScale;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = 0.0f;
	m_afEntry[ 3] = 0.0f;

	m_afEntry[ 4] = 0.0f;
	m_afEntry[ 5] = fScale;
	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = 0.0f;

	m_afEntry[ 8] = 0.0f;
	m_afEntry[ 9] = 0.0f;
	m_afEntry[10] = fScale;
	m_afEntry[11] = 0.0f;

	m_afEntry[12] = 0.0f;
	m_afEntry[13] = 0.0f;
	m_afEntry[14] = 0.0f;
	m_afEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::Translate( float fX, float fY, float fZ )
{
	m_afEntry[ 0] = 1.0f;
	m_afEntry[ 1] = 0.0f;
	m_afEntry[ 2] = 0.0f;
	m_afEntry[ 3] = 0.0f;

	m_afEntry[ 4] = 0.0f;
	m_afEntry[ 5] = 1.0f;
	m_afEntry[ 6] = 0.0f;
	m_afEntry[ 7] = 0.0f;

	m_afEntry[ 8] = 0.0f;
	m_afEntry[ 9] = 0.0f;
	m_afEntry[10] = 1.0f;
	m_afEntry[11] = 0.0f;

	m_afEntry[12] = fX;
	m_afEntry[13] = fY;
	m_afEntry[14] = fZ;
	m_afEntry[15] = 1.0f;
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::Inverse() const
{
    float fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
    float fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
    float fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
    float fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
    float fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
    float fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
    float fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
    float fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
    float fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
    float fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
    float fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
    float fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];

    float fDet = fA0*fB5 - fA1*fB4 + fA2*fB3 + fA3*fB2 - fA4*fB1 + fA5*fB0;
    //if ( Math<Real>::FAbs(fDet) <= Math<Real>::EPSILON )
    //    return Matrix4::ZERO;

    Matrix4f kInv;
    kInv(0,0) = + m_afEntry[ 5]*fB5 - m_afEntry[ 6]*fB4 + m_afEntry[ 7]*fB3;
    kInv(1,0) = - m_afEntry[ 4]*fB5 + m_afEntry[ 6]*fB2 - m_afEntry[ 7]*fB1;
    kInv(2,0) = + m_afEntry[ 4]*fB4 - m_afEntry[ 5]*fB2 + m_afEntry[ 7]*fB0;
    kInv(3,0) = - m_afEntry[ 4]*fB3 + m_afEntry[ 5]*fB1 - m_afEntry[ 6]*fB0;
    kInv(0,1) = - m_afEntry[ 1]*fB5 + m_afEntry[ 2]*fB4 - m_afEntry[ 3]*fB3;
    kInv(1,1) = + m_afEntry[ 0]*fB5 - m_afEntry[ 2]*fB2 + m_afEntry[ 3]*fB1;
    kInv(2,1) = - m_afEntry[ 0]*fB4 + m_afEntry[ 1]*fB2 - m_afEntry[ 3]*fB0;
    kInv(3,1) = + m_afEntry[ 0]*fB3 - m_afEntry[ 1]*fB1 + m_afEntry[ 2]*fB0;
    kInv(0,2) = + m_afEntry[13]*fA5 - m_afEntry[14]*fA4 + m_afEntry[15]*fA3;
    kInv(1,2) = - m_afEntry[12]*fA5 + m_afEntry[14]*fA2 - m_afEntry[15]*fA1;
    kInv(2,2) = + m_afEntry[12]*fA4 - m_afEntry[13]*fA2 + m_afEntry[15]*fA0;
    kInv(3,2) = - m_afEntry[12]*fA3 + m_afEntry[13]*fA1 - m_afEntry[14]*fA0;
    kInv(0,3) = - m_afEntry[ 9]*fA5 + m_afEntry[10]*fA4 - m_afEntry[11]*fA3;
    kInv(1,3) = + m_afEntry[ 8]*fA5 - m_afEntry[10]*fA2 + m_afEntry[11]*fA1;
    kInv(2,3) = - m_afEntry[ 8]*fA4 + m_afEntry[ 9]*fA2 - m_afEntry[11]*fA0;
    kInv(3,3) = + m_afEntry[ 8]*fA3 - m_afEntry[ 9]*fA1 + m_afEntry[10]*fA0;

    float fInvDet = ((float)1.0)/fDet;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kInv(iRow,iCol) *= fInvDet;
    }

    return( kInv );
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix4f::GetBasisX() const
{
	Vector3f Basis;

	for (int i = 0; i < 3; i++)
        Basis[i] = m_afEntry[I(0,i)];
	
	return( Basis );
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix4f::GetBasisY() const
{
	Vector3f Basis;

	for (int i = 0; i < 3; i++)
        Basis[i] = m_afEntry[I(1,i)];
	
	return( Basis );
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix4f::GetBasisZ() const
{
	Vector3f Basis;

	for (int i = 0; i < 3; i++)
        Basis[i] = m_afEntry[I(2,i)];
	
	return( Basis );
}
//----------------------------------------------------------------------------------------------------
Vector3f Matrix4f::GetTranslation() const
{
	Vector3f Pos;

	for (int i = 0; i < 3; i++)
		Pos[i] = m_afEntry[I(3,i)];

	return( Pos );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::SetTranslation( const Vector3f& Trans )
{
	for (int i = 0; i < 3; i++)
		m_afEntry[I(3,i)] = Trans[i];
}
//----------------------------------------------------------------------------------------------------
Matrix3f Matrix4f::GetRotation() const
{
	Matrix3f mRet;

	mRet.SetRow( 0, GetBasisX() );
	mRet.SetRow( 1, GetBasisY() );
	mRet.SetRow( 2, GetBasisZ() );

	return( mRet );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::SetRotation( const Matrix3f& Rot )
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
            m_afEntry[I(i,j)] = Rot[(3*i+j)];
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::RotationMatrixXYZ( float fRadiansX, float fRadiansY, float fRadiansZ )
{
	//Matrix4f mRot1;
	//Matrix4f mRot2;
	//
	//mRot1.RotationZ(fRadiansZ);
	//mRot2.RotationX(fRadiansX);
	//mRot1 *= mRot2;
	//mRot2.RotationY(fRadiansY);
	//mRot1 *= mRot2;
	//*this = mRot1;

	return Matrix4f::RotationMatrixZ( fRadiansZ ) * Matrix4f::RotationMatrixX( fRadiansX ) * Matrix4f::RotationMatrixY( fRadiansY );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::RotationMatrixX( float fRadians )
{
	Matrix4f ret;
	ret.RotationX( fRadians );
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::RotationMatrixY( float fRadians )
{
	Matrix4f ret;
	ret.RotationY( fRadians );
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::RotationMatrixZ( float fRadians )
{
	Matrix4f ret;
	ret.RotationZ( fRadians );
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::ScaleMatrix( float fScale )
{
	Matrix4f ret;
	ret.Scale( fScale );
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::ScaleMatrix( const Vector3f& scale )
{
	Matrix4f ret;

	ret.m_afEntry[ 0] = scale.x;
	ret.m_afEntry[ 1] = 0.0f;
	ret.m_afEntry[ 2] = 0.0f;
	ret.m_afEntry[ 3] = 0.0f;

	ret.m_afEntry[ 4] = 0.0f;
	ret.m_afEntry[ 5] = scale.y;
	ret.m_afEntry[ 6] = 0.0f;
	ret.m_afEntry[ 7] = 0.0f;

	ret.m_afEntry[ 8] = 0.0f;
	ret.m_afEntry[ 9] = 0.0f;
	ret.m_afEntry[10] = scale.z;
	ret.m_afEntry[11] = 0.0f;

	ret.m_afEntry[12] = 0.0f;
	ret.m_afEntry[13] = 0.0f;
	ret.m_afEntry[14] = 0.0f;
	ret.m_afEntry[15] = 1.0f;
	
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::ScaleMatrixXYZ( float fX, float fY, float fZ )
{
	Matrix4f ret;

	ret.m_afEntry[ 0] = fX;
	ret.m_afEntry[ 1] = 0.0f;
	ret.m_afEntry[ 2] = 0.0f;
	ret.m_afEntry[ 3] = 0.0f;

	ret.m_afEntry[ 4] = 0.0f;
	ret.m_afEntry[ 5] = fY;
	ret.m_afEntry[ 6] = 0.0f;
	ret.m_afEntry[ 7] = 0.0f;

	ret.m_afEntry[ 8] = 0.0f;
	ret.m_afEntry[ 9] = 0.0f;
	ret.m_afEntry[10] = fZ;
	ret.m_afEntry[11] = 0.0f;

	ret.m_afEntry[12] = 0.0f;
	ret.m_afEntry[13] = 0.0f;
	ret.m_afEntry[14] = 0.0f;
	ret.m_afEntry[15] = 1.0f;
	
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::TranslationMatrix( float fX, float fY, float fZ )
{
	Matrix4f ret;
	ret.Translate( fX, fY, fZ );
	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::LookAtLHMatrix( Vector3f& eye, Vector3f& at, Vector3f& up )
{
	// This method is based on the method of the same name from the D3DX library.

	Matrix4f ret;

	Vector3f zaxis = at - eye; 
	zaxis.Normalize();
	
	Vector3f xaxis = up.Cross( zaxis );
	xaxis.Normalize();

	Vector3f yaxis = zaxis.Cross( xaxis );

	ret.m_afEntry[ 0] = xaxis.x;
	ret.m_afEntry[ 1] = yaxis.x;
	ret.m_afEntry[ 2] = zaxis.x;
	ret.m_afEntry[ 3] = 0.0f;

	ret.m_afEntry[ 4] = xaxis.y;
	ret.m_afEntry[ 5] = yaxis.y;
	ret.m_afEntry[ 6] = zaxis.y;
	ret.m_afEntry[ 7] = 0.0f;

	ret.m_afEntry[ 8] = xaxis.z;
	ret.m_afEntry[ 9] = yaxis.z;
	ret.m_afEntry[10] = zaxis.z;
	ret.m_afEntry[11] = 0.0f;

	ret.m_afEntry[12] = -(xaxis.Dot(eye));
	ret.m_afEntry[13] = -(yaxis.Dot(eye));
	ret.m_afEntry[14] = -(zaxis.Dot(eye));
	ret.m_afEntry[15] = 1.0f;

	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::PerspectiveFovLHMatrix( float fovy, float aspect, float zn, float zf )
{
	// This method is based on the method of the same name from the D3DX library.

	Matrix4f ret;

	float tanY = tan( fovy / 2.0f );
	if ( 0.0f == tanY ) tanY = 0.001f;
	float yScale = 1.0f / tanY;

	if ( 0.0f == aspect ) aspect = 0.001f;
	float xScale = yScale / aspect;

	ret.m_afEntry[ 0] = xScale;
	ret.m_afEntry[ 1] = 0.0f;
	ret.m_afEntry[ 2] = 0.0f;
	ret.m_afEntry[ 3] = 0.0f;

	ret.m_afEntry[ 4] = 0.0f;
	ret.m_afEntry[ 5] = yScale;
	ret.m_afEntry[ 6] = 0.0f;
	ret.m_afEntry[ 7] = 0.0f;

	ret.m_afEntry[ 8] = 0.0f;
	ret.m_afEntry[ 9] = 0.0f;
	ret.m_afEntry[10] = zf / ( zf-zn );
	ret.m_afEntry[11] = 1.0f;

	ret.m_afEntry[12] = 0.0f;
	ret.m_afEntry[13] = 0.0f;
	ret.m_afEntry[14] = -zn*zf / ( zf-zn );
	ret.m_afEntry[15] = 0.0f;

	return( ret );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::OrthographicLHMatrix( float zn, float zf, float width, float height )
{
	// This method is based on the method of the same name from the D3DX library.

	Matrix4f ret;


	if ( zn == zf ) zf = zn + 0.1f;
	if ( width <= 0.0f ) width = 1.0f;
	if ( height <= 0.0f ) height = 1.0f;

	ret.m_afEntry[ 0] = 2.0f / width;
	ret.m_afEntry[ 1] = 0.0f;
	ret.m_afEntry[ 2] = 0.0f;
	ret.m_afEntry[ 3] = 0.0f;

	ret.m_afEntry[ 4] = 0.0f;
	ret.m_afEntry[ 5] = 2.0f / height;
	ret.m_afEntry[ 6] = 0.0f;
	ret.m_afEntry[ 7] = 0.0f;

	ret.m_afEntry[ 8] = 0.0f;
	ret.m_afEntry[ 9] = 0.0f;
	ret.m_afEntry[10] = 1.0f / ( zf-zn );
	ret.m_afEntry[11] = 0.0f;

	ret.m_afEntry[12] = 0.0f;
	ret.m_afEntry[13] = 0.0f;
	ret.m_afEntry[14] = zn / ( zn-zf );
	ret.m_afEntry[15] = 1.0f;

	return( ret );
}
//----------------------------------------------------------------------------------------------------
float Matrix4f::operator[] ( int iPos ) const
{
    return( m_afEntry[iPos] );
}
//----------------------------------------------------------------------------------------------------
float& Matrix4f::operator[] ( int iPos )
{
    return( m_afEntry[iPos] );
}
//----------------------------------------------------------------------------------------------------
float Matrix4f::operator() ( int iRow, int iCol ) const
{
    return( m_afEntry[I(iRow,iCol)] );
}
//----------------------------------------------------------------------------------------------------
float& Matrix4f::operator() ( int iRow, int iCol )
{
    return( m_afEntry[I(iRow,iCol)] );
}
//----------------------------------------------------------------------------------------------------
bool Matrix4f::operator== ( const Matrix4f& Matrix ) const
{
    return( memcmp( m_afEntry, Matrix.m_afEntry, 4*4*sizeof(float) ) == 0 );
}
//----------------------------------------------------------------------------------------------------
bool Matrix4f::operator!= ( const Matrix4f& Matrix ) const
{
    return( memcmp( m_afEntry, Matrix.m_afEntry, 4*4*sizeof(float) ) != 0 );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator* ( const Matrix4f& Matrix ) const
{
	Matrix4f mProd;

	for (int iRow = 0; iRow < 4; iRow++)
	{
		for (int iCol = 0; iCol < 4; iCol++)
		{
			int i = I(iRow, iCol);
			mProd.m_afEntry[i] = 0.0f;
			for (int iMid = 0; iMid < 4; iMid++)
			{
				mProd.m_afEntry[i] += 
					m_afEntry[I(iRow, iMid)] * Matrix.m_afEntry[I(iMid, iCol)];
			}
		}
	}
	return( mProd );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator+ ( const Matrix4f& Matrix ) const
{
	Matrix4f mSum;

	for (int i = 0; i < 4*4; i++)
		mSum.m_afEntry[i] = m_afEntry[i] + Matrix.m_afEntry[i];

	return( mSum );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator- ( const Matrix4f& Matrix ) const
{
	Matrix4f mDiff;

	for (int i = 0; i < 4*4; i++)
		mDiff.m_afEntry[i] = m_afEntry[i] - Matrix.m_afEntry[i];

	return( mDiff );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator* ( float fScalar ) const
{
	Matrix4f mProd;

	for (int i = 0; i < 4*4; i++)
		mProd.m_afEntry[i] = m_afEntry[i] * fScalar;

	return( mProd );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator/ ( float fScalar ) const
{
	Matrix4f mQuot;
	if (fScalar != 0.0f)
	{
		float fInvScalar = 1.0f / fScalar;
		for (int i = 0; i < 4*4; i++)
			mQuot.m_afEntry[i] = m_afEntry[i] * fInvScalar;
	}
	else
	{
		for (int i = 0; i < 4*4; i++)
			mQuot.m_afEntry[i] = 0;
	}

	return( mQuot );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::operator- () const
{
	Matrix4f mNeg;

	for ( int i = 0; i < 4*4; i++ )
		mNeg.m_afEntry[i] = -m_afEntry[i];

	return( mNeg );
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator+= (const Matrix4f& Matrix)
{
	for ( int i = 0; i < 4*4; i++ )
		m_afEntry[i] += Matrix.m_afEntry[i];

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator-= ( const Matrix4f& Matrix )
{
	for ( int i = 0; i < 4*4; i++ )
		m_afEntry[i] -= Matrix.m_afEntry[i];

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator*= ( float fScalar )
{
	for ( int i = 0; i < 4*4; i++ )
		m_afEntry[i] *= fScalar;

	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator*= ( const Matrix4f& Matrix )
{
	Matrix4f mProd = *this; 
	
	for ( int iRow = 0; iRow < 4; iRow++ )
	{
		for ( int iCol = 0; iCol < 4; iCol++ )
		{
			int i = I(iRow, iCol);
			m_afEntry[i] = 0.0f;
			for ( int iMid = 0; iMid < 4; iMid++ )
			{
				m_afEntry[i] += 
					mProd.m_afEntry[I(iRow, iMid)] * Matrix.m_afEntry[I(iMid, iCol)];
			}
		}
	}
	return( *this );
}
//----------------------------------------------------------------------------------------------------
Matrix4f& Matrix4f::operator/= ( float fScalar )
{
	if ( fScalar != 0.0f )
	{
		float fInvScalar = 1.0f / fScalar;	
		for (int i = 0; i < 4*4; i++)
			m_afEntry[i] *= fInvScalar;
	}
	else
	{
		for ( int i = 0; i < 4*4; i++ )
			m_afEntry[i] = 0;
	}

	return( *this );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::MakeZero()
{
	memset( m_afEntry,0,4*4*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::MakeIdentity()
{
	for ( int iRow = 0; iRow < 4; iRow++ ) 
	{
		for ( int iCol = 0; iCol < 4; iCol++ )
		{
			if ( iRow == iCol )
                m_afEntry[I(iRow,iCol)] = 1.0f;
			else
				m_afEntry[I(iRow,iCol)] = 0.0f;
		}
	}
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::MakeTranspose()
{
	Matrix4f mTranspose;

	for ( int iRow = 0; iRow < 4; iRow++ )
	{
		for ( int iCol = 0; iCol < 4; iCol++ )
			mTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
	}
    
	memcpy( m_afEntry, mTranspose.m_afEntry, 4*4*sizeof(float) );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::Zero()
{
	Matrix4f mReturn = Matrix4f( true );

	return( mReturn );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::Identity()
{
	Matrix4f mIdent;

	for ( int iRow = 0; iRow < 4; iRow++ )
	{
		for ( int iCol = 0; iCol < 4; iCol++ )
		{
			if (iRow == iCol)
                mIdent.m_afEntry[I(iRow,iCol)] = 1.0f;
			else
				mIdent.m_afEntry[I(iRow,iCol)] = 0.0f;
		}
	}

	return( mIdent );
}
//----------------------------------------------------------------------------------------------------
Matrix4f Matrix4f::Transpose()
{
	Matrix4f mTranspose;

	for ( int iRow = 0; iRow < 4; iRow++ )
	{
		for ( int iCol = 0; iCol < 4; iCol++ )
			mTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
	}

	return( mTranspose );

}
//----------------------------------------------------------------------------------------------------
int Matrix4f::I( int iRow, int iCol )
{
    return( 4*iRow + iCol );
}
//----------------------------------------------------------------------------------------------------
Vector4f Matrix4f::operator* ( const Vector4f& Vector ) const
{
    Vector4f vProd;
    for ( int iCol = 0; iCol < 4; iCol++ )
    {
        vProd[iCol] = 0.0f;
        for ( int iRow = 0; iRow < 4; iRow++ )
            vProd[iCol] += m_afEntry[I(iRow,iCol)] * Vector[iRow];
    }
    return( vProd );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::SetRow( int iRow, const Vector4f& Vector )
{
	for ( int iCol = 0; iCol < 4; iCol++ )
        m_afEntry[I(iRow,iCol)] = Vector[iCol];
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::SetRow( int iRow, const Vector3f& Vector )
{
    for ( int iCol = 0; iCol < 3; iCol++ )
        m_afEntry[I(iRow,iCol)] = Vector[iCol];
}
//----------------------------------------------------------------------------------------------------
Vector4f Matrix4f::GetRow( int iRow ) const
{
	Vector4f vRow;
	for ( int iCol = 0; iCol < 4; iCol++ )
        vRow[iCol] = m_afEntry[I(iRow,iCol)];

	return( vRow );
}
//----------------------------------------------------------------------------------------------------
void Matrix4f::SetColumn( int iCol, const Vector4f& Vector )
{
	for ( int iRow = 0; iRow < 4; iRow++ )
        m_afEntry[I(iRow,iCol)] = Vector[iRow];
}
//----------------------------------------------------------------------------------------------------
Vector4f Matrix4f::GetColumn( int iCol ) const
{
	Vector4f vCol;
	for ( int iRow = 0; iRow < 4; iRow++ )
		vCol[iRow] = m_afEntry[I(iRow,iCol)];

	return( vCol );
}
//----------------------------------------------------------------------------------------------------
