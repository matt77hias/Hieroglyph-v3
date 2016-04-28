//--------------------------------------------------------------------------------
// FinalPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D<uint>       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );


struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	int2 location = (int2)(input.tex * float2(320.0f, 240.0f));
	float4 vValues;
	float millis = (float)(ColorTexture[location] );
	vValues.r = millis / 8534.0f;
	vValues.g = 0.0f;//millis / 8534.0f - 0.5f;
	vValues.b = 0.0f;
	vValues.a = 1.0f;
//	float4 vValues = (float)ColorTexture.Sample( LinearSampler, input.tex );

	return( vValues );
}

