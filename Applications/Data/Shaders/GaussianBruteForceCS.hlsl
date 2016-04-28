//
// GaussianBruteForceCS.hlsl
//
// Copyright (C) 2010  Jason Zink 

// Declare the input and output resources
Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

// Group size
#define size_x 32
#define size_y 32

// Declare the filter kernel coefficients
static const float filter[7][7] = {
	0.000904706, 0.003157733, 0.00668492, 0.008583607, 0.00668492, 0.003157733, 0.000904706,
	0.003157733, 0.01102157, 0.023332663, 0.029959733, 0.023332663, 0.01102157, 0.003157733,
	0.00668492, 0.023332663, 0.049395249, 0.063424755, 0.049395249, 0.023332663, 0.00668492,
	0.008583607, 0.029959733, 0.063424755, 0.081438997, 0.063424755, 0.029959733, 0.008583607,
	0.00668492, 0.023332663, 0.049395249, 0.063424755, 0.049395249, 0.023332663, 0.00668492,
	0.003157733, 0.01102157, 0.023332663, 0.029959733, 0.023332663, 0.01102157, 0.003157733,
	0.000904706, 0.003157733, 0.00668492, 0.008583607, 0.00668492, 0.003157733, 0.000904706
};

// Declare one thread for each texel of the current block size.
[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	// Offset the texture location to the first sample location
	int3 texturelocation = DispatchThreadID - int3( 3, 3, 0 );

	// Initialize the output value to zero, then loop through the 
	// filter samples, apply them to the image samples, and sum
	// the results.
	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int x = 0; x < 7; x++ )
		for ( int y = 0; y < 7; y++ )
			Color += InputMap.Load( texturelocation + int3(x,y,0) ) * filter[x][y];

	// Write the output to the output resource
    OutputMap[DispatchThreadID.xy] = Color;
}
