//--------------------------------------------------------------------------------
// FinalVS.hlsl
//
// This vertex shader outputs the standard information for rendering a scene.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------


cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
	matrix WorldViewMatrix;
};


struct VS_INPUT
{
	float3 position 	: POSITION;
};

struct VS_OUTPUT
{
	float4 position 	: SV_Position;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );

	return output;
}

