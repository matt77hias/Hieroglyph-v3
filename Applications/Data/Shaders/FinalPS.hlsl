//--------------------------------------------------------------------------------
// FinalPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D<float>       AmbientOcclusionBuffer : register( t0 );           

// The total texture size
#define totalsize_x 640
#define totalsize_y 480

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float fOcclusion = AmbientOcclusionBuffer[ int2( input.position.xy ) ]; //( int3( input.position.xy, 0 ) ).x;
	
	return( float4( fOcclusion, fOcclusion, fOcclusion, fOcclusion ) );
}

