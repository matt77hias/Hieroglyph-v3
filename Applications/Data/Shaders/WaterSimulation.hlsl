//-----------------------------------------------------------------------------
// NSE_Solver_CS_Optimized_Grid.hlsl
//
// The NSE Solver takes as input a shader resource view representing the 
// current state of the system in a structured buffer, including the current 
// flow and height values for each point in the simulation.  The calculated 
// output is stored in a read/write structured buffer bound to the compute 
// shader with an unordered access view to allow all of the threads to write 
// their output simultaneously.
//
// Copyright (C) 2003-2010 Jason Zink.  All rights reserved.
//-----------------------------------------------------------------------------

// Declare the structure that represents one fluid column's state
struct GridPoint
{
    float  Height;
    float4 Flow;
};

// Declare the input and output resources
RWStructuredBuffer<GridPoint> NewWaterState		: register( u0 );
StructuredBuffer<GridPoint>   CurrentWaterState : register( t0 );

// Declare the constant buffer parameters
cbuffer TimeParameters
{
	float4 TimeFactors;
	float4 DispatchSize;
};

// Simulation Group Size
#define size_x 16
#define size_y 16

// Group size with the extra perimeter
#define padded_x (1 + size_x + 1)
#define padded_y (1 + size_y + 1)

// Declare enough shared memory for the padded group size
groupshared GridPoint loadedpoints[padded_x * padded_y];

// Declare one thread for each texel of the simulation group.  This includes
// one extra texel around the entire perimeter for padding.
[numthreads(padded_x, padded_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Grid size - this matches your dispatch call size!
	int gridsize_x = DispatchSize.x;
	int gridsize_y = DispatchSize.y;

	// The total texture size
	int totalsize_x = DispatchSize.z; 
	int totalsize_y = DispatchSize.w; 
	
	// 1 1 1 1 1 1 1 1 1 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 0 0 0 0 0 0 0 0 1
	// 1 1 1 1 1 1 1 1 1 1


	// Perform all texture accesses here and load the group shared memory with last frame's height and flow.
	// These parameters are initialized to zero to account for 'out of bounds' texels.
	loadedpoints[GroupIndex].Height = 0.0f;
	loadedpoints[GroupIndex].Flow = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Given your GroupThreadID and the GroupID, calculate the thread's location in the buffer.
	int3 location = int3( 0, 0, 0 );
	location.x = GroupID.x * size_x + ( GroupThreadID.x - 1 );
	location.y = GroupID.y * size_y + ( GroupThreadID.y - 1 );
	int textureindex = location.x + location.y * totalsize_x;

	// Test the texture location here, and if within the texture then load the data
	loadedpoints[GroupIndex] = CurrentWaterState[textureindex];

	// Synchronize all threads before moving on to ensure everyone has loaded their state
	// into the group shared memory.
	GroupMemoryBarrierWithGroupSync();

	//----------------------------------------
	// Update all flow calculations
	//
	//                   o--o x
	//                  /|\
	//                 / | \
	//                o  o  o
	//               w   z   y

	// Initialize the flow variable to the last frames flow values
	float4 NewFlow = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Check for 'not' right edge
	if ( ( GroupThreadID.x < padded_x - 1 ) && ( location.x < totalsize_x - 1 ) )
	{
		NewFlow.x = ( loadedpoints[GroupIndex+1].Height - loadedpoints[GroupIndex].Height );

		// Check for 'not' bottom edge
		if ( ( GroupThreadID.y < padded_y - 1 ) && ( location.y < totalsize_y - 1 ) )
		{
			NewFlow.y = ( loadedpoints[(GroupIndex+1) + padded_x].Height - loadedpoints[GroupIndex].Height );
		}
	}

	// Check for 'not' bottom edge
	if ( ( GroupThreadID.y < padded_y - 1 ) && ( location.y < totalsize_y - 1 ) )
	{
		NewFlow.z = ( loadedpoints[GroupIndex+padded_x].Height - loadedpoints[GroupIndex].Height );

		// Check for 'not' left edge
		if ( ( GroupThreadID.x > 0 ) && ( location.x > 0 ) )
		{
			NewFlow.w = ( loadedpoints[GroupIndex + padded_x - 1].Height - loadedpoints[GroupIndex].Height );
		}
	}

	const float TIME_STEP = 0.05f;
	const float PIPE_AREA = 0.0001f;
	const float GRAVITATION = 10.0f;
	const float PIPE_LENGTH = 0.2f;
	const float FLUID_DENSITY = 1.0f;
	const float COLUMN_AREA = 0.05f;
	const float DAMPING_FACTOR = 0.9995f;
	
	float fAccelFactor = ( min( TimeFactors.x, TIME_STEP ) * PIPE_AREA * GRAVITATION ) / ( PIPE_LENGTH * COLUMN_AREA );
	
	// Calculate the new flow, and add in the previous flow value as well.  The damping factor
	// degrades the amount of inertia in the system.
	NewFlow = ( NewFlow * fAccelFactor + loadedpoints[GroupIndex].Flow ) * DAMPING_FACTOR;

	// Store the updated flow value in the group shared memory for other threads to access
	loadedpoints[GroupIndex].Flow = NewFlow;

	// Synchronize all threads before moving on
	GroupMemoryBarrierWithGroupSync();

	//----------------------------------------

	// Calculate the new height for each column, then store the height and modified flow values.
	// The updated height values are now stored in the loadedheights shared memory.

	// Out of the current column...
	loadedpoints[GroupIndex].Height = loadedpoints[GroupIndex].Height + NewFlow.x + NewFlow.y + NewFlow.z + NewFlow.w;

	// From left columns
	loadedpoints[GroupIndex].Height = loadedpoints[GroupIndex].Height - loadedpoints[GroupIndex-1].Flow.x;

	// From upper left columns
	loadedpoints[GroupIndex].Height = loadedpoints[GroupIndex].Height - loadedpoints[GroupIndex-padded_x-1].Flow.y;

	// From top columns
	loadedpoints[GroupIndex].Height = loadedpoints[GroupIndex].Height - loadedpoints[GroupIndex-padded_x].Flow.z;

	// From top right columns
	loadedpoints[GroupIndex].Height = loadedpoints[GroupIndex].Height - loadedpoints[GroupIndex-padded_x+1].Flow.w;

	// Finally store the updated height and flow values only for the threads within the
	// actual group (i.e. excluding the perimeter).  Otherwise there would be a double
	// calculation for each perimeter texel, making the simulation inaccurate!
	if ( ( GroupThreadID.x > 0 ) && ( GroupThreadID.x < padded_x - 1 ) && ( GroupThreadID.y > 0 ) && ( GroupThreadID.y < padded_y - 1 ) )
	{
		NewWaterState[textureindex] = loadedpoints[GroupIndex];
	}

}
