//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Albedo:				Textured
// Geometric Features:	Vertex Normals
// Lighting:			Point Light
// Purpose:				Perspective rendering
// Stage:				Vertex Shader
//--------------------------------------------------------------------------------
cbuffer WorldTransforms
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;	
};
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float2 tex		: TEXCOORD0;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float3 normal   : NORMAL;
	float2 tex      : TEXCOORD0;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Lighting calculations done in world space
	float4 P = mul( float4( input.position, 1.0f ), WorldMatrix );
	float3 N = mul( input.normal, (float3x3)WorldMatrix );

	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.worldPos = P;	
	output.normal = N;
	output.tex = input.tex;
	
	return output;
}
//--------------------------------------------------------------------------------