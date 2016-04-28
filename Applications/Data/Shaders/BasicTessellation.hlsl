//--------------------------------------------------------------------------------
// BasicTessellation.hlsl
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
	matrix WorldMatrix;	
	matrix ViewProjMatrix;
};

cbuffer TessellationParameters
{
	float4 EdgeFactors;
};

cbuffer RenderingParameters
{
	float4 FinalColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
};
//--------------------------------------------------------------------------------
// Inter-stage structures
//--------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 position 		: POSITION;
};
//--------------------------------------------------------------------------------
struct HS_CONTROL_POINT_INPUT
{
	float3 WorldPosition	: POSITION;
};
//--------------------------------------------------------------------------------
struct HS_CONTROL_POINT_OUTPUT
{
	float3 WorldPosition	: POSITION;
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
};
//--------------------------------------------------------------------------------
HS_CONTROL_POINT_INPUT VSMAIN( in VS_INPUT input )
{
	HS_CONTROL_POINT_INPUT output;
	
	output.WorldPosition = mul( float4( input.position, 1.0f ), WorldMatrix ).xyz;

	return output;
}
//--------------------------------------------------------------------------------
HS_CONSTANT_DATA_OUTPUT PassThroughConstantHS( 
	InputPatch<HS_CONTROL_POINT_INPUT, 3> ip, 
    uint PatchID : SV_PrimitiveID )
{	
    HS_CONSTANT_DATA_OUTPUT output;

 //   // Insert code to compute Output here
	output.Edges[0] = EdgeFactors.x; //2.0f;
	output.Edges[1] = EdgeFactors.y; //2.0f;
	output.Edges[2] = EdgeFactors.z; //2.0f;
	
	output.Inside = EdgeFactors.w; //2.0f;

	//output.Edges[0] = 2.0f;
	//output.Edges[1] = 2.0f;
	//output.Edges[2] = 2.0f;

	//output.Inside = 2.0f;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PassThroughConstantHS")]

HS_CONTROL_POINT_OUTPUT HSMAIN( 
    InputPatch<HS_CONTROL_POINT_INPUT, 3> ip, 
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID )
{
    HS_CONTROL_POINT_OUTPUT output;

    // Insert code to compute Output here.
    output.WorldPosition = ip[i].WorldPosition;

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DSMAIN( const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> TrianglePatch,
				        float3 BarycentricCoordinates : SV_DomainLocation, 
				        HS_CONSTANT_DATA_OUTPUT input )
{
	DS_OUTPUT output;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].WorldPosition + BarycentricCoordinates.y * TrianglePatch[1].WorldPosition + BarycentricCoordinates.z * TrianglePatch[2].WorldPosition;
	
	// Transform world position with viewprojection matrix
	output.Position = mul( float4(vWorldPos.xyz, 1.0), ViewProjMatrix );
	
	return output;
}

float4 PSMAIN( in DS_OUTPUT input ) : SV_Target
{
	float4 color = FinalColor;
	//float4 color = float4( 0.75f, 0.75f, 1.0f, 0.0f );

	return( color );
}

