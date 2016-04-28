//--------------------------------------------------------------------------------
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );

cbuffer TransformMatrices
{
	matrix WorldViewProjMatrix;	
};

cbuffer ObjectProperties
{
	float4 objectColor;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color    : COLOR;
	float2 tex		: TEXCOORD0;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	
	output.color = objectColor;
	output.tex = input.tex;

	return output;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 sampledColor = ColorTexture.Sample( LinearSampler, input.tex );
	float4 mixedColor = sampledColor * input.color;

	clip( mixedColor.a - 0.01f );
	
	return( mixedColor );
}

