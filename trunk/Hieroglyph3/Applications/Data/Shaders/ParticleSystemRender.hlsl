//--------------------------------------------------------------------------------
// PhongShading.hlsl
//
// This set of shaders implements the most basic phong shading.
//
// Copyright (C) 2010 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Resources
//--------------------------------------------------------------------------------

cbuffer Transforms
{
	matrix WorldViewMatrix;	
	matrix ProjMatrix;
};

static const float4 g_positions[4] =
{
    float4( -1, 1, 0, 0 ),
    float4( 1, 1, 0, 0 ),
    float4( -1, -1, 0, 0 ),
    float4( 1, -1, 0, 0 ),
};

static const float2 g_texcoords[4] = 
{ 
    float2( 0, 1 ), 
    float2( 1, 1 ),
    float2( 0, 0 ),
    float2( 1, 0 ),
};


struct Particle
{
    float3 position;
	float3 direction;
    float4 status;
};

StructuredBuffer<Particle> SimulationState;
Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );

//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	uint vertexid			: SV_VertexID;
};
//--------------------------------------------------------------------------------
struct GS_INPUT
{
    float3 position			: Position;
};
//--------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 position			: SV_Position;
	float2 texcoords		: TEXCOORD0;
	float distance			: Distance;
};
//--------------------------------------------------------------------------------
GS_INPUT VSMAIN( in VS_INPUT input )
{
	GS_INPUT output;
	
	//output.position.xyz = (float3)input.vertexid;
	output.position.xyz = SimulationState[input.vertexid].position;

	return output;
}
//--------------------------------------------------------------------------------
[maxvertexcount(4)]
void GSMAIN( point GS_INPUT input[1], inout TriangleStream<PS_INPUT> SpriteStream )
{
	PS_INPUT output;

	float dist = length( input[0].position ) / 100.0f;

	// Transform to view space
	float4 viewposition = mul( float4( input[0].position, 1.0f ), WorldViewMatrix );

    // Emit two new triangles
    for ( int i = 0; i < 4; i++ )
    {
		// Transform to clip space
		output.position = mul( viewposition + g_positions[i], ProjMatrix );
		output.texcoords = g_texcoords[i];
		output.distance = dist;

        SpriteStream.Append(output);
    }

	SpriteStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in PS_INPUT input ) : SV_Target
{
	float4 color = float4( 0.0f, input.distance, 0.0f, 1.0f );
	//float4 vValues = ColorTexture.Sample( LinearSampler, input.tex );

	return( color );
}
//--------------------------------------------------------------------------------
