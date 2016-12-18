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
// Albedo:				Object Color
// Geometric Features:	Vertex Normals
// Lighting:			Flat
// Purpose:				Perspective rendering
// Stage:				Pixel Shader
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color    : COLOR;
};
//--------------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	return input.color;
}
//--------------------------------------------------------------------------------