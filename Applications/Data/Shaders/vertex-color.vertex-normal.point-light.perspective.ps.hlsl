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
// Lighting:			Point Light
// Purpose:				Perspective rendering
// Stage:				Pixel Shader
//--------------------------------------------------------------------------------
cbuffer PointLightInfo
{
	float4 LightPosition;
	float4 Ia;
	float4 Id;
	float4 Is;
};
//--------------------------------------------------------------------------------
cbuffer SceneInfo
{
	float4 ViewPosition;
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
struct PS_INPUT
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
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
	float3 P = input.worldPos.xyz;
	float3 N = normalize( input.normal );
	float3 L = normalize( LightPosition.xyz - P.xyz );
	float3 V = normalize( ViewPosition.xyz - P.xyz );
	float3 H = normalize( L + V );

	float NdotL = dot( L, N );
	float NdotV = dot( N, V );
	float NdotH = dot( N, H );

	float VdotH = dot( V, H );
	float LdotH = dot( L, H );

	float3 F0 = input.color.rgb * object_material.y;

	float roughness = object_material.x;
	float alpha = pow( roughness, 2.0f );
	float alphaSquared = pow( alpha, 2.0f );
	float k = pow( roughness + 1.0f, 2.0f ) / 8.0f;

	float3 F = F0 + (1.0-F0) * pow(1.0-LdotH, 5.0);//pow(2, (-5.55473*VdotH -6.98316)*VdotH );
	float G = G1(L,H,k) * G1(V,H,k);
	float D = alphaSquared / ( 3.14f * pow((pow(NdotH,2.0f) * (alphaSquared-1.0f) + 1.0f ),2.0f) );

	float3 Fr = (F*G*D) / (4.0f*NdotL*NdotV);
	
	float3 Fd = (input.color.rgb / 3.14) * (1.0 - object_material.y);

	//float3 output = (Fr + Fd) * (Id * NdotL);
	float3 output = (Fd + Fr) * (Id * NdotL);

	return float4( output, input.color.a );
	//float GD = G*D;
	//return float4( GD, GD, GD, 1.0f );
	//return float4(F,1.0f);
}
//--------------------------------------------------------------------------------