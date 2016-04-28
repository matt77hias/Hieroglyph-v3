//--------------------------------------------------------------------------------
// PhongShading.hlsl
//
// This set of shaders implements the most basic phong shading.
//
// Copyright (C) 2010 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

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
	//output.normal = mul( input.normal, (float3x3)WorldMatrix );
	output.normal  = (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.x] ) * input.weights.x).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.y] ) * input.weights.y).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.z] ) * input.weights.z).xyz;
	output.normal += (mul( input.normal, (float3x3)SkinNormalMatrices[input.bone.w] ) * input.weights.w).xyz;

	//output.normal = mul( input.normal, (float3x3)WorldMatrix );
	output.light = LightPositionWS - output.position.xyz;
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

	const float factor = 16.0f;

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

	output.normal = Coords.x * TriPatch[0].normal
				  + Coords.y * TriPatch[1].normal
				  + Coords.z * TriPatch[2].normal;

	output.normal = normalize( output.normal );

	output.tex = Coords.x * TriPatch[0].tex
			   + Coords.y * TriPatch[1].tex
			   + Coords.z * TriPatch[2].tex;

	// Interpolate world space position with barycentric coordinates
	float4 vWorldPos = Coords.x * TriPatch[0].position
					 + Coords.y * TriPatch[1].position
					 + Coords.z * TriPatch[2].position;


    // Calculate MIP level to fetch normal from
    float fHeightMapMIPLevel = 3.0f; //clamp( ( distance( vWorldPos, g_vEye ) - 100.0f ) / 100.0f, 0.0f, 3.0f);
    
    // Sample normal and height map
    //float4 vNormalHeight = g_nmhTexture.SampleLevel( g_samLinear, output.texCoord, fHeightMapMIPLevel );

	float4 texColor = ColorTexture.SampleLevel( LinearSampler, output.tex, fHeightMapMIPLevel );



	vWorldPos.xyz = vWorldPos.xyz + output.normal * texColor.r;

	// Transform world position with viewprojection matrix
	output.position = mul( vWorldPos, ViewProjMatrix );

	
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

	//float4 color.rgb = LightColor.rgb * (max(dot(n,l),0) + 0.25f );
	float4 color = texColor * (max(dot(n,l),0) + 0.25f );

	return( color );
}
//--------------------------------------------------------------------------------
