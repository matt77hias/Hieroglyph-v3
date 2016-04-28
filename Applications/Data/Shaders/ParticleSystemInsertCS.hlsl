//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Particle
{
    float3 position;
	float3 direction;
	float  time;
};

AppendStructuredBuffer<Particle>	CurrentSimulationState : register( u0 );

cbuffer ParticleInsertParameters
{
	float4 EmitterLocation;
	float4 RandomVector;
};

static const float3 direction[8] =
{
	normalize( float3(  1.0f,  1.0f,  1.0f ) ),
	normalize( float3( -1.0f,  1.0f,  1.0f ) ),
	normalize( float3( -1.0f, -1.0f,  1.0f ) ),
	normalize( float3(  1.0f, -1.0f,  1.0f ) ),
	normalize( float3(  1.0f,  1.0f, -1.0f ) ),
	normalize( float3( -1.0f,  1.0f, -1.0f ) ),
	normalize( float3( -1.0f, -1.0f, -1.0f ) ),
	normalize( float3(  1.0f, -1.0f, -1.0f ) )
};


[numthreads( 8, 1, 1)]

void CSMAIN( uint3 GroupThreadID : SV_GroupThreadID )
{
	Particle p;

	// Initialize position to the current emitter location
	p.position = EmitterLocation.xyz;

	// Initialize direction to a randomly reflected vector
	p.direction = reflect( direction[GroupThreadID.x], RandomVector.xyz ) * 5.0f;

	// Initialize the lifetime of the particle in seconds
	p.time = 0.0f;

	// Append the new particle to the output buffer
	CurrentSimulationState.Append( p );
}
