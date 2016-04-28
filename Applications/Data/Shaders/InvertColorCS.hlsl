//
// InvertColorCS.hlsl
//
// Copyright (C) 2010  Jason Zink 

Texture2D<float4>		InputMap : register( t0 );           
RWTexture2D<float4>		OutputMap : register( u0 );

// Group size
#define size_x 20
#define size_y 20

// Declare one thread for each texel of the input texture.
[numthreads(size_x, size_y, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	int3 texturelocation = int3( 0, 0, 0 );
	texturelocation.x = GroupID.x * size_x + GroupThreadID.x;
	texturelocation.y = GroupID.y * size_y + GroupThreadID.y;

	float4 Color = InputMap.Load(texturelocation);

    OutputMap[texturelocation.xy] = float4( 1.0f, 1.0f, 1.0f, 1.0f ) - Color;
}
