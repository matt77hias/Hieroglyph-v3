//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Particle
{
    float3 position;
	float3 direction;
    float4 status;
};

AppendStructuredBuffer<Particle>	NewSimulationState : register( u0 );
ConsumeStructuredBuffer<Particle>   CurrentSimulationState  : register( u1 );

cbuffer TimeParameters
{
	float4 TimeFactors;
};

cbuffer ParticleParameters
{
	float4 EmitterLocation;
	float4 ConsumerLocation;
};

cbuffer DispatchParams
{
	float4 DispatchSize;
};
static const float3 randomDirection[5] =
{
	float3( 0.0f, 1.0f, 0.0f ),
	float3( 1.0f, 1.0f, 0.0f ),
	float3( 0.0f, 1.0f, 1.0f ),
	float3( 1.0f, 1.0f, 1.0f ),
	float3( 0.5f, 1.0f, 0.8f )
};


[numthreads( 512, 1, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	Particle p = CurrentSimulationState.Consume();

	p.direction += ( ConsumerLocation.xyz - p.position ) * 0.1f * TimeFactors.x;

	p.position += p.direction * TimeFactors.x;

	float dist = length( p.position - ConsumerLocation.xyz );

	if ( dist < 10.0f )
	{
		p.position = EmitterLocation.xyz;
		//p.direction = -p.direction.yzx * 0.2f;
		float fx = fmod( TimeFactors.z, 5.0f ) * 5.0f;
		p.direction = randomDirection[(uint)fx] * 5.0f + float3( 0.0f, 10.0f, 0.0f );
	}

	NewSimulationState.Append( p );
}
