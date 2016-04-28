//-----------------------------------------------------------------------------
cbuffer FullscreenObjectProperties
{
	float4 ObjectColor;
};
//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
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
	o.position = float4( v.position, 1.0f );
	o.color = ObjectColor;
			
	return o;
}
//-----------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 color = input.color;
	
	return( color );
}
//-----------------------------------------------------------------------------