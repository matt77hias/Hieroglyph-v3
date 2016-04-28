//
// InvertColorCS.hlsl
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

// Declare the group shared memory to hold the loaded and calculated data
groupshared float4 horizontalpoints[3+size_x+3][3];

// For the horizontal pass, use only a single row of threads
[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[GroupThreadID.x+3][0] = data * filter[0];
	horizontalpoints[GroupThreadID.x+3][1] = data * filter[1];
	horizontalpoints[GroupThreadID.x+3][2] = data * filter[2];

	// Load the data to the left of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {

		float4 localData = InputMap.Load(DispatchThreadID-int3(3,0,0));

		horizontalpoints[0][0] = localData * filter[0];
		horizontalpoints[0][1] = localData * filter[1];
		horizontalpoints[0][2] = localData * filter[2];

		localData = InputMap.Load(DispatchThreadID-int3(2,0,0));
		
		horizontalpoints[1][0] = localData * filter[0];
		horizontalpoints[1][1] = localData * filter[1];
		horizontalpoints[1][2] = localData * filter[2];
		
		localData = InputMap.Load(DispatchThreadID-int3(1,0,0));

		horizontalpoints[2][0] = localData * filter[0];
		horizontalpoints[2][1] = localData * filter[1];
		horizontalpoints[2][2] = localData * filter[2];
	}

	// Load the data to the right of this line for the last few pixels to use.
	if ( GroupIndex == size_x-1 ) {
		float4 localData = InputMap.Load(DispatchThreadID+int3(1,0,0));

		horizontalpoints[3+size_x+0][0] = localData * filter[0];
		horizontalpoints[3+size_x+0][1] = localData * filter[1];
		horizontalpoints[3+size_x+0][2] = localData * filter[2];

		localData = InputMap.Load(DispatchThreadID+int3(2,0,0));

		horizontalpoints[3+size_x+1][0] = localData * filter[0];
		horizontalpoints[3+size_x+1][1] = localData * filter[1];
		horizontalpoints[3+size_x+1][2] = localData * filter[2];
		
		localData = InputMap.Load(DispatchThreadID+int3(3,0,0));

		horizontalpoints[3+size_x+2][0] = localData * filter[0];
		horizontalpoints[3+size_x+2][1] = localData * filter[1];
		horizontalpoints[3+size_x+2][2] = localData * filter[2];
	}

	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = GroupThreadID;

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );
	
	Color += horizontalpoints[texturelocation.x + 0][0];
	Color += horizontalpoints[texturelocation.x + 1][1];
	Color += horizontalpoints[texturelocation.x + 2][2];
	Color += data * filter[3];
	Color += horizontalpoints[texturelocation.x + 4][2];
	Color += horizontalpoints[texturelocation.x + 5][1];
	Color += horizontalpoints[texturelocation.x + 6][0];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}


// Declare the group shared memory to hold the loaded and calculated data
groupshared float4 verticalpoints[3+size_y+3][3];

// For the vertical pass, use only a single column of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[GroupThreadID.y+3][0] = data * filter[0];
	verticalpoints[GroupThreadID.y+3][1] = data * filter[1];
	verticalpoints[GroupThreadID.y+3][2] = data * filter[2];
	
	// Load the data to the top of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {

		float4 localData = InputMap.Load(DispatchThreadID-int3(0,3,0));

		verticalpoints[0][0] = localData * filter[0];
		verticalpoints[0][1] = localData * filter[1];
		verticalpoints[0][2] = localData * filter[2];

		localData = InputMap.Load(DispatchThreadID-int3(0,2,0));
		
		verticalpoints[1][0] = localData * filter[0];
		verticalpoints[1][1] = localData * filter[1];
		verticalpoints[1][2] = localData * filter[2];
		
		localData = InputMap.Load(DispatchThreadID-int3(0,1,0));

		verticalpoints[2][0] = localData * filter[0];
		verticalpoints[2][1] = localData * filter[1];
		verticalpoints[2][2] = localData * filter[2];
	}

	// Load the data to the bottom of this line for the last few pixels to use.
	if ( GroupIndex == size_y-1 ) {
		float4 localData = InputMap.Load(DispatchThreadID+int3(0,1,0));

		verticalpoints[3+size_y+0][0] = localData * filter[0];
		verticalpoints[3+size_y+0][1] = localData * filter[1];
		verticalpoints[3+size_y+0][2] = localData * filter[2];

		localData = InputMap.Load(DispatchThreadID+int3(0,2,0));

		verticalpoints[3+size_y+1][0] = localData * filter[0];
		verticalpoints[3+size_y+1][1] = localData * filter[1];
		verticalpoints[3+size_y+1][2] = localData * filter[2];
		
		localData = InputMap.Load(DispatchThreadID+int3(0,3,0));

		verticalpoints[3+size_y+2][0] = localData * filter[0];
		verticalpoints[3+size_y+2][1] = localData * filter[1];
		verticalpoints[3+size_y+2][2] = localData * filter[2];
	}

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Offset the texture location to the first sample location
	int3 texturelocation = GroupThreadID;

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	Color += verticalpoints[texturelocation.y + 0][0];
	Color += verticalpoints[texturelocation.y + 1][1];
	Color += verticalpoints[texturelocation.y + 2][2];
	Color += data * filter[3];
	Color += verticalpoints[texturelocation.y + 4][2];
	Color += verticalpoints[texturelocation.y + 5][1];
	Color += verticalpoints[texturelocation.y + 6][0];

	// Write the output to the output resource
	OutputMap[DispatchThreadID.xy] = Color;
}
