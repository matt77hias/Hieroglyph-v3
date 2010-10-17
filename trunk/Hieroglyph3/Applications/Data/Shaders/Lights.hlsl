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
	matrix ProjMatrix;
	matrix InvProjMatrix;	
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
	float3 ViewRay 		: VIEWRAY;
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
Texture2D		DepthTexture : register( t3 );

//-------------------------------------------------------------------------------------------------
// Vertex shader for when a full screen quad is used for rendering lights
//-------------------------------------------------------------------------------------------------
VSOutput VSQuad( in VSInput input )
{
	VSOutput output;

	// Just copy the position
	output.PositionCS = input.Position;
	
	// Compute the view ray
	float3 positionVS = mul( input.Position, InvProjMatrix ).xyz;
	output.ViewRay = float3( positionVS.xy / positionVS.z, 1.0f );

	return output;
}

//-------------------------------------------------------------------------------------------------
// Decodes a spheremap-encoded normal
//-------------------------------------------------------------------------------------------------
float3 SpheremapDecode( float2 encoded )
{
	float4 nn = float4( encoded, 1, -1 );
    float l = dot( nn.xyz, -nn.xyw );
    nn.z = l;
    nn.xy *= sqrt( l );
    return nn.xyz * 2 + float3( 0, 0, -1 );
}

//-------------------------------------------------------------------------------------------------
// Converts a depth buffer value to view-space position
//-------------------------------------------------------------------------------------------------
float3 PositionFromDepth( float zBufferDepth, float3 viewRay )
{
	float linearDepth = ProjMatrix[3][2] / ( zBufferDepth - ProjMatrix[2][2] );
	return viewRay * linearDepth;
}

//-------------------------------------------------------------------------------------------------
// Helper function for extracting G-Buffer attributes
//-------------------------------------------------------------------------------------------------
void GetGBufferAttributes( in float2 screenPos, in float3 viewRay, out float3 normal, out float3 position, 
							out float3 diffuseAlbedo, out float3 specularAlbedo, out float specularPower )
{
	// Determine our indices for sampling the texture based on the current screen position
	int3 sampleIndices = int3( screenPos.xy, 0 );
	
	#if GBUFFEROPTIMIZATIONS
		normal = SpheremapDecode( NormalTexture.Load( sampleIndices ).xy );
		diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleIndices ).xyz;
		float4 spec = SpecularAlbedoTexture.Load( sampleIndices );
		specularAlbedo = spec.xyz;
		specularPower = spec.w * 255.0f;
		position = PositionFromDepth( DepthTexture.Load( sampleIndices ).x, viewRay );
	#else
		// Sample the G-Buffer textures
		normal = NormalTexture.Load( sampleIndices ).xyz;
		position = PositionTexture.Load( sampleIndices ).xyz;
		diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleIndices ).xyz;
		float4 spec = SpecularAlbedoTexture.Load( sampleIndices );
		specularAlbedo = spec.xyz;
		specularPower = spec.w;
	#endif
}

//-------------------------------------------------------------------------------------------------
// Basic pixel shader, no optimizations
//-------------------------------------------------------------------------------------------------
PSOutput PSMain( in VSOutput input, in float4 screenPos : SV_Position )
{
	PSOutput output;

	// Get the G-Buffer attributes
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;
	GetGBufferAttributes( screenPos.xy, input.ViewRay, normal, position, diffuseAlbedo, specularAlbedo, specularPower );
	
	// Calculate the diffuse term
	float3 L = 0;
	float attenuation = 1.0f;
	#if POINTLIGHT || SPOTLIGHT
		// Base the the light vector on the light position
		L = LightPos - position;
	
		// Calculate attenuation based on distance from the light source
		float dist = length( L );		
		static const float LightRange = 2.0f;
		attenuation = max( 0, ( LightRange - dist ) / LightRange );
	
		L /= dist;
	#elif DIRECTIONALLIGHT
		// Light direction is explicit for directional lights
		L = -LightDirection;
	#endif
	
	#if SPOTLIGHT
		// Also add in the spotlight attenuation factor		
		float3 L2 = LightDirection;
		float rho = dot( -L, L2 );
		attenuation *= saturate( ( rho - SpotlightAngles.y ) / ( SpotlightAngles.x - SpotlightAngles.y ) );
	#endif
	
	float3 diffuse = saturate( dot( normal, L ) ) * LightColor * diffuseAlbedo;
	
	#if GBUFFEROPTIMIZATIONS
		// In view space camera position is (0, 0, 0)
		float3 camPos = 0.0f;
	#else
		float3 camPos = CameraPos;
	#endif
	
	// Calculate the specular term
	float3 V = camPos - position;
	float3 H = normalize( L + V );
	float normalizationFactor = ( ( specularPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 specular = pow( saturate( dot( normal, H ) ), specularPower ) * normalizationFactor * LightColor * specularAlbedo.xyz;
	
	// Final output is the sum of the albedo and diffuse with attenuation applied
	output.Color = float4( ( diffuse + specular ) * attenuation, 1.0f );
	
	return output;
}



