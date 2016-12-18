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
// Albedo:				Vertex Color
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
cbuffer PBRMaterialParameters
{
	float4 object_albedo;
	float4 object_material;
	// x: Roughness
	// y: Metallic
};
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color    : COLOR;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color    : COLOR;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;

	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.color = input.color * object_albedo;
	
	return output;
}
//--------------------------------------------------------------------------------