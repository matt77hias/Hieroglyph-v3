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
cbuffer SkinningTransforms
{
	matrix WorldMatrix;	
	matrix ViewProjMatrix;
	matrix SkinMatrices[6];
	matrix SkinNormalMatrices[6];
};

cbuffer LightParameters
{
	float3 LightPositionWS;
	float4 LightColor;
};

Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );


//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	position 		: POSITION;
	int4	bone			: BONEIDS;
	float4	weights			: BONEWEIGHTS;
	float3  normal			: NORMAL;
	float2  tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 position			: SV_Position;
	float3 normal			: NORMAL;
	float3 light			: LIGHT;
	float2 tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Calculate the output position of the vertex
	output.position  = (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.x] ) * input.weights.x);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.y] ) * input.weights.y);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.z] ) * input.weights.z);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.w] ) * input.weights.w);

	// Transform world position with viewprojection matrix
	output.position = mul( output.position, ViewProjMatrix );

	// Calculate the world space normal vector
	output.normal  = (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.x] ) * input.weights.x).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.y] ) * input.weights.y).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.z] ) * input.weights.z).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.w] ) * input.weights.w).xyz;

	// Calculate the world space light vector
	output.light = LightPositionWS - output.position.xyz;
	
	// Pass the texture coordinates through
	output.tex = input.tex;

	return output;
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	// Calculate the lighting
	float3 n = normalize( input.normal );
	float3 l = normalize( input.light );


	float4 texColor = ColorTexture.Sample( LinearSampler, input.tex );

	float4 color = texColor * (max(dot(n,l),0) + 0.05f );

	return( color );
}
//--------------------------------------------------------------------------------
