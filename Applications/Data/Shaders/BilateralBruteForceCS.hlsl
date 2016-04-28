//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

	// Each thread will load its own depth/occlusion values
	float4 CenterColor = InputMap.Load( DispatchThreadID );

	// Range sigma value
	const float rsigma = 0.051f;

	float4 Color = 0.0f;
	float4 Weight = 0.0f;

	for ( int x = 0; x < 7; x++ )
	{
		for ( int y = 0; y < 7; y++ )
		{
			// Get the current sample
			float4 SampleColor = InputMap.Load( texturelocation + int3( x, y, 0 ) );

			// Find the delta, and use that to calculate the range weighting
			float4 Delta = CenterColor - SampleColor;
			float4 Range = exp( ( -1.0f * Delta * Delta ) / ( 2.0f * rsigma * rsigma ) );

			// Sum both the color result and the total weighting used
			Color += SampleColor * Range * filter[x][y];
			Weight += Range * filter[x][y];
		}
	}

	// Store the renormalized result to the output resource
	OutputMap[DispatchThreadID.xy] = Color / Weight;
}