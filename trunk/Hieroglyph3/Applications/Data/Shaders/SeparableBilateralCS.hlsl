//-----------------------------------------------------------------------------
// SeparableBilateralCS.hlsl
//
// This compute shader performs an approximated bilateral filter.
// The bilateral filter is not separable, but separating it produces acceptable
// results anyways.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//-----------------------------------------------------------------------------

Texture2D					DepthMap : register( t0 );
RWTexture2D<float>			AmbientOcclusionTarget : register( u0 );  

// Grid size - this should match your dispatch call!
#define gridsize_x 40
#define gridsize_y 30

// The total texture size
#define totalsize_x 640
#define totalsize_y 480

// Group size
#define size_x (totalsize_x / gridsize_x)
#define size_y (totalsize_y / gridsize_y)

// Declare enough shared memory for the padded group size
groupshared float2 horizontalpoints[totalsize_x];

// Declare one thread for each texel of the simulation group.  This includes
// one extra texel around the entire perimeter for padding.
[numthreads(totalsize_x, 1, 1)]

void CS_Horizontal( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int textureindex = DispatchThreadID.x + DispatchThreadID.y * totalsize_x;

	// Each thread will load its own depth/occlusion values
	float fCenterDepth = DepthMap.Load( DispatchThreadID ).w;
	float fCenterOcclusion = AmbientOcclusionTarget[DispatchThreadID.xy].x;
	
	// Then store them in the GSM for everyone to use
	horizontalpoints[GroupIndex].x = fCenterDepth;
	horizontalpoints[GroupIndex].y = fCenterOcclusion;

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Spatial kernel weights definition and range weighting sigma value
	const float avKernel[7] = { 0.004431f, 0.05402f, 0.2420f, 0.3990f, 0.2420f, 0.05402f, 0.004431f };
	const float rsigma = 0.0051f;

	float fBilateral = 0.0f;
	float fWeight = 0.0f;

	for ( int x = -3; x <= 3; x++ )
	{
		int location = GroupIndex + x;
		float fSampleDepth = horizontalpoints[location].x;
		float fSampleOcclusion = horizontalpoints[location].y;

		float fDelta = fCenterDepth - fSampleDepth;
		float fRange = exp( ( -1.0f * fDelta * fDelta ) / ( 2.0f * rsigma * rsigma ) );

		fBilateral += fSampleOcclusion * fRange * avKernel[x+3];
		fWeight += fRange * avKernel[x+3];
	}

	AmbientOcclusionTarget[DispatchThreadID.xy] = fBilateral / fWeight;
}


// Declare enough shared memory for the padded group size
groupshared float2 verticalpoints[totalsize_y];

// Declare one thread for each texel of the simulation group.  This includes
// one extra texel around the entire perimeter for padding.
[numthreads(1, totalsize_y, 1)]

void CS_Vertical( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int textureindex = DispatchThreadID.x + DispatchThreadID.y * totalsize_x;

	// Each thread will load its own depth/occlusion values
	float fCenterDepth = DepthMap.Load(DispatchThreadID).w;
	float fCenterOcclusion = AmbientOcclusionTarget[DispatchThreadID.xy].x;

	// Then store them in the GSM for everyone to use
	verticalpoints[GroupIndex].x = fCenterDepth;
	verticalpoints[GroupIndex].y = fCenterOcclusion;

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Spatial kernel weights definition and range weighting sigma value
	const float avKernel[7] = { 0.004431f, 0.05402f, 0.2420f, 0.3990f, 0.2420f, 0.05402f, 0.004431f };
	const float rsigma = 0.0051f;

	float fBilateral = 0.0f;
	float fWeight = 0.0f;

	for ( int y = -3; y <= 3; y++ )
	{
		int location = GroupIndex + y;
		float fSampleDepth = verticalpoints[location].x;
		float fSampleOcclusion = verticalpoints[location].y;

		float fDelta = fCenterDepth - fSampleDepth;
		float fRange = exp( ( -1.0f * fDelta * fDelta ) / ( 2.0f * rsigma * rsigma ) );

		fBilateral += fSampleOcclusion * fRange * avKernel[y+3];
		fWeight += fRange * avKernel[y+3];
	}

	AmbientOcclusionTarget[DispatchThreadID.xy] = fBilateral / fWeight;
}
