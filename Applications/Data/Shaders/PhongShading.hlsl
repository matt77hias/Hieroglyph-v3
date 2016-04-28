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
cbuffer WorldAndClipTransforms
{
	matrix WorldMatrix;	
	matrix WorldViewProjMatrix;
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
	float3 position 		: POSITION;
	float3 normal			: NORMAL;
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
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );

	float3 NormalWS = mul( input.normal, (float3x3)WorldMatrix );
	//float diffuse = dot( normalize( LightPositionWS ), NormalWS );
	float diffuse = dot( normalize( float3( 1.0f, 1.0f, -1.0f ) ), NormalWS );

	//float diffuse = dot( float3( 0.0f, 0.0f, -1.0f ), NormalWS );
	//float diffuse = dot( float3( 0.0f, 0.0f, -1.0f ), float3( 0.0f, 0.0f, -1.0f ) );
	//output.color = LightColor;//float4( 0.5f, 0.5f, 0.5f, 0.5f );
	output.color.rgb = LightColor.rgb * diffuse;
	//output.color.rgb = float3( 0.8f, 0.8f, 0.8f ) * diffuse;
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
