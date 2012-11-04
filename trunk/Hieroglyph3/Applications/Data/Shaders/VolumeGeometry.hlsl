//--------------------------------------------------------------------------------
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture3D<float>	VolumeTexture : register( t0 );           
SamplerState		LinearSampler : register( s0 );

cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
};

cbuffer CameraData
{
	float4 CameraPositionTS;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float3 tex		: TEXCOORD0;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.tex = input.tex;

	return output;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{

	// Find the texture space view vector for this pixel.  This is done by taking the texture
	// space view position and subtracting the current texture coordinate.
	//float3 CameraPositionTS = float3( 10.0f, 10.0f, 10.0f );
	float3 direction = normalize( input.tex - CameraPositionTS.xyz );

	// Perform the sampling along the view vector until you max out the number of iterations
	// or you hit the surface.
	
	float3 stepSize = direction * 1.0f / 40.0f;
	float3 location = input.tex;
	float result = 0.0f;


	for ( int i = 0; i < 40; i++ ) {
		
		float sampledValue = VolumeTexture.SampleGrad( LinearSampler, location, 1.0f, 1.0f ).x;

		if ( sampledValue > 0.0f ) {
			result = 1.0f;
			break;
		}

		//location = clamp( location + stepSize, float3( 0.0f, 0.0f, 0.0f ), float3( 1.0f, 1.0f, 1.0f ) );
		location = location + stepSize;
	}

	//result = VolumeTexture.SampleGrad( LinearSampler, input.tex, 1.0f, 1.0f ).x;

	float4 sampledColor = float4( result, result, result, 1.0f );
	
	float4 mixedColor = sampledColor /* * input.color*/;
	
	return( mixedColor );
}
