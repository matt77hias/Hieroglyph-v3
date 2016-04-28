//--------------------------------------------------------------------------------
// PhongShading.hlsl
//
// This set of shaders implements the most basic phong shading.
//
// Copyright (C) 2010 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Resources
//--------------------------------------------------------------------------------
cbuffer Transforms
{
	matrix WorldMatrix;	
	matrix ViewProjMatrix;
	matrix SkinMatrices[26];
};

cbuffer LightParameters
{
	float3 LightPositionWS;
	float4 LightColor;
};



//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	position 		: POSITION;
	int		bone			: BONEIDS;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 position			: SV_Position;
	float4 color			: COLOR;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	//output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.position = mul( float4( input.position, 1.0f ), SkinMatrices[input.bone] );
	//output.position = mul( output.position, WorldMatrix );
	output.position = mul( output.position, ViewProjMatrix );
	output.color.rgb = LightColor.rgb;
	output.color.a = 1.0f;

	return output;
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 color = input.color;

	return( color );
}
//--------------------------------------------------------------------------------
