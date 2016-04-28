//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Particle
{
    float3 position;
	float3 velocity;
	float  time;
};

AppendStructuredBuffer<Particle>	NewSimulationState : register( u0 );
ConsumeStructuredBuffer<Particle>   CurrentSimulationState  : register( u1 );

cbuffer SimulationParameters
{
	float4 TimeFactors;
	float4 EmitterLocation;
	float4 ConsumerLocation;
};

cbuffer ParticleCount
{
	uint4 NumParticles;
};

static const float G = 10.0f;
static const float m1 = 10.0f;
static const float m2 = 500.0f;
static const float m1m2 = m1 * m2;
static const float eventHorizon = 5.0f;

[numthreads( 512, 1, 1)]

void CSMAIN( uint3 DispatchThreadID : SV_DispatchThreadID )
{
	// Check for if this thread should run or not.
	uint myID = DispatchThreadID.x + DispatchThreadID.y * 512 + DispatchThreadID.z * 512 * 512;

	if ( myID < NumParticles.x )
	{
		// Get the current particle
		Particle p = CurrentSimulationState.Consume();

		// Calculate the current gravitational force applied to it
		float3 d = ConsumerLocation.xyz - p.position;
		float r = length( d );
		float3 Force = ( G * m1m2 / (r*r) ) * normalize( d );

		// Calculate the new velocity, accounting for the acceleration from
		// the gravitational force over the current time step.
		p.velocity = p.velocity + ( Force / m1 ) * TimeFactors.x;

		// Calculate the new position, accounting for the new velocity value
		// over the current time step.
		p.position += p.velocity * TimeFactors.x;

		// Update the life time left for the particle.
		p.time = p.time + TimeFactors.x;

		// Test to see how close the particle is to the black hole, and 
		// don't pass it to the output list if it is too close.
		if ( r > eventHorizon )
		{
			if ( p.time < 30.0f )
			{
				NewSimulationState.Append( p );
			}
		}
	}
}
