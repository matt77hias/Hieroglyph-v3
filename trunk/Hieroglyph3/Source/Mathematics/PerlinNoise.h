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
// PerlinNoise 
//
//--------------------------------------------------------------------------------
#ifndef PerlinNoise_h
#define PerlinNoise_h
//--------------------------------------------------------------------------------

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

		#define B 0x100
		#define BM 0xff

		#define N 0x1000
		#define NP 12
		#define NM 0xfff

		int p[B + B + 2];
		float g1[B + B + 2];
		float g2[B + B + 2][2];
		float g3[B + B + 2][3];
	};
};
//--------------------------------------------------------------------------------
#endif // PerlinNoise_h
