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
	//float2 tex		: TEXCOORD0;
	float3 normal	: TEXCOORD0;
	float3 eye		: TEXCOORD1;
};

struct PS_INPUT
{
	float4 position : SV_Position;
	float  z_value  : ZVALUE;
	//float2 tex      : TEXCOORD0;
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

	// First calculate the output position based on the paraboloid maps that we are
	// currently generating.

	//float direction = viewParms.z;		// this determines the front or back hemisphere

	OUT.position = mul( float4( IN.position, 1 ), WorldViewMatrix );	// transform vertex into the maps basis

	//OUT.position.z = OUT.position.z * direction;		// set z-values to forward or backward
	
	float L = length( OUT.position.xyz );				// determine the distance between (0,0,0) and the vertex
	OUT.position = OUT.position / L;					// divide the vertex position by the distance 
	
	OUT.z_value = OUT.position.z;						// remember which hemisphere the vertex is in
	//OUT.position.z = OUT.position.z + 1;				// add the reflected vector to find the normal vector

	//OUT.position.x = OUT.position.x / OUT.position.z;	// divide x coord by the new z-value
	//OUT.position.y = OUT.position.y / OUT.position.z;	// divide y coord by the new z-value

	OUT.position.z = L / 500;							// set a depth value for correct z-buffering
	OUT.position.w = 1;									// set w to 1 so there is no w divide


	// These are used to lookup this object's paraboloid maps' information
	float4 WorldPos		= mul( float4(IN.position, 1), WorldMatrix );		// find world space position
	float3 N			= normalize(mul(IN.normal, (float3x3)WorldMatrix));	// find world space normal
	float3 E			= normalize(WorldPos.xyz - ViewPosition.xyz);			// find world space eye vector

	OUT.normal			= N;									// output world space normal vector
	OUT.eye				= E;									// output world space eye vector

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


		//output.tex = input[order[i]].tex;
		output.z_value = input[order[i]].z_value * direction;
		output.normal = input[order[i]].normal;
		output.eye = input[order[i]].eye;
		output.rtindex = id;

        OutputStream.Append(output);
    }

	OutputStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 PSMAIN( PS_INPUT IN ) : SV_Target
{
    float4 OUT;

	clip( IN.z_value );

	float3 N = normalize( IN.normal );			// normalize input per-vertex normal vector
	float3 E = normalize( IN.eye );				// normalize input per-vertex eye vector
	float3 R = reflect( E, N );					// calculate the reflection vector
	
	R = mul( R, (float3x3)ParaboloidBasis );	// transform reflection vector to the maps basis
	
	// calculate the forward paraboloid map texture coordinates	
	float3 front;
	front.x = (R.x / (2*(1 + R.z))) + 0.5;
	front.y = 1-((R.y / (2*(1 + R.z))) + 0.5);
	front.z = 0.0f;
	
	// calculate the backward paraboloid map texture coordinates
	float3 back;
	back.x = (R.x / (2*(1 - R.z))) + 0.5;
	back.y = 1-((R.y / (2*(1 - R.z))) + 0.5);
	back.z = 1.0f;

	if ( R.z > 0 )
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, front );	// sample the front paraboloid map
	else
		OUT = ParaboloidTexture.Sample( ParaboloidSampler, back );	// sample the back paraboloid map

	OUT *= 0.8f;

	return OUT;
}
//-----------------------------------------------------------------------------

