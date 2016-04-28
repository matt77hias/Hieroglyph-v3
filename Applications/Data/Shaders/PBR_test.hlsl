//--------------------------------------------------------------------------------
// ImmediateGeometrySolid.hlsl
//
//
// Copyright (C) 2012 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

cbuffer WorldTransforms
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;	
};

cbuffer ObjectMaterialInfo
{
	//float4 Ka;
	//float4 Kd;
	float4 Ks;
};

cbuffer PointLightInfo
{
	float4 LightPosition;
	float4 Ia;
	float4 Id;
	float4 Is;
};

cbuffer SceneInfo
{
	float4 ViewPosition;
};




struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Lighting calculations done in world space
	float4 P = mul( float4( input.position, 1.0f ), WorldMatrix );
	float3 N = mul( input.normal, (float3x3)WorldMatrix );

	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.worldPos = P;	
	output.normal = N;
	output.color = input.color;
	
	return output;
}


//float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
//{
//	float3 P = input.worldPos.xyz;// / input.worldPos.w;
//	float3 N = normalize( input.normal );
//	float3 L = normalize( LightPosition.xyz - P.xyz );
//	float3 V = normalize( ViewPosition.xyz - P.xyz );
//	float3 H = normalize( L + V );
//
//	float NdotL = dot( L, N );
//	float4 ambientIntensity = input.color * Ia;
//	float4 diffuseIntensity = input.color * Id * clamp( NdotL, 0.0f, 1.0f );
//	float4 specularIntensity = Ks * Is * clamp( pow( abs( dot( H, N ) ), 64.0f ), 0.0f, 1.0f );
//
//	float4 output = ambientIntensity;
//
//	if ( NdotL > 0.0f ) {
//		output += diffuseIntensity;
//		output += specularIntensity;
//	}
//
//	output.a = 1.0f;
//
//
//	return( output );
//}


// UE4 PBR Formulation

float G1( float3 n, float3 v, float k )
{
	float NdotV = dot( n, v );
	return NdotV / ( NdotV * (1.0f-k) + k );
}

float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
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

	float3 F0 = input.color.rgb;

	float roughness = 0.5f;
	float alpha = pow( roughness, 2.0f );
	float alphaSquared = pow( alpha, 2.0f );
	float k = pow( roughness + 1.0f, 2.0f ) / 8.0f;

	float3 F = F0 + (1.0-F0) * pow(2, (-5.55473*VdotH -6.98316)*VdotH );
	float G = G1(L,H,k) * G1(V,H,k);
	float D = alphaSquared / ( 3.14f * pow((pow(NdotH,2.0f) * (alphaSquared-1.0f) + 1.0f ),2.0f) );

	float3 Fr = (F*G*D) / (4.0f*NdotL*NdotV);
	
	float3 Fd = input.color.rgb / 3.14;

	float3 output = (Fr + Fd) * (Id * NdotL);

	return float4( output, 1.0f );
}




/*
float3 F_Schlick (in float3 f0 , in float f90 , in float u)
{
	return f0 + (f90 - f0) * pow (1.0f - u, 5.0f);
}

float V_SmithGGXCorrelated ( float NdotL , float NdotV , float alphaG )
{
	// Original formulation of G_SmithGGX Correlated
	// lambda_v = (-1 + sqrt ( alphaG2 * (1 - NdotL2 ) / NdotL2 + 1)) * 0.5 f;
	// lambda_l = (-1 + sqrt ( alphaG2 * (1 - NdotV2 ) / NdotV2 + 1)) * 0.5 f;
	// G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l );
	// V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV );

	// This is the optimize version
	float alphaG2 = alphaG * alphaG ;
	// Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
	float Lambda_GGXV = NdotL * sqrt ((- NdotV * alphaG2 + NdotV ) * NdotV + alphaG2 );
	float Lambda_GGXL = NdotV * sqrt ((- NdotL * alphaG2 + NdotL ) * NdotL + alphaG2 );

	return 0.5f / ( Lambda_GGXV + Lambda_GGXL );
}

float D_GGX ( float NdotH , float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = ( NdotH * m2 - NdotH ) * NdotH + 1;
	return m2 / (f * f);
}

float Fr_DisneyDiffuse ( float NdotV , float NdotL , float LdotH , float linearRoughness )
{
	float energyBias = lerp (0.0f, 0.5f, linearRoughness );
	float energyFactor = lerp (1.0f, 1.0f / 1.51f, linearRoughness );
	float fd90 = energyBias + 2.0f * LdotH * LdotH * linearRoughness ;
	float3 f0 = float3 (1.0f, 1.0f, 1.0f);
	float lightScatter = F_Schlick (f0, fd90, NdotL ).r;
	float viewScatter = F_Schlick (f0, fd90, NdotV ).r;

	return lightScatter * viewScatter * energyFactor ;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float3 f0 = input.color.rgb;
	float f90 = 1.0;
	float roughness = 0.5f;
	float linearRoughness = roughness;

	float3 P = input.worldPos.xyz;// / input.worldPos.w;
	float3 N = normalize( input.normal );
	float3 L = normalize( LightPosition.xyz - P.xyz );
	float3 V = normalize( ViewPosition.xyz - P.xyz );
	//float3 H = normalize( L + V );

	float PI = 3.14f;



	// This code is an example of call of previous functions
	float NdotV = abs( dot (N, V)) + 1e-5f; // avoid artifact
	float3 H = normalize (V + L);
	float LdotH = saturate ( dot (L, H));
	float NdotH = saturate ( dot (N, H));
	float NdotL = saturate ( dot (N, L));

	// Specular BRDF
	float3 F = F_Schlick (f0 , f90 , LdotH );
	float Vis = V_SmithGGXCorrelated (NdotV , NdotL , roughness );
	float D = D_GGX (NdotH , roughness );
	float Fr = D * F * Vis / PI;

	// Diffuse BRDF
	float Fd = Fr_DisneyDiffuse (NdotV , NdotL , LdotH , linearRoughness ) / PI;

	return Fr+Fd;
}
*/