//--------------------------------------------------------------------------------
// GBuffer
//
// Vertex shader and pixel shader for filling the G-Buffer of a classic
// deferred renderer
//--------------------------------------------------------------------------------

cbuffer Transforms
{
	matrix WorldMatrix;
	matrix WorldViewMatrix;
	matrix WorldViewProjMatrix;
};

struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORDS0;
	float3 Normal	: NORMAL;
	float4 Tangent	: TANGENT;
};

struct VSOutput
{
	float4 PositionCS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 PositionWS	: POSITIONWS;
	float3 TangentWS	: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
};

struct VSOutputOptimized
{
	float4 PositionCS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
	float3 NormalVS		: NORMALVS;
	float3 TangentVS	: TANGENTVS;
	float3 BitangentVS	: BITANGENTVS;
};

struct PSInput
{
	float4 PositionSS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
	float3 NormalWS		: NORMALWS;
	float3 PositionWS	: POSITIONWS;
	float3 TangentWS	: TANGENTWS;
	float3 BitangentWS	: BITANGENTWS;
};

struct PSOutput
{
	float4 Normal			: SV_Target0;
	float4 DiffuseAlbedo 	: SV_Target1;
	float4 SpecularAlbedo 	: SV_Target2;
	float4 Position			: SV_Target3;
};

struct PSInputOptimized
{
	float4 PositionSS 			: SV_Position;
	float2 TexCoord				: TEXCOORD;
	float3 NormalVS				: NORMALVS;
	float3 TangentVS			: TANGENTVS;
	float3 BitangentVS			: BITANGENTVS;
};

struct PSOutputOptimized
{
	float4 Normal			: SV_Target0;
	float4 DiffuseAlbedo 	: SV_Target1;
	float4 SpecularAlbedo 	: SV_Target2;
};

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2D       DiffuseMap : register( t0 );
Texture2D		NormalMap : register( t1 );

SamplerState    AnisoSampler : register( s0 );

//-------------------------------------------------------------------------------------------------
// Basic vertex shader, no optimizations
//-------------------------------------------------------------------------------------------------
VSOutput VSMain( in VSInput input )
{
	VSOutput output;

	// Convert position and normals to world space
	output.PositionWS = mul( input.Position, WorldMatrix ).xyz;
	float3 normalWS = normalize( mul( input.Normal, (float3x3)WorldMatrix ) );
	output.NormalWS = normalWS;

	// Reconstruct the rest of the tangent frame
	float3 tangentWS = normalize( mul( input.Tangent.xyz, (float3x3)WorldMatrix ) );
	float3 bitangentWS = normalize( cross( normalWS, tangentWS ) ) * input.Tangent.w;

	output.TangentWS = tangentWS;
	output.BitangentWS = bitangentWS;

	// Calculate the clip-space position
	output.PositionCS = mul( input.Position, WorldViewProjMatrix );

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}

//-------------------------------------------------------------------------------------------------
// Vertex shader, with optimizations
//-------------------------------------------------------------------------------------------------
VSOutputOptimized VSMainOptimized( in VSInput input )
{
	VSOutputOptimized output;

	// Convert normals to view space
	float3 normalVS = normalize( mul( input.Normal, (float3x3)WorldViewMatrix ) );
	output.NormalVS = normalVS;

	// Reconstruct the rest of the tangent frame
	float3 tangentVS = normalize( mul( input.Tangent.xyz, (float3x3)WorldViewMatrix ) );
	float3 bitangentVS = normalize( cross( normalVS, tangentVS ) ) * input.Tangent.w;

	output.TangentVS = tangentVS;
	output.BitangentVS = bitangentVS;

	// Calculate the clip-space position
	output.PositionCS = mul( input.Position, WorldViewProjMatrix );

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}

//-------------------------------------------------------------------------------------------------
// Basic pixel shader, no optimizations
//-------------------------------------------------------------------------------------------------
PSOutput PSMain( in PSInput input )
{
	PSOutput output;

	// Sample the diffuse map
	float3 diffuseAlbedo = DiffuseMap.Sample( AnisoSampler, input.TexCoord ).rgb;

	// Normalize the tangent frame after interpolation
	float3x3 tangentFrameWS = float3x3( normalize( input.TangentWS ),
										normalize( input.BitangentWS ),
										normalize( input.NormalWS ) );

	// Sample the tangent-space normal map and decompress
	float3 normalTS = normalize( NormalMap.Sample( AnisoSampler, input.TexCoord ).rgb * 2.0f - 1.0f );

	// Convert to world space
	float3 normalWS = mul( normalTS, tangentFrameWS );

	// Output our G-Buffer values
	output.Normal = float4( normalWS, 1.0f );
	output.DiffuseAlbedo = float4( diffuseAlbedo, 1.0f );
	output.SpecularAlbedo = float4( 0.7f, 0.7f, 0.7f, 64.0f );
	output.Position = float4( input.PositionWS, 1.0f );

	return output;
}

//-------------------------------------------------------------------------------------------------
// Function for encoding normals using a spheremap transform
//-------------------------------------------------------------------------------------------------
float2 SpheremapEncode( float3 normalVS )
{
    return normalize( normalVS.xy ) * ( sqrt( -normalVS.z * 0.5f + 0.5f ) );
}

//-------------------------------------------------------------------------------------------------
// Pixel shader, with optimizations
//-------------------------------------------------------------------------------------------------
PSOutputOptimized PSMainOptimized( in PSInputOptimized input )
{
	PSOutputOptimized output;

	// Sample the diffuse map
	float3 diffuseAlbedo = DiffuseMap.Sample( AnisoSampler, input.TexCoord ).rgb;

	// Normalize the tangent frame after interpolation
	float3x3 tangentFrameVS = float3x3( normalize( input.TangentVS ),
										normalize( input.BitangentVS ),
										normalize( input.NormalVS ) );

	// Sample the tangent-space normal map and decompress
	float3 normalTS = normalize( NormalMap.Sample( AnisoSampler, input.TexCoord ).rgb * 2.0f - 1.0f );

	// Convert to view space
	float3 normalVS = mul( normalTS, tangentFrameVS );

	// Output our G-Buffer values
	output.Normal = float4( SpheremapEncode( normalVS ), 1.0f, 1.0f );
	output.DiffuseAlbedo = float4( diffuseAlbedo, 1.0f );
	output.SpecularAlbedo = float4( 0.7f, 0.7f, 0.7f, 64.0f / 255.0f );

	return output;
}