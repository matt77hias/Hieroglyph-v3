//
// GaussianSeparableCS.hlsl
//
// This pair of compute shaders implements a separable 7x7 Gaussian filter.
// The image is processed one line at a time, with each line consisting of
// a portion of the input image (or all of the image if it is small enough).
// To allow the pixels on the end of each line to be properly processed, extra
// data is loaded by the first and last threads of the thread group.
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

// Declare the group shared memory to hold the loaded data, including three
// extra samples on the left and right of this line.
groupshared float4 horizontalpoints[3+size_x+3];

// For the horizontal pass, use only a single row of threads
[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[GroupThreadID.x+3] = data;

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

	// Offset the texture location to the first sample location.  Since there are
	// three more samples loaded than there are threads, we start without an offset
	// into the group shared memory.  This allows all seven samples to be sequentially
	// accessed in a simple way.
	int texturelocation = GroupThreadID.x;

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int x = 0; x < 7; x++ )
		Color += horizontalpoints[texturelocation + x] * filter[x];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}


// Declare the group shared memory to hold the loaded data, including three
// extra samples on the left and right of this line.
groupshared float4 verticalpoints[3+size_y+3];

// For the vertical pass, use only a single column of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[GroupThreadID.y+3] = data;

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

	// Offset the texture location to the first sample location.  Since there are
	// three more samples loaded than there are threads, we start without an offset
	// into the group shared memory.  This allows all seven samples to be sequentially
	// accessed in a simple way.
	int texturelocation = GroupThreadID.y;

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int y = 0; y < 7; y++ )
		Color += verticalpoints[texturelocation + y] * filter[y];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}
