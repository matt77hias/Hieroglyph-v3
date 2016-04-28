//--------------------------------------------------------------------------------
// DepthVS
//
// This vertex shader outputs the linear view space depth to the pixel shader.
// The linear space is used to avoid precision issues on the later calculations.
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
	float3 position : POSITION;
	float2 tex		: TEXCOORDS0;
	float3 normal	: NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 depth	: COLOR;
};


VS_OUTPUT VSMAIN( in VS_INPUT v )
{
	VS_OUTPUT output;
	output.position = mul( float4( v.position, 1.0f ), WorldViewProjMatrix );
	
	float3 ViewSpaceNormals = mul( float4( v.normal, 0.0f ), WorldViewMatrix ).xyz;
	output.depth.xyz = ViewSpaceNormals * 0.5f + 0.5f;

	output.depth.w = output.position.w / 25.0f;
		
	return output;
}

