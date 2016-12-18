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
// Lighting:			Flat
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
	float2 tex		: TEXCOORD0;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;

	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.tex = input.tex;
	
	return output;
}
//--------------------------------------------------------------------------------