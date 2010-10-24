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
	float4 LightRange;
};

cbuffer CameraParams
{
	float3 CameraPos;
	matrix ProjMatrix;
	matrix InvProjMatrix;
}

cbuffer Transforms
{
	matrix WorldMatrix;
	matrix WorldViewMatrix;
	matrix WorldViewProjMatrix;
};

// This macro indicates whether we're currently rendering a volume
#define VOLUMERENDERING ( ( POINTLIGHT || SPOTLIGHT ) && LIGHTVOLUMES )

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
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput VSMain( in VSInput input )
{
	VSOutput output;

	#if VOLUMERENDERING
		// Apply the world * view * projection
		output.PositionCS = mul( input.Position, WorldViewProjMatrix );
	
		// Calculate the view space vertex position, and output that as the view ray
		float3 positionVS = mul( input.Position, WorldViewMatrix ).xyz;
		output.ViewRay = positionVS;
	#else
		// Just copy the position
		output.PositionCS = input.Position;
	
		// For a quad we can clamp in the vertex shader, since we only interpolate in the XY direction
		float3 positionVS = mul( input.Position, InvProjMatrix ).xyz;
		output.ViewRay = float3( positionVS.xy / positionVS.z, 1.0f );
	#endif
	
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
float3 PositionFromDepth( in float zBufferDepth, in float3 viewRay )
{
	#if VOLUMERENDERING
		// Clamp the view space position to the plane at Z = 1
		viewRay = float3( viewRay.xy / viewRay.z, 1.0f );
	#else
		// For a quad we already clamped in the vertex shader
		viewRay = viewRay.xyz;
	#endif

	// Convert to a linear depth value using the projection matrix
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
		attenuation = max( 0, 1.0f - ( dist / LightRange.x ) );
	
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
	
	float diffuseNormalizationFactor = 1.0f / 3.14159265f;
	float3 diffuse = saturate( dot( normal, L ) ) * LightColor * diffuseAlbedo * diffuseNormalizationFactor;
	
	#if GBUFFEROPTIMIZATIONS
		// In view space camera position is (0, 0, 0)
		float3 camPos = 0.0f;
	#else
		float3 camPos = CameraPos;
	#endif
	
	// Calculate the specular term
	float3 V = camPos - position;
	float3 H = normalize( L + V );
	float specNormalizationFactor = ( ( specularPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float3 specular = pow( saturate( dot( normal, H ) ), specularPower ) * specNormalizationFactor * LightColor * specularAlbedo.xyz;
	
	// Final output is the sum of the albedo and diffuse with attenuation applied
	output.Color = float4( ( diffuse + specular ) * attenuation, 1.0f );
	
	return output;
}



