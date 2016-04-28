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

cbuffer ParticleRenderParameters
{
	float4 EmitterLocation;
	float4 ConsumerLocation;
};

static const float scale = 0.5f;

static const float4 g_positions[4] =
{
    float4( -scale, scale, 0, 0 ),
    float4( scale, scale, 0, 0 ),
    float4( -scale, -scale, 0, 0 ),
    float4( scale, -scale, 0, 0 ),
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
	float  time;
};

StructuredBuffer<Particle> SimulationState;
Texture2D       ParticleTexture : register( t0 );           
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
	float4 color			: Color;
};
//--------------------------------------------------------------------------------
GS_INPUT VSMAIN( in VS_INPUT input )
{
	GS_INPUT output;
	
	output.position.xyz = SimulationState[input.vertexid].position;

	return output;
}
//--------------------------------------------------------------------------------
[maxvertexcount(4)]
void GSMAIN( point GS_INPUT input[1], inout TriangleStream<PS_INPUT> SpriteStream )
{
	PS_INPUT output;

	float dist = saturate( length( input[0].position - ConsumerLocation.xyz ) / 100.0f );
	//float4 color = float4( 0.2f, 1.0f, 0.2f, 0.0f ) * dist + float4( 1.0f, 0.1f, 0.1f, 0.0f ) * ( 1.0f - dist ); 
	//float4 color = float4( 0.2f, 1.0f, 1.0f, 0.0f ) * dist + float4( 1.0f, 0.1f, 0.1f, 0.0f ) * ( 1.0f - dist ); 
	float4 color = float4( 0.2f, 0.2f, 1.0f, 0.0f ) * dist + float4( 1.0f, 0.1f, 0.1f, 0.0f ) * ( 1.0f - dist ); 

	// Transform to view space
	float4 viewposition = mul( float4( input[0].position, 1.0f ), WorldViewMatrix );

    // Emit two new triangles
    for ( int i = 0; i < 4; i++ )
    {
		// Transform to clip space
		output.position = mul( viewposition + g_positions[i], ProjMatrix );
		output.texcoords = g_texcoords[i];
		output.color = color;

        SpriteStream.Append(output);
    }

	SpriteStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in PS_INPUT input ) : SV_Target
{
	float4 color = ParticleTexture.Sample( LinearSampler, input.texcoords );
	color = color * input.color;

	return( color );
}
//--------------------------------------------------------------------------------
