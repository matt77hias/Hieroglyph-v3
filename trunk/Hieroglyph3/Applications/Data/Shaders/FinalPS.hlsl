//--------------------------------------------------------------------------------
// FinalPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term of a standard lighting equation.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

StructuredBuffer<float> AmbientOcclusionBuffer : register( t0 );           
//StructuredBuffer<float> BilateralOutputBuffer : register( t0 );           
//Texture2D       AmbientOcclusionBuffer : register( t0 );           

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
	int3 screenspace = int3( input.position.x, input.position.y, 0 );
	//int index = (screenspace.y*totalsize_x + screenspace.x)*4;
	int index = (screenspace.y*totalsize_x + screenspace.x);

	float fOcclusion = AmbientOcclusionBuffer[index+0];// *0.5f;
	//float fOcclusion = 1.0f - BilateralOutputBuffer[index+0];
	//float fOcclusion = BilateralOutputBuffer[index+0] * 8.0f + 0.2f;
	//fOcclusion += AmbientOcclusionBuffer[index+1];
	//fOcclusion += AmbientOcclusionBuffer[index+2];
	//fOcclusion += AmbientOcclusionBuffer[index+3];

	//fOcclusion = (1.0f - ( fOcclusion / 0.3009f ));
	//fOcclusion = 2.75*(1.0f - ( fOcclusion ));
	//fOcclusion = 1.0*(0.75f - ( fOcclusion*2.0f ));
	
	float3 Light = normalize( float3( 0.0f, 1.0f, 0.0f ) );
	float diffuse = max( dot( Light, input.normal ), 0.0f );
	float total = 0.4f*fOcclusion + 0.3f*diffuse;
	
	return( float4( fOcclusion, fOcclusion, fOcclusion, fOcclusion ) );
	//return( float4( total, total, total, total ) );
	//return f.colour;
}

