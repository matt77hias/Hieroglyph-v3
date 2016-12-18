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
// Stage:				Pixel Shader
//--------------------------------------------------------------------------------
Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );
//--------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
};
//--------------------------------------------------------------------------------
// UE4 PBR Formulation
//--------------------------------------------------------------------------------
float G1( float3 n, float3 v, float k )
{
	float NdotV = dot( n, v );
	return NdotV / ( NdotV * (1.0f-k) + k );
}

float4 PSMAIN( in PS_INPUT input ) : SV_Target
{
	float4 albedo = ColorTexture.Sample( LinearSampler, input.tex );

	return albedo;
}
//--------------------------------------------------------------------------------