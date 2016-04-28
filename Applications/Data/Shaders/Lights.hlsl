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

// This indicates the number of MSAA samples
#if MSAA
	#define NUMSUBSAMPLES 4
#else
	#define NUMSUBSAMPLES 1
#endif

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
#if MSAA
	Texture2DMS<float4, 4> NormalTexture : register( t0 );
	Texture2DMS<float4, 4> DiffuseAlbedoTexture : register( t1 );
	Texture2DMS<float4, 4> SpecularAlbedoTexture : register( t2 );
	Texture2DMS<float4, 4> PositionTexture : register( t3 );
	Texture2DMS<float, 4> DepthTexture : register( t3 );
#else
	Texture2D       NormalTexture : register( t0 );
	Texture2D		DiffuseAlbedoTexture : register( t1 );
	Texture2D		SpecularAlbedoTexture : register( t2 );
	Texture2D		PositionTexture : register( t3 );
	Texture2D		DepthTexture : register( t3 );
#endif

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
void GetGBufferAttributes( in float2 screenPos, in float3 viewRay, in int subSampleIndex, out float3 normal,
							out float3 position, out float3 diffuseAlbedo, out float3 specularAlbedo,
							out float specularPower )
{
	// Determine our coordinate for sampling the texture based on the current screen position
	int3 sampleCoord = int3( screenPos.xy, 0 );

	#if GBUFFEROPTIMIZATIONS
		#if MSAA
			diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleCoord.xy, subSampleIndex ).xyz;
			normal = SpheremapDecode( NormalTexture.Load( sampleCoord.xy, subSampleIndex ).xy );
			float4 spec = SpecularAlbedoTexture.Load( sampleCoord.xy, subSampleIndex );
			position = PositionFromDepth( DepthTexture.Load( sampleCoord.xy, subSampleIndex ).x, viewRay );
		#else
			normal = SpheremapDecode( NormalTexture.Load( sampleCoord ).xy );
			diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleCoord ).xyz;
			float4 spec = SpecularAlbedoTexture.Load( sampleCoord );
			position = PositionFromDepth( DepthTexture.Load( sampleCoord ).x, viewRay );
		#endif

		specularAlbedo = spec.xyz;
		specularPower = spec.w * 255.0f;
	#else
		#if MSAA
			normal = NormalTexture.Load( sampleCoord.xy, subSampleIndex ).xyz;
			position = PositionTexture.Load( sampleCoord.xy, subSampleIndex ).xyz;
			diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleCoord.xy, subSampleIndex ).xyz;
			float4 spec = SpecularAlbedoTexture.Load( sampleCoord.xy, subSampleIndex );
		#else
			normal = NormalTexture.Load( sampleCoord ).xyz;
			position = PositionTexture.Load( sampleCoord ).xyz;
			diffuseAlbedo = DiffuseAlbedoTexture.Load( sampleCoord ).xyz;
			float4 spec = SpecularAlbedoTexture.Load( sampleCoord );
		#endif

		specularAlbedo = spec.xyz;
		specularPower = spec.w;
	#endif
}

//-------------------------------------------------------------------------------------------------
// Calculates the lighting term for a single G-Buffer texel
//-------------------------------------------------------------------------------------------------
float3 CalcLighting( in float3 normal, in float3 position, in float3 diffuseAlbedo,
				     in float3 specularAlbedo, in float specularPower )
{
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

	const float DiffuseNormalizationFactor = 1.0f / 3.14159265f;
	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * LightColor * diffuseAlbedo * DiffuseNormalizationFactor;

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
	float3 specular = pow( saturate( dot( normal, H ) ), specularPower ) * specNormalizationFactor * LightColor * specularAlbedo.xyz * nDotL;

	// Final value is the sum of the albedo and diffuse with attenuation applied
	return ( diffuse + specular ) * attenuation;
}

//-------------------------------------------------------------------------------------------------
// Light pixel shader
//-------------------------------------------------------------------------------------------------
PSOutput PSMain( in VSOutput input, in float4 screenPos : SV_Position, in uint coverage : SV_Coverage )
{
	PSOutput output;
	float3 lighting = 0;
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	#if MSAA
		// Calculate lighting for MSAA samples
		float numSamplesApplied = 0.0f;
		for ( int i = 0; i < NUMSUBSAMPLES; ++i )
		{
			// We only want to calculate lighting for a sample if the light volume is covered.
			// We determine this using the input coverage mask.
			if ( coverage & ( 1 << i ) )
			{
				GetGBufferAttributes( screenPos.xy, input.ViewRay, i, normal, position, diffuseAlbedo,
									specularAlbedo, specularPower );
				lighting += CalcLighting( normal, position, diffuseAlbedo, specularAlbedo, specularPower );
				++numSamplesApplied;
			}
		}

		lighting /= numSamplesApplied;
	#else
		// Calculate lighting for a single G-Buffer sample
		GetGBufferAttributes( screenPos.xy, input.ViewRay, 0, normal, position, diffuseAlbedo,
								specularAlbedo, specularPower );
		lighting = CalcLighting( normal, position, diffuseAlbedo, specularAlbedo, specularPower );
	#endif

	output.Color = float4( lighting, 1.0f );

	return output;
}



