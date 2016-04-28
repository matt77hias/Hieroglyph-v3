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
	float4 LightPositionTS;
};

cbuffer ObjectMaterialInfo
{
	//float4 Ka;
	float4 Kd;
	float4 Ks;
};

cbuffer PointLightInfo
{
	float4 LightPosition;
	float4 Ia;
	float4 Id;
	float4 Is;
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
	
	float stepLength = 1.0f/80.0f;
	float3 stepSize = direction * stepLength;
	float3 location = input.tex;
	float surfaceHit = 0.0f;

	float currTSDF = 0.0f;
	float prevTSDF = 0.0f;

	float3 surfacePosition = input.tex;


	for ( int i = 0; i < 80; i++ ) {
		
		prevTSDF = currTSDF;
		currTSDF = VolumeTexture.SampleGrad( LinearSampler, location, 1.0f, 1.0f ).x;

		if ( currTSDF > 0.0f ) {

			float TSDFdelta = currTSDF - prevTSDF;
			float TSDFintersect = currTSDF / TSDFdelta;
			surfacePosition = location - stepSize * TSDFintersect;
			
			surfaceHit = 1.0f;
			break;
		}

		location = location + stepSize;
	}

	// Calculate the gradient based normal vector using central differences

	float right = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3(  1.0f,  0.0f,  0.0f ) * stepLength, 1.0f, 1.0f ).x;
	float left  = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3( -1.0f,  0.0f,  0.0f ) * stepLength, 1.0f, 1.0f ).x;
	float up    = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3(  0.0f,  1.0f,  0.0f ) * stepLength, 1.0f, 1.0f ).x;
	float down  = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3(  0.0f, -1.0f,  0.0f ) * stepLength, 1.0f, 1.0f ).x;
	float front = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3(  0.0f,  0.0f,  1.0f ) * stepLength, 1.0f, 1.0f ).x;
	float back  = VolumeTexture.SampleGrad( LinearSampler, surfacePosition + float3(  0.0f,  0.0f, -1.0f ) * stepLength, 1.0f, 1.0f ).x;

	float3 surfaceNormal = normalize( float3( left-right, down-up, back-front ) ); // (2*stepLength);


	// Shade the point based on the calculated normal vector

	float3 P = surfacePosition;
	float3 N = surfaceNormal;
	float3 L = normalize( LightPositionTS.xyz - P.xyz );
	float3 V = normalize( CameraPositionTS.xyz - P.xyz );
	float3 H = normalize( L + V );

	float NdotL = dot( L, N );
	float4 ambientIntensity = Kd * Ia;
	float4 diffuseIntensity = Kd * Id * clamp( NdotL, 0.0f, 1.0f );
	float4 specularIntensity = Ks * Is * clamp( pow( abs( dot( H, N ) ), 64.0f ), 0.0f, 1.0f );

	float4 output = ambientIntensity;

	if ( NdotL > 0.0f ) {
		output += diffuseIntensity;
		output += specularIntensity;
	}

	output.a = 1.0f;



	// Multiply by the 'hit' value to ensure only rendering a portion of the implicit surface.

	return( output * surfaceHit );
}


