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
// PerlinNoise 
//
//--------------------------------------------------------------------------------
#ifndef PerlinNoise_h
#define PerlinNoise_h
//--------------------------------------------------------------------------------
#include "Vector2f.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PerlinNoise
	{
	public:
		PerlinNoise();
		~PerlinNoise();	

		void initialize();
		float noise ( float x );
		float noise2( float x, float y );
		float noise3( float x, float y, float z );

		float noise2( float x, float y, int octaves );

	private:
		float curve(float t);
		float lerp(float t, float a, float b);
		void normalize2(float v[2]);
		void normalize3(float v[3]);

		#define base 0x100
		#define baseMask 0xff

		int permutation[base + base + 2];
		float g1[base + base + 2];
		float g2[base + base + 2][2];
		float g3[base + base + 2][3];

		//Vector3f g3[base+base+2];
	};
};
//--------------------------------------------------------------------------------
#endif // PerlinNoise_h
