//--------------------------------------------------------------------------------
// FullscreenTwoTextures.hlsl
//
// Copyright (C) Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D       ColorTexture1 : register( t0 );
Texture2D		ColorTexture2 : register( t1 );
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
	float exposure = 1.0;

	float4 color1 = ColorTexture1.Sample( LinearSampler, input.tex );
	float4 color2 = ColorTexture2.Sample( LinearSampler, input.tex );
	float3 hdr = color1.xyz + color2.xyz;

	float3 tone_mapped = float3( 1.0, 1.0, 1.0 ) - exp(-hdr * exposure);

	return( float4( tone_mapped, 1.0 ) );
}

