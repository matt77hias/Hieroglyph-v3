

cbuffer StaticMeshTransforms
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;	
};

cbuffer LightParameters
{
	float3 LightPositionWS;
	float4 LightColor;
};


Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );


struct VS_INPUT
{
	float3 position : POSITION;
	float2 tex		: TEXCOORDS0;
	float3 normal	: NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 light	: LIGHT;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Generate the clip space position for feeding to the rasterizer
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );

	// Generate the world space normal vector
	output.normal = mul( input.normal, (float3x3)WorldMatrix );

	// Find the world space position of the vertex
	float3 PositionWS = mul( float4( input.position, 1.0f ), WorldMatrix ).xyz;

	// Calculate the world space light vector
	output.light = LightPositionWS - PositionWS;

	// Pass through the texture coordinates
	output.tex = input.tex;

	return output;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	// Normalize the world space normal and light vectors
	float3 n = normalize( input.normal );
	float3 l = normalize( input.light );

	// Calculate the amount of light reaching this fragment
	float4 Illumination = max(dot(n,l),0) + 0.2f;

	// Determine the color properties of the surface from a texture
	float4 SurfaceColor = ColorTexture.Sample( LinearSampler, input.tex );

	// Return the surface color modulated by the illumination
	return( SurfaceColor * Illumination );
}


