//-----------------------------------------------------------------------------
Texture2D       SilhouetteTarget : register( t0 );           
SamplerState    LinearSampler : register( s0 );
//-----------------------------------------------------------------------------
cbuffer SilhouetteProperties
{
	float4 ObjectColor;
	float4 ScreenResolution;
};
//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoords : TEXCOORD;
};
//-----------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	// Transform the new position to clipspace.
	o.position = float4( v.position, 1.0f );
	o.texcoords.x = (v.position.x + 1.0f) * 0.5f;
	o.texcoords.y = (-v.position.y + 1.0f) * 0.5f;
			
	return o;
}
//-----------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 center = SilhouetteTarget.Sample( LinearSampler, input.texcoords );

	float4 right = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( ScreenResolution.z, 0.0f ) * 1.0f );
	float4 left = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( -ScreenResolution.z, 0.0f ) * 1.0f );
	float4 up = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( 0.0f, -ScreenResolution.w ) * 1.0f );
	float4 down = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( 0.0f, ScreenResolution.w ) * 1.0f );

	float4 upper_right = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( ScreenResolution.z, -ScreenResolution.w ) * 1.0f );
	float4 upper_left = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( -ScreenResolution.z, -ScreenResolution.w ) * 1.0f );
	float4 lower_right = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( ScreenResolution.z, ScreenResolution.w ) * 1.0f );
	float4 lower_left = SilhouetteTarget.Sample( LinearSampler, input.texcoords + float2( -ScreenResolution.z, ScreenResolution.w ) * 1.0f );


	float4 accumulated = right + left + up + down/* + upper_right + upper_left + lower_right + lower_left*/;

	float center_value = center.r + center.g + center.b + center.a;
	float value = accumulated.r + accumulated.g + accumulated.b + accumulated.a;

	float4 color = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	if ( value > 0.0f && center_value <= 0.01f ) {
		color = ObjectColor;
	}

	clip( color.a - 0.01f );

	return( color );
}
//-----------------------------------------------------------------------------