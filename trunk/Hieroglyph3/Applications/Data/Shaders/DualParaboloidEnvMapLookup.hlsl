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

	float4 WorldPos		= mul( float4(IN.position, 1), WorldMatrix );		// find world space position
	float3 N			= normalize(mul(IN.normal, (float3x3)WorldMatrix));	// find world space normal
	float3 E			= normalize(WorldPos.xyz - ViewPosition.xyz);			// find world space eye vector

	OUT.position		= mul( float4(IN.position, 1), WorldViewProjMatrix );	// output clip space position
	OUT.normal			= N;									// output world space normal vector
	OUT.eye				= E;									// output world space eye vector
	
	return OUT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PSMAIN( PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	float3 N = normalize( IN.normal );			// normalize input per-vertex normal vector
	float3 E = normalize( IN.eye );				// normalize input per-vertex eye vector
	float3 R = reflect( E, N );					// calculate the reflection vector
	
	R = mul( R, (float3x3)ParaboloidBasis );	// transform reflection vector to the maps basis
	
	// calculate the forward paraboloid map texture coordinates	
	float3 front;
	front.x = (R.x / (2*(1 + R.z))) + 0.5;
	front.y = 1-((R.y / (2*(1 + R.z))) + 0.5);
	front.z = 0.0f;

	// calculate the backward paraboloid map texture coordinates
	float3 back;
	back.x = (R.x / (2*(1 - R.z))) + 0.5;
	back.y = 1-((R.y / (2*(1 - R.z))) + 0.5);
	back.z = 1.0f;

	//float4 forward = tex2D( FrontSampler, front );	// sample the front paraboloid map
	//float4 backward = tex2D( BackSampler, back );	// sample the back paraboloid map
	//OUT.color = max(forward, backward);				// output the max of the two maps

	if ( R.z > 0 )
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, front );	// sample the front paraboloid map
	else
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, back );	// sample the back paraboloid map

	OUT *= 0.8f;

    return OUT;
}
//-----------------------------------------------------------------------------