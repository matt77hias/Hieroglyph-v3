//--------------------------------------------------------------------------------
// FinalVS.hlsl
//
// This vertex shader outputs the standard information for rendering a scene.
// No special calculations are needed to support ambient occlusion except for
// the screen space positions.
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
	float3 normal : NORMAL;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	//output.colour.xyz = float4( 1.0, 1.0, 1.0, 1.0 ); //v.colour;

	float3 ViewSpaceNormals = mul( float4( input.normal, 0.0f ), WorldViewMatrix ).xyz;
	output.normal = ViewSpaceNormals * 0.5f + 0.5f;

	return output;
}

