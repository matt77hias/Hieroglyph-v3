//--------------------------------------------------------------------------------
#include "PCH.h"
#include "BezierCubic.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BezierCubic::BezierCubic()
{
}
//--------------------------------------------------------------------------------
BezierCubic::~BezierCubic()
{

}
//--------------------------------------------------------------------------------
Vector3f BezierCubic::sample( float t ) const
{
	// This is the basic Bezier curve sampling equation as taken from Eric 
	// Lengyel's book "Mathematics for 3D Game Programming", 2nd Edition.

	Vector3f result = 
		points[0] * pow((1.0f-t), 3.0f)  +
		points[1] * 3.0f*t * pow( (1.0f-t), 2.0f ) +
		points[2] * 3.0f*t*t * (1.0f-t) +
		points[3] * t*t*t;

	return result;
}
//--------------------------------------------------------------------------------
Vector3f BezierCubic::sample_normal( float u ) const
{
	// First find the tangent in the forward direction, then find a perpendicular
	// vector to it.  This returns one normal out of the infinite number of
	// possible solutions, but not in a particular direction.
	
	Vector3f tangent = sample_tangent( u );
	Vector3f normal = Vector3f::Perpendicular( tangent );

	return( normal );
}
//--------------------------------------------------------------------------------
Vector3f BezierCubic::sample_tangent( float u ) const
{
	// Calculate the tangent with finite differences by sampling a small distance
	// forward and backward from the requested point, then normalizing the vector
	// between those two points.

	const float epsilon = 0.001f;

	Vector3f s1 = sample( u - epsilon );
	Vector3f s2 = sample( u + epsilon );

	Vector3f tangent = Vector3f::Normalize( Vector3f( s2 - s1 ) );

	return( tangent );
}
//--------------------------------------------------------------------------------