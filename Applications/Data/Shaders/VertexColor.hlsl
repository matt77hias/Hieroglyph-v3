//-----------------------------------------------------------------------------
cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
};

//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
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
	VS_OUTPUT o = (VS_OUTPUT)0;

	// Transform the new position to clipspace.
	o.position = mul( float4(v.position, 1.0f), WorldViewProjMatrix );
	o.color = v.color;
			
	return o;
}
//-----------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 color = input.color;
	//float4 color = float4( 0.75f, 0.75f, 1.0f, 1.0f );

	return( color );
}
//-----------------------------------------------------------------------------