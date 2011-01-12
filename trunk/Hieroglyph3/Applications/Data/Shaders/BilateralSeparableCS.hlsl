//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Declare the input and output resources
Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

// Image sizes
#define size_x 640
#define size_y 480

// Declare the filter kernel coefficients
static const float filter[7] = {
	0.030078323, 0.104983664, 0.222250419, 0.285375187, 0.222250419, 0.104983664, 0.030078323
};

// Declare the group shared memory to hold the loaded data
groupshared float4 horizontalpoints[size_x];

// For the horizontal pass, use only a single row of threads
[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 DispatchThreadID : SV_DispatchThreadID )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[DispatchThreadID.x] = CenterColor;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID - int3( 3, 0, 0 );

	// Range sigma value
	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int x = 0; x < 7; x++ )
	{
		// Get the current sample
		float4 SampleColor = horizontalpoints[texturelocation.x + x];

		// Find the delta, and use that to calculate the range weighting
		float4 Delta = CenterColor - SampleColor;
		float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

		// Sum both the color result and the total weighting used
		Color += SampleColor * Range * filter[x];
		Weight += Range * filter[x];
	}

	// Store the renormalized result to the output resource
	OutputMap[DispatchThreadID.xy] = Color / Weight;
}

// Declare the group shared memory to hold the loaded data
groupshared float4 verticalpoints[size_y];

// For the vertical pass, use only a single column of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[DispatchThreadID.y] = CenterColor;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID - int3( 0, 3, 0 );

	// Range sigma value
	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int y = 0; y < 7; y++ )
	{
		// Get the current sample
		float4 SampleColor = verticalpoints[texturelocation.y + y];

		// Find the delta, and use that to calculate the range weighting
		float4 Delta = CenterColor - SampleColor;
		float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

		// Sum both the color result and the total weighting used
		Color += SampleColor * Range * filter[y];
		Weight += Range * filter[y];
	}

	// Store the renormalized result to the output resource
	OutputMap[DispatchThreadID.xy] = Color / Weight;
}