//
// InvertColorCS.hlsl
//
// Copyright (C) 2010  Jason Zink 

Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

// Group size
#define size_x 32
#define size_y 32


static const float filter[7][7] = {
	0.000020, 0.000236, 0.001081, 0.001769, 0.001081, 0.000236, 0.000020,
	0.000236, 0.002908, 0.013068, 0.021558, 0.013068, 0.002908, 0.000236,
	0.001081, 0.013068, 0.058581, 0.096587, 0.058581, 0.013068, 0.001081,
	0.001769, 0.021558, 0.096587, 0.159235, 0.096587, 0.021558, 0.001769,
	0.001081, 0.013068, 0.058581, 0.096587, 0.058581, 0.013068, 0.001081,
	0.000236, 0.002908, 0.013068, 0.021558, 0.013068, 0.002908, 0.000236,
	0.000020, 0.000236, 0.001081, 0.001769, 0.001081, 0.000236, 0.000020
};

// Declare one thread for each texel of the input texture.
[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int3 texturelocation = DispatchThreadID - int3( 3, 3, 0 );

	float4 Color = float4( 0.0, 0.0, 0.0, 0.0 );

	for ( int x = 0; x < 7; x++ )
		for ( int y = 0; y < 7; y++ )
			Color += InputMap.Load( texturelocation + int3(x,y,0) ) * filter[x][y];

    OutputMap[DispatchThreadID.xy] = Color;
}
