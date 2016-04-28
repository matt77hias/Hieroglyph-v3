//-----------------------------------------------------------------------------
// dual_paraboloid_environment_map.fx
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
cbuffer ParaboloidLookupParams
{
	matrix WorldViewProjMatrix;
	matrix WorldMatrix;
	matrix ParaboloidBasis;
	float4 ViewPosition;
}
//-----------------------------------------------------------------------------
Texture2DArray  ParaboloidTexture : register( t0 );           
SamplerState    ParaboloidSampler : register( s0 );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
struct VS_INPUT
{
    float3 position	: POSITION;
    float3 normal   : NORMAL;
};
//-----------------------------------------------------------------------------
struct PS_INPUT
{
    float4 position : SV_Position;
	float3 normal   : TEXCOORD0;
	float3 eye      : TEXCOORD1;
};
//-----------------------------------------------------------------------------
struct pixel
{
	float4 color    : COLOR;
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
PS_INPUT VSMAIN( VS_INPUT IN )
{
    PS_INPUT OUT;

	// Find the world space position of the vertex.
	float4 WorldPos		= mul( float4( IN.position, 1 ), WorldMatrix );

	// Output the clip space position for the rasterizer.
	OUT.position		= mul( float4( IN.position, 1 ), WorldViewProjMatrix );

	// Find world space normal and eye vectors.
	OUT.normal			= normalize( mul( IN.normal, (float3x3)WorldMatrix ) );
	OUT.eye				= normalize( WorldPos.xyz - ViewPosition.xyz );
	
	return OUT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PSMAIN( PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	// Normalize the input normal and eye vectors
	float3 N = normalize( IN.normal );
	float3 E = normalize( IN.eye );

	// Calculate the world space reflection vector, and then transform it to 
	// the paraboloid basis.
	float3 R = reflect( E, N );
	R = mul( R, (float3x3)ParaboloidBasis );
	
	// Calculate the forward paraboloid map texture coordinates, with z 
	// determining which paraboloid map to sample (front or back).
	float3 front;
	front.x = (R.x / (2*(1 + R.z))) + 0.5;
	front.y = 1-((R.y / (2*(1 + R.z))) + 0.5);
	front.z = 0.0f;

	// Calculate the backward paraboloid map texture coordinates, with z 
	// determining which paraboloid map to sample (front or back).
	float3 back;
	back.x = (R.x / (2*(1 - R.z))) + 0.5;
	back.y = 1-((R.y / (2*(1 - R.z))) + 0.5);
	back.z = 1.0f;

	// Sample the appropriate paraboloid map based on which direction 
	// the reflection vector is pointing.
	if ( R.z > 0 )
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, front );
	else
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, back );

	OUT *= 0.8f;

    return OUT;
}
//-----------------------------------------------------------------------------