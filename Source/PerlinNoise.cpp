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
#include "PCH.h"
#include "PerlinNoise.h"
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

	srand(0x01);

	for (i = 0; i < base; i++)
	{
		permutation[i] = i;

		g1[i] = (float)((rand() % (base+base))-base)/base;
		
		for (j = 0; j<2; j++)
			g2[i][j] = (float)((rand() % (base+base))-base)/base;
		normalize2(g2[i]);

		for (j = 0; j<3; j++)
			g3[i][j] = (float)((rand() % (base+base))-base)/base;
		normalize3(g3[i]);
	}

	while(--i)
	{
		k = permutation[i];
		permutation[i] = permutation[j = rand() % base];
		permutation[j] = k;
	}

	for (i=0; i < base + 2; i++)
	{
		permutation[base+i] = permutation[i];

		g1[base+i] = g1[i];

		for (j = 0; j<2; j++)
			g2[base+i][j] = g2[i][j];

		for (j = 0; j<3; j++)
			g3[base+i][j] = g3[i][j];
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
	int x0, x1;
	float rx0, rx1, sx, u, v;

	// Find the enclosing basis points around our input value
	x0 = ((int)x) % base;
	x1 = (x0+1) % base;

	// Calculate the distance to each basis point
	rx0 = x - (int)x;
	rx1 = rx0 - 1.0f;

	// Get a smoothed interpolation input
	sx = curve(rx0);

	// Perform dot product with the gradient vectors to find our two input
	// values for the interpolation
	u = rx0 * g1[ permutation[ x0 ] ];
	v = rx1 * g1[ permutation[ x1 ] ];

	// Return the interpolated value
	return( lerp(sx, u, v) );
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise2( float x, float y )
{
	int x0, x1, y0, y1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, u, v;
	register int i, j;

	// Find the enclosing basis points around our input x value
	x0 = ((int)x) & baseMask;
	x1 = (x0+1) & baseMask;

	// Calculate the distance to each basis point along x
	rx0 = x - (int)x;
	rx1 = rx0 - 1.0f;


	// Find the enclosing basis points around our input y value
	y0 = ((int)y) & baseMask;
	y1 = (y0+1) & baseMask;

	// Calculate the distance to each basis point along y
	ry0 = y - (int)y;
	ry1 = ry0 - 1.0f;


	// Permute the gradient indices using one permutation for
	// each input axis.
	i = permutation[x0];
	j = permutation[x1];

	b00 = permutation[ i + y0 ];
	b10 = permutation[ j + y0 ];
	b01 = permutation[ i + y1 ];
	b11 = permutation[ j + y1 ];

	// Get a smoothed interpolation input
	sx = curve(rx0);
	sy = curve(ry0);

	// Perform dot product with the gradient vectors to find our two input
	// values for the interpolation
	q = g2[b00]; u = rx0*q[0] + ry0*q[1];
	q = g2[b10]; v = rx1*q[0] + ry0*q[1];
	a = lerp(sx, u, v);

	q = g2[b01]; u = rx0*q[0] + ry1*q[1];
	q = g2[b11]; v = rx1*q[0] + ry1*q[1];
	b = lerp(sx, u, v);

	// Return the interpolated value
	return(lerp(sy, a, b));
}
//--------------------------------------------------------------------------------
float PerlinNoise::noise3( float x, float y, float z )
{
	int x0, x1, y0, y1, z0, z1;
	int b000, b001, b010, b011, b100, b101, b110, b111;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sx, sy, sz, a, b, c, d, u, v;

	// Find the enclosing basis points around our input x value
	x0 = ((int)x) & baseMask;
	x1 = (x0+1) & baseMask;

	// Calculate the distance to each basis point along x
	rx0 = x - (int)x;
	rx1 = rx0 - 1.0f;


	// Find the enclosing basis points around our input y value
	y0 = ((int)y) & baseMask;
	y1 = (y0+1) & baseMask;

	// Calculate the distance to each basis point along y
	ry0 = y - (int)y;
	ry1 = ry0 - 1.0f;


	// Find the enclosing basis points around our input z value
	z0 = ((int)z) & baseMask;
	z1 = (z0+1) & baseMask;

	// Calculate the distance to each basis point along z
	rz0 = z - (int)z;
	rz1 = rz0 - 1.0f;


	// Permute the gradient indices using one permutation for
	// each input axis.

	b000 = permutation[ permutation[ permutation[x0] + y0 ] + z0 ];
	b010 = permutation[ permutation[ permutation[x0] + y1 ] + z0 ];
	b001 = permutation[ permutation[ permutation[x0] + y0 ] + z1 ];
	b011 = permutation[ permutation[ permutation[x0] + y1 ] + z1 ];
	b100 = permutation[ permutation[ permutation[x1] + y0 ] + z0 ];
	b110 = permutation[ permutation[ permutation[x1] + y1 ] + z0 ];
	b101 = permutation[ permutation[ permutation[x1] + y0 ] + z1 ];
	b111 = permutation[ permutation[ permutation[x1] + y1 ] + z1 ];

	// Get a smoothed interpolation input
	sx = curve(rx0);
	sy = curve(ry0);
	sz = curve(rz0);

	// Perform dot product with the gradient vectors to find our two input
	// values for the interpolation
	q = g2[b000]; u = rx0*q[0] + ry0*q[1] + rz0*q[2];
	q = g2[b100]; v = rx1*q[0] + ry0*q[1] + rz0*q[2];
	a = lerp(sx, u, v);

	q = g2[b010]; u = rx0*q[0] + ry1*q[1] + rz0*q[2];
	q = g2[b110]; v = rx1*q[0] + ry1*q[1] + rz0*q[2];
	b = lerp(sx, u, v);

	q = g2[b001]; u = rx0*q[0] + ry0*q[1] + rz1*q[2];
	q = g2[b101]; v = rx1*q[0] + ry0*q[1] + rz1*q[2];
	c = lerp(sx, u, v);

	q = g2[b011]; u = rx0*q[0] + ry1*q[1] + rz1*q[2];
	q = g2[b111]; v = rx1*q[0] + ry1*q[1] + rz1*q[2];
	d = lerp(sx, u, v);

	// Return the interpolated value
	return( lerp( sz, lerp( sy, a, b ), lerp( sy, c, d ) ) );
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