//-----------------------------------------------------------------------------
cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
};

cbuffer DispatchParams
{
	float4 DispatchSize;
};

struct GridPoint
{
    float  Height;
    float4 Flow;
};

StructuredBuffer<GridPoint>   CurrentWaterState  : register(t0);
//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float2 coords : TEXCOORDS;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	/*nointerpolation*/ float4 color : COLOR ;
};
//-----------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	// Find where in the Water State buffer to look up for this vertex.
	int3 coords = int3( floor(v.coords.x), floor(v.coords.y), 0 );
	int textureindex = coords.x + coords.y * DispatchSize.z;

	// Sample the heightmap at the specified location.
	float height = CurrentWaterState[textureindex].Height;

	// Transform the new position to clipspace.
	float4 SampledPosition = float4( v.position.x, height, v.position.z, 1.0f );
	o.position = mul( SampledPosition, WorldViewProjMatrix );
	
	// Select the color of the grid based on the thread group it belongs to.
	int2 GridPosition = ( v.coords.xy / 16.0f );
	float ColorIndex = frac( ( GridPosition.x + GridPosition.y ) / 2.0f );
	
	if ( ColorIndex < 0.5f )
		o.color = float4( 0.0f, 1.0f, 0.0f, 1.0f );
	else
		o.color = float4( 0.0f, 0.0f, 1.0f, 1.0f );
			
	return o;
}
//-----------------------------------------------------------------------------
float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float4 color = input.color;
	//float4 color = float4( 0.75f, 0.75f, 1.0f, 0.0f );

	return( color );
}
//-----------------------------------------------------------------------------