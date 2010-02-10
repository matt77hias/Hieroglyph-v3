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
#include "PerlinNoise.h"
#include <stdlib.h>
#include "math.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PerlinNoise::PerlinNoise()
{
	initialize();
}
//--------------------------------------------------------------------------------
PerlinNoise::~PerlinNoise()
{

}
//--------------------------------------------------------------------------------
void PerlinNoise::initialize()
{
	int i,j,k;

	srand(0xff);

	for (i = 0; i < B; i++)
	{
		p[i] = i;

		g1[i] = (float)((rand() % (B+B))-B)/B;
		
		for (j = 0; j<2; j++)
			g2[i][j] = (float)((rand() % (B+B))-B)/B;
		normalize2(g2[i]);

		for (j = 0; j<3; j++)
			g3[i][j] = (float)((rand() % (B+B))-B)/B;
		normalize3(g3[i]);
	}

	while(--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i=0; i < B + 2; i++)
	{
		p[B+i] = p[i];

		g1[B+i] = g1[i];

		for (j = 0; j<2; j++)
			g2[B+i][j] = g2[i][j];

		for (j = 0; j<3; j++)
			g3[B+i][j] = g3[i][j];
	}
}
//--------------------------------------------------------------------------------
float PerlinNoise::curve(float t)
{
	return( t*t * (3.0f - 2.0f * t));
}
//--------------------------------------------------------------------------------
float PerlinNoise::lerp(float t, float a, float b)
{
	return( a + t * (b-a));
}
//--------------------------------------------------------------------------------
void PerlinNoise::normalize2(float v[2])
{
	float s;
	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0]/s;
	v[1] = v[1]/s;
}
//--------------------------------------------------------------------------------
void PerlinNoise::normalize3(float v[3])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0]/s;
	v[1] = v[1]/s;
	v[2] = v[2]/s;
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise ( float x )
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = x;
	t = vec[0] + N;
	bx0 = ((int)t) & BM;
	bx1 = (bx0+1) & BM;
	rx0 = t - (int)t;
	rx1 = rx0 - 1.0f;

	sx = curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return( lerp(sx, u, v) );
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise2( float x, float y )
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v, vec[2];
	register int i, j;

	vec[0] = x;
	vec[1] = y;

	t = vec[0] + N;
	bx0 = ((int)t) & BM;
	bx1 = (bx0+1) & BM;
	rx0 = t - (int)t;
	rx1 = rx0 - 1.0f;

	t = vec[1] + N;
	by0 = ((int)t) & BM;
	by1 = (by0+1) & BM;
	ry0 = t - (int)t;
	ry1 = ry0 - 1.0f;

	i = p[bx0];
	j = p[bx1];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = curve(rx0);
	sy = curve(ry0);

//#define at2(rx,ry) (rx*q[0] + ry*q[1])

	q = g2[b00]; u = rx0*q[0] + ry0*q[1];
	q = g2[b10]; v = rx1*q[0] + ry0*q[1];
	a = lerp(sx, u, v);

	q = g2[b01]; u = rx0*q[0] + ry1*q[1];
	q = g2[b11]; v = rx1*q[0] + ry1*q[1];
	b = lerp(sx, u, v);

	return(lerp(sy, a, b));
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise3( float x, float y, float z )
{
	return( 0.0f );
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise2( float x, float y, int octaves )
{
	float fScale = float(octaves*16);
	float fAmp = 1.0f;
	float fOutput = 0.0f;

	for (int i = 1; i < octaves + 1; i++)
	{
		fAmp = 1.0f/float(i);
		fScale /= 2.0f;
		fOutput += fAmp * noise2( x/fScale, y/fScale );
	}


	return( fOutput );

	//vec[0] = (float)x/32.0f;
	//vec[1] = (float)y/32.0f;
	//fValue = NoiseMaker.noise2(vec[0],vec[1]);
	//
	//vec[0] = (float)x/16.0f;
	//vec[1] = (float)y/16.0f;
	//fValue += 0.5f*NoiseMaker.noise2(vec[0],vec[1]);

	//vec[0] = (float)x/8.0f;
	//vec[1] = (float)y/8.0f;
	//fValue += 0.25f*NoiseMaker.noise2(vec[0],vec[1]);
	//
	//vec[0] = (float)x/4.0f;
	//vec[1] = (float)y/4.0f;
	//fValue += 0.125f*NoiseMaker.noise2(vec[0],vec[1]);

	//vec[0] = (float)x/2.0f;
	//vec[1] = (float)y/2.0f;
	//fValue += 0.0625f*NoiseMaker.noise2(vec[0],vec[1]);
}
//--------------------------------------------------------------------------------