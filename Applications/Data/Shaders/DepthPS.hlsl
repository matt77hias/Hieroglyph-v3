//--------------------------------------------------------------------------------
// DepthPS.hlsl
//
// This pixel shader outputs the linear view space depth to the render target.
// The linear space is used to avoid precision issues on the later calculations.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------


struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 depth	: COLOR;
};

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	return input.depth;
}

