//--------------------------------------------------------------------------------
// dual_paraboloid_environmentmap_terrain.fx
//
//--------------------------------------------------------------------------------


cbuffer ParaboloidLookupParaboloidParams
{
	matrix WorldViewMatrix;
	matrix WorldMatrix;
	matrix ParaboloidBasis;
	float4 ViewPosition;
}
//-----------------------------------------------------------------------------
Texture2DArray  ParaboloidTexture : register( t0 );           
SamplerState    ParaboloidSampler : register( s0 );
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
};

struct GS_INPUT
{
	float4 position : SV_Position;
	float  z_value  : ZVALUE;
	float3 normal	: TEXCOORD0;
	float3 eye		: TEXCOORD1;
};

struct PS_INPUT
{
	float4 position : SV_Position;
	float  z_value  : ZVALUE;
	float3 normal	: TEXCOORD0;
	float3 eye		: TEXCOORD1;
	uint   rtindex  : SV_RenderTargetArrayIndex;
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Vertex Shader
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

	// Find the world space position of the vertex.
	float4 WorldPos		= mul( float4( IN.position, 1 ), WorldMatrix );

	// Find world space normal and eye vectors.
	OUT.normal			= normalize( mul( IN.normal, (float3x3)WorldMatrix ) );
	OUT.eye				= normalize( WorldPos.xyz - ViewPosition.xyz );

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

		// Propagate the normal and eye vectors.
		output.normal = input[order[i]].normal;
		output.eye = input[order[i]].eye;
		
		output.z_value = input[order[i]].z_value * direction;
		
		// Write the vertex to the output stream.
        OutputStream.Append(output);
    }

	OutputStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	clip( IN.z_value + 0.05f );

	// Normalize the input normal and eye vectors
	float3 N = normalize( IN.normal );
	float3 E = normalize( IN.eye );

	// Calculate the world space reflection vector, and then transform it to 
	// the paraboloid basis.
	float3 R = reflect( E, N );
	R = mul( R, (float3x3)ParaboloidBasis );
	
	// Calculate the forward paraboloid map texture coordinates, with z 
	// determining which paraboloid map to sample (front or back).
	float3 front;
	front.x = (R.x / (2*(1 + R.z))) + 0.5;
	front.y = 1-((R.y / (2*(1 + R.z))) + 0.5);
	front.z = 0.0f;
	
	// Calculate the backward paraboloid map texture coordinates, with z 
	// determining which paraboloid map to sample (front or back).
	float3 back;
	back.x = (R.x / (2*(1 - R.z))) + 0.5;
	back.y = 1-((R.y / (2*(1 - R.z))) + 0.5);
	back.z = 1.0f;

	// Sample the appropriate paraboloid map based on which direction 
	// the reflection vector is pointing.
	if ( R.z > 0 )
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, front );
	else
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, back );

	OUT *= 0.8f;

	return OUT;
}
//-----------------------------------------------------------------------------

