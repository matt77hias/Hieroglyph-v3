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


struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 vValues = ColorTexture.Sample( LinearSampler, input.tex );

	return( vValues );
}

