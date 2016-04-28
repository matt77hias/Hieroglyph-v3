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
groupshared float4 horizontalpoints[3+size_x+3];

// For the horizontal pass, use only a single row of threads
[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[GroupThreadID.x+3] = CenterColor;

	// Load the data to the left of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {
		horizontalpoints[0] = InputMap.Load(DispatchThreadID-int3(3,0,0));
		horizontalpoints[1] = InputMap.Load(DispatchThreadID-int3(2,0,0));
		horizontalpoints[2] = InputMap.Load(DispatchThreadID-int3(1,0,0));
	}

	// Load the data to the right of this line for the last few pixels to use.
	if ( GroupIndex == size_x-1 ) {
		horizontalpoints[3+size_x+0] = InputMap.Load(DispatchThreadID+int3(1,0,0));
		horizontalpoints[3+size_x+1] = InputMap.Load(DispatchThreadID+int3(2,0,0));
		horizontalpoints[3+size_x+2] = InputMap.Load(DispatchThreadID+int3(3,0,0));
	}

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = GroupThreadID;

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
groupshared float4 verticalpoints[3+size_y+3];

// For the vertical pass, use only a single column of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[GroupThreadID.y+3] = CenterColor;
	
	// Load the data to the top of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {
		verticalpoints[0] = InputMap.Load(DispatchThreadID-int3(0,3,0));
		verticalpoints[1] = InputMap.Load(DispatchThreadID-int3(0,2,0));
		verticalpoints[2] = InputMap.Load(DispatchThreadID-int3(0,1,0));
	}

	// Load the data to the bottom of this line for the last few pixels to use.
	if ( GroupIndex == size_y-1 ) {
		verticalpoints[3+size_y+0] = InputMap.Load(DispatchThreadID+int3(0,1,0));
		verticalpoints[3+size_y+1] = InputMap.Load(DispatchThreadID+int3(0,2,0));
		verticalpoints[3+size_y+2] = InputMap.Load(DispatchThreadID+int3(0,3,0));
	}

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = GroupThreadID;

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