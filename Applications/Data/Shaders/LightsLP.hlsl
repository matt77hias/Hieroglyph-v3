//--------------------------------------------------------------------------------
// LightsLP
//
// Vertex shaders and pixel shaders for performing the lighting pass of a
// light prepass deferred renderer
//--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------
cbuffer CameraParams
{
	matrix ViewMatrix;
	matrix ProjMatrix;
	matrix InvProjMatrix;
	float2 ClipPlanes;
}

// This indicates the number of MSAA samples
#define NUMSUBSAMPLES 4

//-------------------------------------------------------------------------------------------------
// Input/output structs
//-------------------------------------------------------------------------------------------------
struct VSInput
{
	#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosWS 		: POSITION;
		float LightRange 		: RANGE;
	#endif

	float3 LightColor 			: COLOR;

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionWS : DIRECTION;
	#endif

	#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	#endif
};

struct VSOutput
{
	#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosVS 		: POSITION;
		float LightRange 		: RANGE;
	#endif

	float3 LightColor 			: COLOR;

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionVS : DIRECTION;
	#endif

	#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	#endif
};

struct GSOutput
{
	float4 PositionCS			: SV_Position;
	float3 ViewRay				: VIEWRAY;

	#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosVS 		: POSITION;
		float LightRange 		: RANGE;
	#endif

	float3 LightColor 			: COLOR;

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionVS : DIRECTION;
	#endif

	#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	#endif
};

struct PSInput
{
	float4 ScreenPos			: SV_Position;
	float3 ViewRay				: VIEWRAY;

	#if ( POINTLIGHT || SPOTLIGHT )
		float3 LightPosVS 		: POSITION;
		float LightRange 		: RANGE;
	#endif

	float3 LightColor 			: COLOR;

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		float3 LightDirectionVS : DIRECTION;
	#endif

	#if SPOTLIGHT
		float2 SpotlightAngles	: SPOTANGLES;
	#endif
};

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2DMS<float4> GBufferTexture : register( t0 );
Texture2DMS<float> DepthTexture : register( t1 );

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point. This shader passes along the light parameters to the geometry shader,
// and also transforms the light postition and direction to view space.
//-------------------------------------------------------------------------------------------------
VSOutput VSMain( in VSInput input )
{
	VSOutput output;

	output.LightColor = input.LightColor;

	#if ( POINTLIGHT || SPOTLIGHT )
		output.LightPosVS = mul( float4( input.LightPosWS, 1.0f ), ViewMatrix ).xyz;
		output.LightRange = input.LightRange;
	#endif

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		output.LightDirectionVS = mul( input.LightDirectionWS, (float3x3)ViewMatrix );
	#endif

	#if SPOTLIGHT
		output.SpotlightAngles = input.SpotlightAngles;
	#endif

	return output;
}

//-------------------------------------------------------------------------------------------------
// Determines the extents of a minimum-area quad fit to a spherical light volume
//-------------------------------------------------------------------------------------------------
void CalcLightQuad( in float3 lightPosVS, in float lightRange, out float2 topLeft, out float2 bottomRight )
{
	// Fit a bounding sphere to the light, where the center is the light position and the radius
	// is the light's range.
	float4 lightCenterVS = float4( lightPosVS, 1.0f );
	float radius = lightRange;

    // Figure out the four points at the top, bottom, left, and right of the sphere
    float4 topVS = lightCenterVS + float4( 0.0f, radius, 0.0f, 0.0f );
    float4 bottomVS = lightCenterVS - float4( 0.0f, radius, 0.0f, 0.0f );
    float4 leftVS = lightCenterVS - float4( radius, 0.0f, 0.0f, 0.0f );
    float4 rightVS = lightCenterVS + float4( radius, 0.0f, 0.0f, 0.0f );

    // Figure out whether we want to use the top and right from quad
    // tangent to the front of the sphere, or the back of the sphere
    leftVS.z = leftVS.x < 0.0f ? leftVS.z - radius : leftVS.z + radius;
    rightVS.z = rightVS.x < 0.0f ? rightVS.z + radius : rightVS.z - radius;
    topVS.z = topVS.y < 0.0f ? topVS.z + radius : topVS.z - radius;
    bottomVS.z = bottomVS.y < 0.0f ? bottomVS.z - radius : bottomVS.z + radius;

    // Clamp the z coordinate to the clip planes
    leftVS.z = clamp( leftVS.z, ClipPlanes.x, ClipPlanes.y );
    rightVS.z = clamp( rightVS.z, ClipPlanes.x, ClipPlanes.y );
    topVS.z = clamp( topVS.z, ClipPlanes.x, ClipPlanes.y );
    bottomVS.z = clamp( bottomVS.z, ClipPlanes.x, ClipPlanes.y );

    // Figure out the rectangle in clip-space by applying the perspective transform.
    // We assume that the perspective transform is symmetrical with respect to X and Y.
    float rectLeftCS = leftVS.x * ProjMatrix[0][0] / leftVS.z;
    float rectRightCS = rightVS.x * ProjMatrix[0][0] / rightVS.z;
    float rectTopCS = topVS.y * ProjMatrix[1][1] / topVS.z;
    float rectBottomCS = bottomVS.y * ProjMatrix[1][1] / bottomVS.z;

    // clamp the rectangle to the screen extents
    rectTopCS = clamp( rectTopCS, -1.0f, 1.0f );
    rectBottomCS = clamp( rectBottomCS, -1.0f, 1.0f );
    rectLeftCS = clamp( rectLeftCS, -1.0f, 1.0f );
    rectRightCS = clamp( rectRightCS, -1.0f, 1.0f );

    topLeft = float2( rectLeftCS, rectTopCS );
	bottomRight = float2( rectRightCS, rectBottomCS );
}

//-------------------------------------------------------------------------------------------------
// Geometry shader entry point. This shader emits a quad, whose extents are determined by finding
// the screen space min and max of a view space-aligned bounding box fit to the light volume.
// For directional lights, a full-screen quad is emitted instead.
//-------------------------------------------------------------------------------------------------
[maxvertexcount(4)]
void GSMain(point VSOutput input[1], inout TriangleStream<GSOutput> QuadStream)
{
	GSOutput output;

	// Pass along light properties
	output.LightColor = input[0].LightColor;

	#if ( POINTLIGHT || SPOTLIGHT )
		output.LightPosVS = input[0].LightPosVS;
		output.LightRange = input[0].LightRange;
	#endif

	#if ( DIRECTIONALLIGHT || SPOTLIGHT )
		output.LightDirectionVS = input[0].LightDirectionVS;
	#endif

	#if SPOTLIGHT
		output.SpotlightAngles = input[0].SpotlightAngles;
	#endif

	#if ( POINTLIGHT || SPOTLIGHT )				
		// Figure out a depth for the quad, by using the point on the sphere 
		// furthest from the camera. We combine this with "greater-than" depth testing,
		// so that any pixels where the sphere is "floating in air" will be culled and only
		// pixels where the volume intersects with geometry will be lit. This is based on 
		// the fact that our scene is mostly an empty volume, with walls on the far side.
		// However we'll clamp to the far plane if the light intersects with it, so that
		// we don't inadvertantly clip pixels that still need to be lit.
		float lightDepth = input[0].LightPosVS.z + input[0].LightRange;
		if ( input[0].LightPosVS.z - input[0].LightRange < ClipPlanes.y )
			lightDepth = min( lightDepth, ClipPlanes.y );

		// Project the light depth
		lightDepth = mul( float4( 0, 0, lightDepth, 1.0f ), ProjMatrix ).z / lightDepth;

		// Figure out the quad extents
		float2 topLeft, bottomRight;
		CalcLightQuad( input[0].LightPosVS, input[0].LightRange, topLeft, bottomRight );

		float4 quadVerts[4] =
		{
			float4( topLeft.x, topLeft.y, lightDepth, 1.0f ),
			float4( bottomRight.x, topLeft.y, lightDepth, 1.0f ),
			float4( topLeft.x, bottomRight.y, lightDepth, 1.0f ),
			float4( bottomRight.x, bottomRight.y, lightDepth, 1.0f )
		};
	#elif DIRECTIONALLIGHT
		float4 quadVerts[4] =
		{
			float4( -1, 1, 1, 1 ),
			float4( 1, 1, 1, 1 ),
			float4( -1, -1, 1, 1 ),
			float4( 1, -1, 1, 1 )
		};
	#endif

	// Emit 4 new verts, and 2 new triangles
	for (int i = 0; i < 4; i++)
	{
		output.PositionCS = quadVerts[i];

		// Calculate the view ray by unprojecting the vertex position into view space.
		// For a quad we can clamp in the vertex shader, since we only interpolate in the XY direction.
		float3 positionVS = mul( quadVerts[i], InvProjMatrix ).xyz;
		output.ViewRay = float3( positionVS.xy / positionVS.z, 1.0f );

		QuadStream.Append(output);
	}

	QuadStream.RestartStrip();
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
	// For a quad we already clamped in the vertex shader
	viewRay = viewRay.xyz;

	// Convert to a linear depth value using the projection matrix
	float linearDepth = ProjMatrix[3][2] / ( zBufferDepth - ProjMatrix[2][2] );
	return viewRay * linearDepth;
}

//-------------------------------------------------------------------------------------------------
// Helper function for extracting G-Buffer attributes
//-------------------------------------------------------------------------------------------------
void GetGBufferAttributes( in float2 screenPos, in float3 viewRay, in uint subSampleIndex,
							out float3 normal, out float3 position, out float specularPower )
{
	// Determine our coordinate for sampling the texture based on the current screen position
	int2 sampleCoord = int2( screenPos.xy );

	float4 gBuffer = GBufferTexture.Load( sampleCoord, subSampleIndex );
	normal = SpheremapDecode( gBuffer.xy );
	position = PositionFromDepth( DepthTexture.Load( sampleCoord, subSampleIndex ).x, viewRay );
	specularPower = gBuffer.z;
}

//-------------------------------------------------------------------------------------------------
// Calculates the lighting term for a single G-Buffer texel
//-------------------------------------------------------------------------------------------------
float4 CalcLighting( in float3 normal, in float3 position, in float specularPower, in PSInput input )
{
	// Calculate the diffuse term
	float3 L = 0;
	float attenuation = 1.0f;
	#if POINTLIGHT || SPOTLIGHT
		// Base the the light vector on the light position
		L = input.LightPosVS - position;

		// Calculate attenuation based on distance from the light source
		float dist = length( L );
		attenuation = max( 0, 1.0f - ( dist / input.LightRange ) );

		L /= dist;
	#elif DIRECTIONALLIGHT
		// Light direction is explicit for directional lights
		L = -input.LightDirectionVS;
	#endif

	#if SPOTLIGHT
		// Also add in the spotlight attenuation factor
		float3 L2 = input.LightDirectionVS;
		float rho = dot( -L, L2 );
		attenuation *= saturate( ( rho - input.SpotlightAngles.y ) / ( input.SpotlightAngles.x - input.SpotlightAngles.y ) );
	#endif

	float nDotL = saturate( dot( normal, L ) );
	float3 diffuse = nDotL * input.LightColor * attenuation;

	// In view space camera position is (0, 0, 0)
	float3 camPos = float3(0.0f, 0.0f, 0.0f);

	// Calculate the specular term
	float3 V = camPos - position;
	float3 H = normalize( L + V );
	float specNormalizationFactor = ( ( specularPower + 8.0f ) / ( 8.0f * 3.14159265f ) );
	float specular = pow( saturate( dot( normal, H ) ), specularPower ) * specNormalizationFactor * nDotL * attenuation;

	// Final value is diffuse RGB + mono specular
	return float4( diffuse, specular );
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-pixel.
//-------------------------------------------------------------------------------------------------
float4 PSMain( in PSInput input ) : SV_Target0
{
	float3 normal;
	float3 position;
	float specularPower;

	// Get the G-Buffer values for the first sub-sample, and calculate the lighting
	GetGBufferAttributes( input.ScreenPos.xy, input.ViewRay, 0, normal, position, specularPower );

	return CalcLighting( normal, position, specularPower, input );	
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-sample. Note that this shader doesn't need to
// use SV_Coverage to determine if the light volume is covering a particular subsample, because
// the shader is running per-sample. So if a subsample isn't covered or doesn't pass the
// depth/stencil test, then the shader simply won't execute at all for that sample.
// Also running at per-sample frequency means that the lighting values are stored individually
// for each sample, so that the final pass can decide per-sample which values to use.
//-------------------------------------------------------------------------------------------------
float4 PSMainPerSample( in PSInput input, in uint subSampleIndex : SV_SampleIndex )  : SV_Target0
{
	float3 normal;
	float3 position;
	float specularPower;

	// Get the G-Buffer values for the current sub-sample, and calculate the lighting
	GetGBufferAttributes( input.ScreenPos.xy, input.ViewRay, subSampleIndex, normal, position, specularPower );

	return CalcLighting( normal, position, specularPower, input );	
}
