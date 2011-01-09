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

	float direction = 1.0f;//viewParms.z;		// this determines the front or back hemisphere

	OUT.position = mul( float4( IN.position, 1 ), WorldViewMatrix );	// transform vertex into the maps basis
	//OUT.position = OUT.position / OUT.position.w;		// divide by w to normalize

	//OUT.position.z = OUT.position.z * direction;		// set z-values to forward or backward
	
	float L = length( OUT.position.xyz );				// determine the distance between (0,0,0) and the vertex
	OUT.position = OUT.position / L;					// divide the vertex position by the distance 
	
	OUT.z_value = OUT.position.z;						// remember which hemisphere the vertex is in
	//OUT.position.z = OUT.position.z + 1;				// add the reflected vector to find the normal vector

	//OUT.position.x = OUT.position.x / OUT.position.z;	// divide x coord by the new z-value
	//OUT.position.y = OUT.position.y / OUT.position.z;	// divide y coord by the new z-value

	OUT.position.z = L / 500;							// set a depth value for correct z-buffering
	OUT.position.w = 1;									// set w to 1 so there is no w divide

	OUT.tex	= IN.tex;		// pass through texcoords set 0

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

	uint3 order = uint3( 0, 1, 2 );
	float direction = 1.0f;

	// Transform to view space - need to change this to use a matrix array!
	if ( id == 1 )
	{
		order.xyz = order.xzy;
		direction = -1.0f;
	}

    // Emit two new triangles
    for ( int i = 0; i < 3; i++ )
    {
		float projFactor = input[order[i]].z_value * direction + 1.0f;
		output.position.x = input[order[i]].position.x / projFactor;
		output.position.y = input[order[i]].position.y / projFactor;
		output.position.z = input[order[i]].position.z;
		output.position.w = 1.0f;
		//OUT.position.z = OUT.position.z + 1;				// add the reflected vector to find the normal vector

		//OUT.position.x = OUT.position.x / OUT.position.z;	// divide x coord by the new z-value
		//OUT.position.y = OUT.position.y / OUT.position.z;	// divide y coord by the new z-value


		// Transform to clip space
		//output.position = input[order[i]].position;
		//output.position.z = output.position.z * direction;
		output.tex = input[order[i]].tex;
		output.z_value = input[order[i]].z_value * direction;
		output.rtindex = id;

        OutputStream.Append(output);
    }

	OutputStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	clip( IN.z_value + 0.05f );

	OUT = ColorTexture.Sample( LinearSampler, IN.tex );
	
    return OUT;
}
//-----------------------------------------------------------------------------
