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

cbuffer DispatchParams
{
	float4 DispatchSize;
};


[numthreads( 512, 1, 1)]

void CSMAIN( uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex )
{
	Particle p = CurrentSimulationState.Consume();

	p.position += p.direction * TimeFactors.x;

	NewSimulationState.Append( p );
}
