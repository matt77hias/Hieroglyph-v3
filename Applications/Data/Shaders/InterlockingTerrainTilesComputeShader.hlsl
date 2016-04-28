// -----------------------
// D E C L A R A T I O N S
// -----------------------

Texture2D				texHeightMap	: register( t0 );
RWTexture2D<float4>		bufferResults	: register( u0 );

// Compute Shader State
groupshared float		groupResults[16 * 16];
groupshared float4		plane;
groupshared float3		rawNormals[2][2];
groupshared float3		corners[2][2];


// -------------------------------
// S H A R E D   F U N C T I O N S
// -------------------------------

float4 CreatePlaneFromPointAndNormal(float3 n, float3 p)
{
	return float4(n,(-n.x*p.x - n.y*p.y - n.z*p.z));
}

float ComputeDistanceFromPlane(float4 plane, float3 position)
{
	return dot(plane.xyz,position) - plane.w;
}

// -----------------------------
// C O M P U T E   S H A D E R S
// -----------------------------

[numthreads(16, 16, 1)]
void csMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )
{   
	// Gid  = The xyz index as spun up by the actual application call
	//		  e.g. if ::Dispatch(16,16,1) is used then this will index into those bounds
	// DTid = Similar to above, but the global offset, so for [numthreads(16,16,1)] and
	//        ::Dispatch(16,16,1) this variable will range between 0-255,0-255,0
	// GTid = The offset within the [numthreads()] bounds e.g. 0-15,0-15,0
	// GI   = The flattened offset for this group e.g. 0-255 (16*16)
	
	// PHASE ONE
	// =========
	
	if(
		((GTid.x ==  0) && (GTid.y ==  0))
		||
		((GTid.x == 15) && (GTid.y ==  0))
		||
		((GTid.x ==  0) && (GTid.y == 15))
		||
		((GTid.x == 15) && (GTid.y == 15))
	  )
	{
		// This is a corner thread, so we want it to load
		// its value first
		groupResults[GI] = texHeightMap.Load( uint3( DTid.xy, 0 ) ).r;
		
		corners[GTid.x / 15][GTid.y / 15] = float3(GTid.x / 15, groupResults[GI], GTid.y / 15);
		
		// The above will unfairly bias based on the height ranges
		corners[GTid.x / 15][GTid.y / 15].x /= 64.0f;
		corners[GTid.x / 15][GTid.y / 15].z /= 64.0f;
	}

	// Block until all threads have finished reading
	GroupMemoryBarrierWithGroupSync();
	
	// PHASE TWO
	// =========
	if((GTid.x ==  0) && (GTid.y ==  0))
	{
		rawNormals[0][0] = normalize(cross
							(
								corners[0][1] - corners[0][0],
								corners[1][0] - corners[0][0]
							));
	}
	else if((GTid.x ==  15) && (GTid.y ==  0))
	{
		rawNormals[1][0] = normalize(cross
							(
								corners[0][0] - corners[1][0],
								corners[1][1] - corners[1][0]
							));
	}
	else if((GTid.x ==  0) && (GTid.y ==  15))
	{
		rawNormals[0][1] = normalize(cross
							(
								corners[1][1] - corners[0][1],
								corners[0][0] - corners[0][1]
							));
	}
	else if((GTid.x ==  15) && (GTid.y ==  15))
	{
		rawNormals[1][1] = normalize(cross
							(
								corners[1][0] - corners[1][1],
								corners[0][1] - corners[1][1]
							));
	}
	else
	{
		// This is just one of the other threads, so let it
		// load in its sample into shared memory
		groupResults[GI] = texHeightMap.Load( uint3( DTid.xy, 0 ) ).r;
	}

	// Block until all the data is ready
	GroupMemoryBarrierWithGroupSync();
	
	// PHASE THREE
	// ===========
	if(GI == 0)
	{
		// Let the first thread only determine the plane coefficients
		
		// First, decide on the average normal vector
		float3 n = normalize(rawNormals[0][0] + rawNormals[0][1] + rawNormals[1][0] + rawNormals[1][1]);
		
		// Second, decide the lowest point on which to base it
		float3 p = float3(0.0f,1e9f,0.0f);
		for(int i = 0; i < 2; ++i)
			for(int j = 0; j < 2; ++j)
				if(corners[i][j].y < p.y)
					p = corners[i][j];
		
		// Third, derive the plane from point+normal
		plane = CreatePlaneFromPointAndNormal(n,p);
	}

	GroupMemoryBarrierWithGroupSync();
	
	// PHASE FOUR
	// ==========
	
	// All threads now translate the raw height into the distance
	// from the base plane.
	groupResults[GI] = ComputeDistanceFromPlane(plane, float3((float)GTid.x / 15.0f, groupResults[GI], (float)GTid.y / 15.0f));

	GroupMemoryBarrierWithGroupSync();
	
	// PHASE FIVE
	// ==========
	
	if(GI == 0)
	{
		// Let the first thread compute the standard deviation for
		// this patch. The 'average' is really just going to be 0.0
		// as we want the deviation from the plane and any point on the
		// plane now has a 'height' of zero.
		float stddev = 0.0f;
		
		for(int i = 0; i < 16*16; ++i)
			stddev += pow(groupResults[i],2);
			
		stddev /= ((16.0f * 16.0f) - 1.0f);
		
		stddev = sqrt(stddev);
		
		// Then write the normal vector and standard deviation
		// to the output buffer for use by the Domain and Hull Shaders
		bufferResults[uint2(Gid.x, Gid.y)] = float4(plane.xyz, stddev);
	}
}