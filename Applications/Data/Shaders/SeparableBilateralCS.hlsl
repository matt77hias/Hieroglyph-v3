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

// The thread group size
#define size_x 640
#define size_y 480

// Declare enough shared memory for the padded group size
groupshared float2 horizontalpoints[3+size_x+3];

// Declare one thread for each texel of the simulation group.  This includes
// one extra texel around the entire perimeter for padding.
[numthreads(size_x, 1, 1)]

void CS_Horizontal( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Each thread will load its own depth/occlusion values
	float fCenterDepth = DepthMap.Load( DispatchThreadID ).w;
	float fCenterOcclusion = AmbientOcclusionTarget[DispatchThreadID.xy].x;
	
	// Then store them in the GSM for everyone to use
	horizontalpoints[GroupIndex+3].x = fCenterDepth;
	horizontalpoints[GroupIndex+3].y = fCenterOcclusion;

	// Load the data to the left of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {
		horizontalpoints[0].x = DepthMap.Load(DispatchThreadID-int3(3,0,0)).w;
		horizontalpoints[0].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(3,0)];
		horizontalpoints[1].x = DepthMap.Load(DispatchThreadID-int3(2,0,0)).w;
		horizontalpoints[1].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(2,0)];
		horizontalpoints[2].x = DepthMap.Load(DispatchThreadID-int3(1,0,0)).w;
		horizontalpoints[2].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(1,0)];
	}

	// Load the data to the right of this line for the last few pixels to use.
	if ( GroupIndex == size_x-1 ) {
		horizontalpoints[3+size_x+0].x = DepthMap.Load(DispatchThreadID+int3(1,0,0)).w;
		horizontalpoints[3+size_x+0].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(1,0)];
		horizontalpoints[3+size_x+1].x = DepthMap.Load(DispatchThreadID+int3(2,0,0)).w;
		horizontalpoints[3+size_x+1].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(2,0)];
		horizontalpoints[3+size_x+2].x = DepthMap.Load(DispatchThreadID+int3(3,0,0)).w;
		horizontalpoints[3+size_x+2].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(3,0)];
	}

	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Spatial kernel weights definition and range weighting sigma value
	const float avKernel[7] = { 0.004431f, 0.05402f, 0.2420f, 0.3990f, 0.2420f, 0.05402f, 0.004431f };
	const float rsigma = 0.0051f;

	float fBilateral = 0.0f;
	float fWeight = 0.0f;

	for ( int x = 0; x < 7; x++ )
	{
		int location = GroupIndex + x;
		float fSampleDepth = horizontalpoints[location].x;
		float fSampleOcclusion = horizontalpoints[location].y;

		float fDelta = fCenterDepth - fSampleDepth;
		float fRange = exp( ( -1.0f * fDelta * fDelta ) / ( 2.0f * rsigma * rsigma ) );

		fBilateral += fSampleOcclusion * fRange * avKernel[x];
		fWeight += fRange * avKernel[x];
	}

	AmbientOcclusionTarget[DispatchThreadID.xy] = fBilateral / fWeight;
}


// Declare enough shared memory for the padded group size
groupshared float2 verticalpoints[3+size_y+3];

// Declare one thread for each texel of the simulation group.  This includes
// one extra texel around the entire perimeter for padding.
[numthreads(1, size_y, 1)]

void CS_Vertical( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Each thread will load its own depth/occlusion values
	float fCenterDepth = DepthMap.Load(DispatchThreadID).w;
	float fCenterOcclusion = AmbientOcclusionTarget[DispatchThreadID.xy].x;

	// Then store them in the GSM for everyone to use
	verticalpoints[GroupIndex+3].x = fCenterDepth;
	verticalpoints[GroupIndex+3].y = fCenterOcclusion;

	// Load the data to the left of this line for the first few pixels to use.
	if ( GroupIndex == 0 ) {
		verticalpoints[0].x = DepthMap.Load(DispatchThreadID-int3(0,3,0)).w;
		verticalpoints[0].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(0,3)];
		verticalpoints[1].x = DepthMap.Load(DispatchThreadID-int3(0,2,0)).w;
		verticalpoints[1].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(0,2)];
		verticalpoints[2].x = DepthMap.Load(DispatchThreadID-int3(0,1,0)).w;
		verticalpoints[2].y = AmbientOcclusionTarget[DispatchThreadID.xy-int2(0,1)];
	}

	// Load the data to the right of this line for the last few pixels to use.
	if ( GroupIndex == size_x-1 ) {
		verticalpoints[3+size_y+0].x = DepthMap.Load(DispatchThreadID+int3(0,1,0)).w;
		verticalpoints[3+size_y+0].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(0,1)];
		verticalpoints[3+size_y+1].x = DepthMap.Load(DispatchThreadID+int3(0,2,0)).w;
		verticalpoints[3+size_y+1].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(0,2)];
		verticalpoints[3+size_y+2].x = DepthMap.Load(DispatchThreadID+int3(0,3,0)).w;
		verticalpoints[3+size_y+2].y = AmbientOcclusionTarget[DispatchThreadID.xy+int2(0,3)];
	}


	// Synchronize all threads
	GroupMemoryBarrierWithGroupSync();

	// Spatial kernel weights definition and range weighting sigma value
	const float avKernel[7] = { 0.004431f, 0.05402f, 0.2420f, 0.3990f, 0.2420f, 0.05402f, 0.004431f };
	const float rsigma = 0.0051f;

	float fBilateral = 0.0f;
	float fWeight = 0.0f;

	for ( int y = 0; y < 7; y++ )
	{
		int location = GroupIndex + y;
		float fSampleDepth = verticalpoints[location].x;
		float fSampleOcclusion = verticalpoints[location].y;

		float fDelta = fCenterDepth - fSampleDepth;
		float fRange = exp( ( -1.0f * fDelta * fDelta ) / ( 2.0f * rsigma * rsigma ) );

		fBilateral += fSampleOcclusion * fRange * avKernel[y];
		fWeight += fRange * avKernel[y];
	}

	AmbientOcclusionTarget[DispatchThreadID.xy] = fBilateral / fWeight;
}
