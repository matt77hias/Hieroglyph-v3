//--------------------------------------------------------------------------------
// Resources
//--------------------------------------------------------------------------------
cbuffer SkinningTransforms
{
	matrix WorldMatrix;	
	matrix ViewProjMatrix;
	matrix SkinMatrices[6];
	matrix SkinNormalMatrices[6];
};

cbuffer LightParameters
{
	float3 LightPositionWS;
	float4 LightColor;
};

Texture2D       ColorTexture : register( t0 );           
Texture2D       HeightTexture : register( t1 );           
SamplerState    LinearSampler : register( s0 );


//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3	position 		: POSITION;
	int4	bone			: BONEIDS;
	float4	weights			: BONEWEIGHTS;
	float3  normal			: NORMAL;
	float2  tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 position			: SV_Position;
	float3 normal			: NORMAL;
	float3 light			: LIGHT;
	float2 tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
struct HS_POINT_OUTPUT
{
    float4 position			: SV_Position;
	float3 normal			: NORMAL;
	float3 light			: LIGHT;
	float2 tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
struct HS_PATCH_OUTPUT
{
    float Edges[3]			: SV_TessFactor;
	float Inside			: SV_InsideTessFactor;
};
//--------------------------------------------------------------------------------
struct DS_OUTPUT
{
    float4 position			: SV_Position;
	float3 normal			: NORMAL;
	float3 light			: LIGHT;
	float2 tex				: TEXCOORDS;
};
//--------------------------------------------------------------------------------
VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Calculate the output position of the vertex
	output.position  = (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.x] ) * input.weights.x);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.y] ) * input.weights.y);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.z] ) * input.weights.z);
	output.position += (mul( float4( input.position, 1.0f ), SkinMatrices[input.bone.w] ) * input.weights.w);

	// Calculate the world space normal vector
	output.normal  = (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.x] ) * input.weights.x).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.y] ) * input.weights.y).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.z] ) * input.weights.z).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.w] ) * input.weights.w).xyz;

	// Calculate the world space light vector
	output.light = LightPositionWS - output.position.xyz;

	// Pass the texture coordinates through
	output.tex = input.tex;

	return output;
}
//--------------------------------------------------------------------------------
HS_PATCH_OUTPUT HSPATCH( 
	InputPatch<VS_OUTPUT, 3> ip, 
    uint PatchID : SV_PrimitiveID )
{	
    HS_PATCH_OUTPUT output;

 //   // Insert code to compute Output here
	//output.Edges[0] = EdgeFactors.x; //2.0f;
	//output.Edges[1] = EdgeFactors.y; //2.0f;
	//output.Edges[2] = EdgeFactors.z; //2.0f;
	//
	//output.Inside = EdgeFactors.w; //2.0f;

	const float factor = 5.0f;

	output.Edges[0] = factor;
	output.Edges[1] = factor;
	output.Edges[2] = factor;

	output.Inside = factor;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSPATCH")]

HS_POINT_OUTPUT HSMAIN( 
    InputPatch<VS_OUTPUT, 3> ip, 
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID )
{
    HS_POINT_OUTPUT output;

    // Insert code to compute Output here.
    output.position = ip[i].position;
	output.normal = ip[i].normal;
	output.light = ip[i].light;
	output.tex = ip[i].tex;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DSMAIN( const OutputPatch<HS_POINT_OUTPUT, 3> TriPatch,
				        float3 Coords : SV_DomainLocation, 
				        HS_PATCH_OUTPUT input )
{
	DS_OUTPUT output;

	// Calculate the interpolated normal vector
	output.normal = Coords.x * TriPatch[0].normal
				  + Coords.y * TriPatch[1].normal
				  + Coords.z * TriPatch[2].normal;

	// Normalize the vector length for use in displacement
	output.normal = normalize( output.normal );

	// Interpolate the texture coordinates
	output.tex = Coords.x * TriPatch[0].tex
			   + Coords.y * TriPatch[1].tex
			   + Coords.z * TriPatch[2].tex;

	// Interpolate world space position
	float4 vWorldPos = Coords.x * TriPatch[0].position
					 + Coords.y * TriPatch[1].position
					 + Coords.z * TriPatch[2].position;

    // Calculate MIP level to fetch normal from
    float fHeightMapMIPLevel = clamp( ( distance( vWorldPos.xyz, float3(-20.0f, 25.0f, 10.0f) ) - 100.0f ) / 100.0f, 0.0f, 3.0f);
    
	// Sample the height map to know how much to displace the surface by
	float4 texHeight = HeightTexture.SampleLevel( LinearSampler, output.tex, fHeightMapMIPLevel );

	// Perform the displacement.  The 'fScale' parameter determines the maximum 
	// world space offset that can be applied to the surface.  The displacement
	// is performed along the interpolated vertex normal vector.
	const float fScale = 0.5f;
	vWorldPos.xyz = vWorldPos.xyz + output.normal * texHeight.r * fScale;

	// Transform world position with viewprojection matrix
	output.position = mul( vWorldPos, ViewProjMatrix );

	// Calculate the interpolated world space light vector.
	output.light  = Coords.x * TriPatch[0].light
				  + Coords.y * TriPatch[1].light
				  + Coords.z * TriPatch[2].light;
	
	return output;
}
//--------------------------------------------------------------------------------
float4 PSMAIN( in DS_OUTPUT input ) : SV_Target
{
	// Calculate the lighting
	float3 n = normalize( input.normal );
	float3 l = normalize( input.light );


	float4 texColor = ColorTexture.Sample( LinearSampler, input.tex );

	float4 color = texColor * (max(dot(n,l),0) + 0.05f );

	return( color );
}
//--------------------------------------------------------------------------------
