//--------------------------------------------------------------------------------
// ImageViewerPS.hlsl
//
// This pixel shader samples the ambient occlusion buffer and uses its value to 
// modify the ambient term.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D<float4> ColorMap00 : register( t0 );           
SamplerState    LinearSampler : register( s0 );


struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 vValues = ColorMap00.Sample( LinearSampler, input.tex );

	return( vValues );
}

