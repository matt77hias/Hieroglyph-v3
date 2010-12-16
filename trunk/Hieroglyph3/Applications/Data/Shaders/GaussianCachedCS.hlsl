//
// InvertColorCS.hlsl
//
// Copyright (C) 2010  Jason Zink 

Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

// Group size
#define size_x 640
#define size_y 480


static const float filter[7] = {
	0.001769, 0.021558, 0.096587, 0.159235, 0.096587, 0.021558, 0.001769
};

// Declare enough shared memory for the padded group size
groupshared float4 horizontalpoints[size_x][3];

// For the horizontal pass, use only a single row of threads
[numthreads(size_x, 1, 1)]

void CSMAINX( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	horizontalpoints[DispatchThreadID.x][0] = data * filter[0];
	horizontalpoints[DispatchThreadID.x][1] = data * filter[1];
	horizontalpoints[DispatchThreadID.x][2] = data * filter[2];
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	
	int3 texturelocation = DispatchThreadID - int3( 3, 0, 0 );
	
	Color += horizontalpoints[texturelocation.x + 0][0];
	Color += horizontalpoints[texturelocation.x + 1][1];
	Color += horizontalpoints[texturelocation.x + 2][2];
	Color += data * filter[3];
	Color += horizontalpoints[texturelocation.x + 4][2];
	Color += horizontalpoints[texturelocation.x + 5][1];
	Color += horizontalpoints[texturelocation.x + 6][0];

    OutputMap[DispatchThreadID.xy] = Color;
}


// Declare enough shared memory for the padded group size
groupshared float4 verticalpoints[size_y][3];

// For the horizontal pass, use only a single row of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[DispatchThreadID.y][0] = data * filter[0];
	verticalpoints[DispatchThreadID.y][1] = data * filter[1];
	verticalpoints[DispatchThreadID.y][2] = data * filter[2];
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	
	int3 texturelocation = DispatchThreadID - int3( 0, 3, 0 );
	
	Color += verticalpoints[texturelocation.y + 0][0];
	Color += verticalpoints[texturelocation.y + 1][1];
	Color += verticalpoints[texturelocation.y + 2][2];
	Color += data * filter[3];
	Color += verticalpoints[texturelocation.y + 4][2];
	Color += verticalpoints[texturelocation.y + 5][1];
	Color += verticalpoints[texturelocation.y + 6][0];

    OutputMap[DispatchThreadID.xy] = Color;
}
