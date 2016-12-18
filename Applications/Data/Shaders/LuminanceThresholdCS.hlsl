//
// LuminanceThreshold.hlsl
//
// Copyright (C) Jason Zink 

// Declare the input and output resources
Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

cbuffer LuminanceThresholdParams
{
	float4 Thresholds;
};

// Group size
#define size_x 32
#define size_y 32

// Declare one thread for each texel of the current block size.
[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID;

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 color = InputMap.Load( texturelocation );

	float luminance = dot( color, float4( 0.2126, 0.7152, 0.0722, 0.0 ) );

	// Write the output to the output resource if the luminance exceeds the threshold
	if ( luminance < Thresholds.x ) {
		color = float4( 0.0, 0.0, 0.0, 0.0 );
	}

	OutputMap[DispatchThreadID.xy] = color;
}
