/***
*
*	Copyright (c) 1998, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

#ifndef __MATHLIB__
#define __MATHLIB__

#include "PCH.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOUBLEVEC_T
typedef double vec_t;
#else
typedef float vec_t;
#endif
typedef vec_t vec3_t[3];	// x,y,z
typedef vec_t vec4_t[4];	// x,y,z,w

#define	Q_PI	3.14159265358979323846

#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])

void ClearBounds (vec3_t mins, vec3_t maxs);
void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs);

void AngleMatrix (const vec3_t angles, float matrix[3][4] );
void R_ConcatTransforms (const float in1[3][4], const float in2[3][4], float out[3][4]);

void VectorIRotate (const vec3_t in1, const float in2[3][4], vec3_t out);
void VectorRotate (const vec3_t in1, const float in2[3][4], vec3_t out);

void VectorTransform (const vec3_t in1, const float in2[3][4], vec3_t out);
void VectorITransform (const vec3_t in1, const float in2[3][4], vec3_t out);

void AngleQuaternion( const vec3_t angles, vec4_t quaternion );
void QuaternionMatrix( const vec4_t quaternion, float (*matrix)[4] );
void QuaternionSlerp( const vec4_t p, vec4_t q, float t, vec4_t qt );

#ifdef __cplusplus
}
#endif

#endif
