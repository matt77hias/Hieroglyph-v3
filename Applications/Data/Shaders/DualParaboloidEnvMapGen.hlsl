//--------------------------------------------------------------------------------
// DualParaboloidEnvMapGen.hlsl
//
// This shader is used to render a simple textured, non-reflective object into a 
// paraboloid map.
//--------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
cbuffer ParaboloidTransforms
{
	matrix WorldViewMatrix;
};

Texture2D       ColorTexture : register( t0 );           
SamplerState    LinearSampler : register( s0 );

//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float2 tex		: TEXCOORDS0;
};

struct GS_INPUT
{
	float4 position : SV_Position;
	float2 tex		: TEXCOORD0;
	float  z_value  : ZVALUE;
};

struct PS_INPUT
{
	float4 position : SV_Position;
	float2 tex      : TEXCOORD0;
	float  z_value  : ZVALUE;
	uint   rtindex  : SV_RenderTargetArrayIndex;
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GS_INPUT VSMAIN( VS_INPUT IN )
{
    GS_INPUT OUT;

	// Transform the vertex to be relative to the paraboloid's basis. 
	OUT.position = mul( float4( IN.position, 1 ), WorldViewMatrix );

	// Determine the distance between the paraboloid origin and the vertex.
	float L = length( OUT.position.xyz );

	// Normalize the vector to the vertex
	OUT.position = OUT.position / L;
	
	// Save the z-component of the normalized vector
	OUT.z_value = OUT.position.z;

	// Store the distance to the vertex for use in the depth buffer.
	OUT.position.z = L / 500;

	// Set w to 1 since we aren't doing any perspective distortion.
	OUT.position.w = 1;

	// Pass through texture coordinates
	OUT.tex	= IN.tex;		

	return OUT;
}
//-----------------------------------------------------------------------------
[maxvertexcount(3)]
[instance(2)]
void GSMAIN( triangle GS_INPUT input[3],
			 uint id : SV_GSInstanceID,
             inout TriangleStream<PS_INPUT> OutputStream )
{
	PS_INPUT output;

	// Initialize the vertex order and the direction of the paraboloid.
	uint3 order = uint3( 0, 1, 2 );
	float direction = 1.0f;

	// Check to see which copy of the primitive this is.  If it is 0, then it
	// is considered the front facing paraboloid.  If it is 1, then it is
	// considered the back facing paraboloid.  For back facing, we reverse
	// the output vertex winding order.
	if ( id == 1 )
	{
		order.xyz = order.xzy;
		direction = -1.0f;
	}

    // Emit three vertices for one complete triangle.
    for ( int i = 0; i < 3; i++ )
    {
		// Create a projection factor, which determines which half space 
		// will be considered positive and also adds the viewing vector
		// which is (0,0,1) and hence can only be added to the z-component.
		float projFactor = input[order[i]].z_value * direction + 1.0f;
		output.position.x = input[order[i]].position.x / projFactor;
		output.position.y = input[order[i]].position.y / projFactor;
		output.position.z = input[order[i]].position.z;
		output.position.w = 1.0f;

		// Simply use the geometry shader instance as the render target
		// index for this primitive.
		output.rtindex = id;

		// Pass through the texture coordinates to the pixel shader
		output.tex = input[order[i]].tex;

		output.z_value = input[order[i]].z_value * direction;

		// Write the vertex to the output stream.
        OutputStream.Append(output);
    }

	OutputStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	clip( IN.z_value + 0.05f );

	// Sample the texture to find the appropriate color value
	OUT = ColorTexture.Sample( LinearSampler, IN.tex );
	
    return OUT;
}
//-----------------------------------------------------------------------------
