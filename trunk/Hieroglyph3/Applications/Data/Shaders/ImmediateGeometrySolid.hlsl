//--------------------------------------------------------------------------------
// ImmediateGeometrySolid.hlsl
//
//
// Copyright (C) 2012 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color    : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color    : COLOR;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.color = input.color;

	return output;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	return( input.color );
}

