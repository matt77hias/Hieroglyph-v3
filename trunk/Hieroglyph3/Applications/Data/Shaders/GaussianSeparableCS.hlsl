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

	int3 texturelocation = DispatchThreadID - int3( 3, 0, 0 );

	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int x = 0; x < 7; x++ )
		Color += horizontalpoints[texturelocation.x + x] * filter[x];

    OutputMap[DispatchThreadID.xy] = Color;
}


// Declare enough shared memory for the padded group size
groupshared float4 verticalpoints[size_y];

// For the horizontal pass, use only a single row of threads
[numthreads(1, size_y, 1)]

void CSMAINY( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Load the current data from input texture
	float4 data = InputMap.Load( DispatchThreadID );

	// Stor the data into the GSM for the current thread
	verticalpoints[DispatchThreadID.y] = data;
	
	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	int3 texturelocation = DispatchThreadID - int3( 0, 3, 0 );

	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int y = 0; y < 7; y++ )
		Color += verticalpoints[texturelocation.y + y] * filter[y];

    OutputMap[DispatchThreadID.xy] = Color;
}
