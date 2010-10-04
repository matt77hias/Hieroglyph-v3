//--------------------------------------------------------------------------------
// Lights
//
// Vertex shaders and pixel shaders for performing the lighting pass of a 
// classic deferred renderer
//--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------
cbuffer LightParams
{
	float3 LightPos;
	float3 LightColor;
	float3 LightDirection;
	float2 SpotlightAngles;	
};

cbuffer CameraParams
{
	float3 CameraPos;
}

//-------------------------------------------------------------------------------------------------
// Input/output structs
//-------------------------------------------------------------------------------------------------
struct VSInput
{
	float4 Position : POSITION;
};

struct VSOutput
{
	float4 PositionCS	: SV_Position;
};

struct PSOutput
{
	float4 Color : SV_Target0;
};

//-------------------------------------------------------------------------------------------------
// Textures 
//-------------------------------------------------------------------------------------------------
Texture2D       NormalTexture : register( t0 );
Texture2D		DiffuseAlbedoTexture : register( t1 );
Texture2D		SpecularAlbedoTexture : register( t2 );
Texture2D		PositionTexture : register( t3 );

//-------------------------------------------------------------------------------------------------
// Vertex shader for when a full screen quad is used for rendering lights
//-------------------------------------------------------------------------------------------------
VSOutput VSQuad( in VSInput input )
{
	VSOutput output;

	// Just copy the position
	output.PositionCS = input.Position;

	return output;
}

//-------------------------------------------------------------------------------------------------
// Basic pixel shader, no optimizations
//-------------------------------------------------------------------------------------------------
PSOutput PSMain( in VSOutput input, in float4 screenPos : SV_Position )
{
	PSOutput output;

	// Determine our indices for sampling the texture based on the current screen position
	int3 sampleIndices = int3( screenPos.xy, 0 );
	
	// Sample the G-Buffer textures
	float3 normal = NormalTexture.Load( sampleIndices ).xyz;
	float3 position = PositionTexture.Load( sampleIndices ).xyz;
	float3 diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleIndices ).xyz;
	float4 specularAlbedo = SpecularAlbedoTexture.Load( sampleIndices );
	
	// Calculate the diffuse term
	float3 L = 0;
	float attenuation = 1.0f;
	#if POINTLIGHT || SPOTLIGHT
		// Base the the light vector on the light position
		L = LightPos - position;
	
		// Calculate attenuation based on distance from the light source
		float dist = length( L );
		attenuation = 1.0f / dist;
	
		L /= dist;
	#elif DIRECTIONALLIGHT
		// Light direction is explicit for directional lights
		L = -LightDirection;
	#endif
	
	float3 diffuse = saturate( dot( normal, L ) ) * LightColor * diffuseAlbedo;
	
	// Calculate the specular term
	float3 V = CameraPos - position;
	float3 H = normalize( L + V );
	float normalizationFactor = ( ( specularAlbedo.w + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 specular = pow( saturate( dot( normal, H ) ), specularAlbedo.w ) * normalizationFactor * LightColor * specularAlbedo.xyz;
	
	// Final output is the sum of the albedo and diffuse with attenuation applied
	output.Color = float4( ( diffuse + specular ) * attenuation, 1.0f );
	
	return output;
}



