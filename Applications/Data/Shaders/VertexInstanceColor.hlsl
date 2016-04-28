//-----------------------------------------------------------------------------
cbuffer InstanceTransforms
{
	matrix ViewProjMatrix;	
};

//-----------------------------------------------------------------------------
struct VS_INPUT
{
	// Per-vertex data
    float3 position : POSITION;

	// Instance data
    float4x4 transform : TRANSFORM;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
//-----------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT v )
{
	VS_OUTPUT o;

	float4 positionWS = mul( float4( v.position, 1 ), v.transform );

	// Transform the new position to clipspace.
	o.position = mul( positionWS, ViewProjMatrix );
	o.color = v.color;
			
	return o;
}
//-----------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 color = input.color;

	return( color );
}
//-----------------------------------------------------------------------------