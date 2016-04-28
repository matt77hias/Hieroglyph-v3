//--------------------------------------------------------------------------------
// FinalPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );

struct VS_INPUT
{
	float3 position : POSITION;
	float2 tex      : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = float4( input.position, 1.0f );
	output.tex = input.tex;

	return output;
}

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 vValues = ColorTexture.Sample( LinearSampler, input.tex );

	return( vValues );
}

