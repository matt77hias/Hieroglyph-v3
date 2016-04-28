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
template <typename TValue>
TValue Linear( const TValue& start, const TValue& end, float t )
{
	return( start*(1.0f-t) + end*(t) );
}
//--------------------------------------------------------------------------------
template <typename TValue>
TValue QuadraticInOut( const TValue& start, const TValue& end, float t )
{
	float s;

	if ( t < 0.5f ) {
		t *= 2.0f;
		s = t*t*0.5f;
	} else {
		t = (t-1.0f)*2.0f;
		s = (1.0f-(t*t)) * 0.5f + 0.5f;
	}
	return( start*(1.0f-s) + end*(s) );
}
//--------------------------------------------------------------------------------
template <typename TValue>
TValue Hermite( const TValue& start, const TValue& end, float t )
{
	// Perform interpolation between the current frame and the next frame
	// to find the current state.

	Vector3f tangentOut0 = start.m_tangentOut;
	Vector3f tangentIn1 = end.m_tangentIn;
	Vector3f position0 = start.m_spacialData;
	Vector3f position1 = end.m_spacialData;

	float t2 = t*t;
	float t3 = t2*t;

	// calculate Hermite basis
	float h1 =  2.0f * t3 - 3.0f * t2 + 1.0f;
	float h2 = -2.0f * t3 + 3.0f * t2;
	float h3 =         t3 - 2.0f * t2 + t;
	float h4 =         t3 -        t2;

	TValue result;

	result.m_spacialData.x = h1 * position0.x + h3 * tangentOut0.x + h2 * position1.x + h4 * tangentIn1.x;
	result.m_spacialData.y = h1 * position0.y + h3 * tangentOut0.y + h2 * position1.y + h4 * tangentIn1.y;
	result.m_spacialData.z = h1 * position0.z + h3 * tangentOut0.z + h2 * position1.z + h4 * tangentIn1.z;

	return( result );
}
//--------------------------------------------------------------------------------