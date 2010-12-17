//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Texture2D<float4>		InputMap : register( t0 );
RWTexture2D<float4>		OutputMap : register( u0 );  

// Group size
#define size_x 32
#define size_y 32

static const float filter[7][7] = {
	0.000020, 0.000236, 0.001081, 0.001769, 0.001081, 0.000236, 0.000020,
	0.000236, 0.002908, 0.013068, 0.021558, 0.013068, 0.002908, 0.000236,
	0.001081, 0.013068, 0.058581, 0.096587, 0.058581, 0.013068, 0.001081,
	0.001769, 0.021558, 0.096587, 0.159235, 0.096587, 0.021558, 0.001769,
	0.001081, 0.013068, 0.058581, 0.096587, 0.058581, 0.013068, 0.001081,
	0.000236, 0.002908, 0.013068, 0.021558, 0.013068, 0.002908, 0.000236,
	0.000020, 0.000236, 0.001081, 0.001769, 0.001081, 0.000236, 0.000020
};

[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int3 texturelocation = DispatchThreadID - int3( 3, 3, 0 );

	// Each thread will load its own depth/occlusion values
	float4 CenterColor = InputMap.Load( DispatchThreadID );
	

	// Spatial kernel weights definition and range weighting sigma value

	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int x = 0; x < 7; x++ )
	{
		for ( int y = 0; y < 7; y++ )
		{
			float4 SampleColor = InputMap.Load( texturelocation + int3( x, y, 0 ) );

			float4 Delta = CenterColor - SampleColor;
			float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

			Color += SampleColor * Range * filter[x][y];
			Weight += Range * filter[x][y];
		}
	}

	OutputMap[DispatchThreadID.xy] = Color / Weight;
}