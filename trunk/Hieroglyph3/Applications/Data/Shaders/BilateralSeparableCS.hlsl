//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Texture2D<float4>		InputMap : register( t0 );
RWTexture2D<float4>		OutputMap : register( u0 );  

// Group size
#define size_x 640
#define size_y 480

static const float filter[7] = {
	0.001769, 0.021558, 0.096587, 0.159235, 0.096587, 0.021558, 0.001769
};

groupshared float4 horizontalpoints[size_x];

[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[DispatchThreadID.x] = CenterColor;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	int3 texturelocation = DispatchThreadID - int3( 3, 0, 0 );


	// Spatial kernel weights definition and range weighting sigma value

	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int x = 0; x < 7; x++ )
	{
		float4 SampleColor = horizontalpoints[texturelocation.x + x];

		float4 Delta = CenterColor - SampleColor;
		float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

		Color += SampleColor * Range * filter[x];
		Weight += Range * filter[x];
	}

	OutputMap[DispatchThreadID.xy] = Color / Weight;
}

groupshared float4 verticalpoints[size_y];

[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[DispatchThreadID.y] = CenterColor;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	int3 texturelocation = DispatchThreadID - int3( 0, 3, 0 );


	// Spatial kernel weights definition and range weighting sigma value

	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int y = 0; y < 7; y++ )
	{
		float4 SampleColor = verticalpoints[texturelocation.y + y];

		float4 Delta = CenterColor - SampleColor;
		float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

		Color += SampleColor * Range * filter[y];
		Weight += Range * filter[y];
	}

	OutputMap[DispatchThreadID.xy] = Color / Weight;
}