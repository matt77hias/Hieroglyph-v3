//--------------------------------------------------------------------------------
// GBufferLP
//
// Vertex shader and pixel shader for filling the G-Buffer of a light prepass
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
	float3 NormalVS		: NORMALVS;
	float3 TangentVS	: TANGENTVS;
	float3 BitangentVS	: BITANGENTVS;
};

struct PSInput
{
	centroid float4 PositionSS 	: SV_Position;
	float2 TexCoord				: TEXCOORD;
	float3 NormalVS				: NORMALVS;
	float3 TangentVS			: TANGENTVS;
	float3 BitangentVS			: BITANGENTVS;
};

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2D		NormalMap : register( t0 );
SamplerState    AnisoSampler : register( s0 );

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput VSMain( in VSInput input )
{
	VSOutput output;

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
// Function for encoding normals using a spheremap transform
//-------------------------------------------------------------------------------------------------
float2 SpheremapEncode( float3 normalVS )
{
    return normalize( normalVS.xy ) * ( sqrt( -normalVS.z * 0.5f + 0.5f ) );
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point
//-------------------------------------------------------------------------------------------------
float4 PSMain( in PSInput input ) : SV_Target0
{	
	// Normalize the tangent frame after interpolation
	float3x3 tangentFrameVS = float3x3( normalize( input.TangentVS ),
										normalize( input.BitangentVS ),
										normalize( input.NormalVS ) );

	// Sample the tangent-space normal map and decompress
	float3 normalTS = normalize( NormalMap.Sample( AnisoSampler, input.TexCoord ).rgb * 2.0f - 1.0f );

	// Convert to view space
	float3 normalVS = mul( normalTS, tangentFrameVS );

	// Output a mask value indicating whether the current pixel is an edge pixel
	float msaaMask = dot( abs( frac( input.PositionSS.xy ) - 0.5 ), 1000.0 );

	// Output our G-Buffer values
	float2 normalEncoded  = SpheremapEncode( normalVS );
	return float4( normalEncoded, 64.0f, msaaMask );	
}




