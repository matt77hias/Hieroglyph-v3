//--------------------------------------------------------------------------------
// TextureVS.hlsl
//
// This vertex shader serves as a pass through for pre-transformed vertices.  This
// is typically used for full screen quad rendering.
//
// Copyright (C) 2010 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = input.position;

	return output;
}

