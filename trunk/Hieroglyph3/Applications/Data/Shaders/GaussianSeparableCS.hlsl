//
// GaussianSeparableCS.hlsl
//
// Copyright (C) 2010  Jason Zink 

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

void CSMAINX( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[DispatchThreadID.x] = data;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID - int3( 3, 0, 0 );

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int x = 0; x < 7; x++ )
		Color += horizontalpoints[texturelocation.x + x] * filter[x];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}


// Declare the group shared memory to hold the loaded data
groupshared float4 verticalpoints[size_y];

// For the vertical pass, use only a single column of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[DispatchThreadID.y] = data;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID - int3( 0, 3, 0 );

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int y = 0; y < 7; y++ )
		Color += verticalpoints[texturelocation.y + y] * filter[y];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}
