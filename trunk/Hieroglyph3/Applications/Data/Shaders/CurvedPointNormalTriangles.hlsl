//--------------------------------------------------------------------------------
// CurvedPointNormalTriangles.hlsl
//
// This set of shaders implements the most basic tessellation output, and just
// interpolates the input vertices to locate the resulting tessellated points.
//
// Copyright (C) 2009 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Resources
//--------------------------------------------------------------------------------
cbuffer Transforms
{
	matrix mWorld;	
	matrix mViewProj;
	matrix mInvTposeWorld;
};

cbuffer TessellationParameters
{
	float4 EdgeFactors;
};

cbuffer RenderingParameters
{
	float3 cameraPosition;
};
//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position 		: POSITION;
	float3 normal 			: NORMAL;
};
//--------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float3 position			: WORLD_SPACE_CONTROL_POINT_POSITION;
	float3 normal			: WORLD_SPACE_CONTROL_POINT_NORMAL;
};
//--------------------------------------------------------------------------------
struct HS_OUTPUT
{
	float3 position			: WORLD_SPACE_CONTROL_POINT_POSITION;
	float3 normal			: WORLD_SPACE_CONTROL_POINT_NORMAL;
};
//--------------------------------------------------------------------------------
struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[3]			: SV_TessFactor;
	float Inside			: SV_InsideTessFactor;
};
//--------------------------------------------------------------------------------
struct DS_OUTPUT
{
    float4 Position			: SV_Position;
    float3 Colour			: COLOUR;
};
//--------------------------------------------------------------------------------
VS_OUTPUT vsMain( in VS_INPUT input )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	o.position = mul( float4( input.position, 1.0f ), mWorld ).xyz;
	
	o.normal = normalize( mul( float4( input.normal, 1.0f ), mInvTposeWorld ) ).xyz;
		
	return o;
}
//--------------------------------------------------------------------------------
HS_CONSTANT_DATA_OUTPUT hsConstantFunc( InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_CONSTANT_DATA_OUTPUT output;

	// Insert code to compute Output here
	output.Edges[0] = EdgeFactors.x;
	output.Edges[1] = EdgeFactors.y;
	output.Edges[2] = EdgeFactors.z;
	
	output.Inside = EdgeFactors.w;

	//output.Inside = 2.0f;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hsConstantFunc")]
HS_OUTPUT hsDefault( InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;

    // Simply pass through the value for now
	output.position = ip[i].position;
	output.normal = ip[i].normal;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hsConstantFunc")]
HS_OUTPUT hsSilhouette( InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;

    // Simply pass through the value for now
	output.position = ip[i].position;
	output.normal = ip[i].normal;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT dsMain( const OutputPatch<HS_OUTPUT, 3> TrianglePatch, float3 BarycentricCoordinates : SV_DomainLocation, HS_CONSTANT_DATA_OUTPUT input )
{
	DS_OUTPUT output;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].position + BarycentricCoordinates.y * TrianglePatch[1].position + BarycentricCoordinates.z * TrianglePatch[2].position;
	
	// Transform world position with viewprojection matrix
	output.Position = mul( float4(vWorldPos.xyz, 1.0), mViewProj );
	
	// Compute the normal
	float3 vWorldNorm = BarycentricCoordinates.x * TrianglePatch[0].normal + BarycentricCoordinates.y * TrianglePatch[1].normal + BarycentricCoordinates.z * TrianglePatch[2].normal;
	vWorldNorm = normalize( vWorldNorm );
	
	float3 toCamera = normalize( cameraPosition );
	output.Colour = saturate( dot(vWorldNorm, toCamera) );
	
	return output;
}
//--------------------------------------------------------------------------------
[maxvertexcount(3)]
void gsMain( triangle DS_OUTPUT input[3], inout TriangleStream<DS_OUTPUT> TriangleOutputStream )
{
    TriangleOutputStream.Append( input[0] );
    TriangleOutputStream.Append( input[1] );
    TriangleOutputStream.Append( input[2] );
    TriangleOutputStream.RestartStrip();
}
//--------------------------------------------------------------------------------
float4 psMain( in DS_OUTPUT input ) : SV_Target
{
	return float4(input.Colour, 1.0f);
}

