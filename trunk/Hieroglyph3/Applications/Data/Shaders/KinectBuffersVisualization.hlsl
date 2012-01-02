//--------------------------------------------------------------------------------
// KinectBuffersVisualization.hlsl
//
// This set of shaders is used to produce a 3D visualization of the Kinect depth
// buffer.  The geometry is passed in as a 1-to-1 height map with 1 vertex for 
// each depth texel.  A normal vector is calculated in the vertex shader after the
// appropriate height is determined.  This is used to generate a slight lighting
// effect to exaggerate the produced geometry.
//
// The triangle layout is static and provided in an index buffer, with invalid or 
// unnecessary triangles being culled out in the geometry shader.  The pixel 
// shader simply passes on the color information to the output merger.
//
// Copyright (C) 2012 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D<float>       KinectDepthBuffer;
Texture2D<float4>       KinectColorBuffer;

cbuffer Transforms
{
	matrix WorldViewProjMatrix;	
};


struct VS_INPUT
{
	float3 position : POSITION;
	float2 coords : TEXCOORDS;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float  height : HEIGHT;
};

float3 ComputeNormal( int2 coords ) {
	int2 o00 = coords + int2( -1, -1 );
	int2 o10 = coords + int2(  0, -1 );
	int2 o20 = coords + int2(  1, -1 );
 
	int2 o01 = coords + int2( -1,  0 );
	int2 o21 = coords + int2(  1,  0 );
 
	int2 o02 = coords + int2( -1,  1 );
	int2 o12 = coords + int2(  0,  1 );
	int2 o22 = coords + int2(  1,  1 );
 
	// Use of the sobel filter requires the eight samples
	// surrounding the current pixel:
	float h00 = KinectDepthBuffer[ o00 ];
	float h10 = KinectDepthBuffer[ o10 ];
	float h20 = KinectDepthBuffer[ o20 ];
 
	float h01 = KinectDepthBuffer[ o01 ];
	float h21 = KinectDepthBuffer[ o21 ];
 
	float h02 = KinectDepthBuffer[ o02 ];
	float h12 = KinectDepthBuffer[ o12 ];
	float h22 = KinectDepthBuffer[ o22 ];
 
	// Evaluate the Sobel filters
	float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
	float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;
 
	// Generate the missing Z
	float Gz = 0.5f * sqrt( 1.0f - Gx * Gx - Gy * Gy );
 
	// Make sure the returned normal is of unit length
	return normalize( float3( 2.0f * Gx, 2.0f * Gy, Gz ) );
}

VS_OUTPUT VSMAIN( in VS_INPUT v)
{
	VS_OUTPUT o;

	float fHeight = KinectDepthBuffer[ int2( v.coords.x, 240-v.coords.y ) ] * 500.0f;

	float3 normal = ComputeNormal( int2( v.coords.x, 240-v.coords.y ) );

	float diffuse = max( dot( normal, normalize(float3( 1.0f, 0.0f, 1.0f )) ), 0.0f ); 
	

	// Transform the new position to clipspace.
	float4 SampledPosition = float4( v.position.x, 1.0f-fHeight, v.position.z, 1.0f );
	SampledPosition.xyz = SampledPosition.xyz * 0.1f;

	o.position = mul( SampledPosition, WorldViewProjMatrix );

	o.color = diffuse * lerp( float4(1.0f, 1.0f, 0.0f, 1.0f), float4(0.2f, 0.0f, 0.0f, 1.0f), diffuse*2-1.0f );
	if ( fHeight == 0.0f ) {
		o.color = float4( 0.0f, 0.0f, 0.0f, 0.0f );
	}

	o.height = fHeight;

//	o.color = float4( 0.0f, 1.0f, 0.0f, 1.0f );

	return o;
}

[maxvertexcount(3)]
void GSMAIN( triangle VS_OUTPUT input[3],
             inout TriangleStream<VS_OUTPUT> OutputStream )
{
	float minHeight = min( input[0].height, input[1].height );
	minHeight = min( minHeight, input[2].height );

	float maxLength = max( length( abs(input[0].height - input[1].height) ), length( abs(input[1].height - input[2].height) ) );
	maxLength = max( maxLength, length( abs(input[2].height - input[0].height) ) );

	if (( minHeight > 0.0f ) && ( maxLength < 20.0f ))  {
		for ( int i = 0; i < 3; i++ ) {
		        OutputStream.Append(input[i]);
		}
	}

	OutputStream.RestartStrip();
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	
//	float4 vValues = KinectColorBuffer.Sample( LinearSampler, input.tex );
	
	float4 color = input.color;
	return( color );
}

