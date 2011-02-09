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
float ComputeWeight(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	return dot(inPatch[j].position - inPatch[i].position, inPatch[i].normal);
}
//--------------------------------------------------------------------------------
float3 ComputeEdgePosition(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	return ( (2.0f * inPatch[i].position) + inPatch[j].position - (ComputeWeight(inPatch, i, j) * inPatch[i].normal)) / 3.0f;
	/*
	float3 Q = lerp(inPatch[i].position, inPatch[j].position, 1.0f / 3.0f);
	// Q projected onto a plane with normal N attached to point P is
	// = Q - wN
	//   w = (Q - P)•N
	float3 P = inPatch[i].position;
	float3 N = inPatch[i].normal;
	float w = dot(Q-P,N);
	Q = Q - (w * N);
	return Q;
	*/
}
//--------------------------------------------------------------------------------
float ComputeNormalWeight(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	float n = dot( (inPatch[j].position - inPatch[i].position), (inPatch[i].normal - inPatch[j].normal) );
	float d = dot( (inPatch[j].position - inPatch[i].position), (inPatch[j].position - inPatch[i].position) );
	
	return 2.0f * ( n / d);
}
//--------------------------------------------------------------------------------
float3 ComputeEdgeNormal(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	return  normalize
			(
				(
					inPatch[i].normal
					+
					inPatch[j].normal
				)
				-
				(
					ComputeNormalWeight(inPatch, i, j) 
					* 
					(inPatch[j].position - inPatch[i].position)
				)
			);
}
//--------------------------------------------------------------------------------
VS_OUTPUT vsMain( in VS_INPUT input )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	o.position = mul( float4( input.position, 1.0f ), mWorld ).xyz;
	
	//o.normal = normalize( mul( float4( input.normal, 1.0f ), mInvTposeWorld ).xyz );
	o.normal = normalize(mul(input.normal,(float3x3)mWorld));
		
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

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(13)]
[patchconstantfunc("hsConstantFunc")]
HS_OUTPUT hsDefault( InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;

    // Must provide a default definition just in
    // case we don't match any branch below
	output.position = float3(0.0f, 0.0f, 0.0f);
	output.normal = float3(0.0f, 0.0f, 0.0f);	
	
	// From paper:
	switch(i)
	{
		// Three actual vertices:
		
		// b(300)
		case 0:
		// b(030)
		case 1:
		// b(003)
		case 2:
		
			output.position = ip[i].position;
			output.normal = ip[i].normal;
			break;
		
		// Edge between v0 and v1
		
		// b(210)
		case 3:
			output.position = ComputeEdgePosition(ip, 0, 1);
			break;
		// b(120)
		case 4:
			output.position = ComputeEdgePosition(ip, 1, 0);
			break;
		
		// Edge between v1 and v2
		
		// b(021)
		case 5:
			output.position = ComputeEdgePosition(ip, 1, 2);
			break;
		// b(012)
		case 6:	
			output.position = ComputeEdgePosition(ip, 2, 1);
			break;
		
		// Edge between v2 and v0
		
		// b(102)
		case 7:
			output.position = ComputeEdgePosition(ip, 2, 0);
			break;
		// b(201)
		case 8:
			output.position = ComputeEdgePosition(ip, 0, 2);
			break;
		
		// Middle of triangle
		
		// b(111)
		case 9:
			float3 E = 
						(
							ComputeEdgePosition(ip, 0, 1) + ComputeEdgePosition(ip, 1, 0)
							+
							ComputeEdgePosition(ip, 1, 2) + ComputeEdgePosition(ip, 2, 1)
							+
							ComputeEdgePosition(ip, 2, 0) + ComputeEdgePosition(ip, 0, 2)
						) / 6.0f;
			float3 V = (ip[0].position + ip[1].position + ip[2].position) / 3.0f;
			
			output.position = E + ( (E - V) / 2.0f );
			
			break;
			
		// n(110) - between v0 and v1
		case 10:
			output.normal = ComputeEdgeNormal(ip, 0, 1);
			break;
		
		// n(011) - between v1 and v2
		case 11:
			output.normal = ComputeEdgeNormal(ip, 1, 2);
			break;
		
		// n(101) - between v2 and v0
		case 12:
			output.normal = ComputeEdgeNormal(ip, 2, 0);
			break;
	}
	

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(10)]
[patchconstantfunc("hsConstantFunc")]
HS_OUTPUT hsSilhouette( InputPatch<VS_OUTPUT, 3> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;

    // Must provide a default definition just in
    // case we don't match any branch below
	output.position = float3(0.0f, 0.0f, 0.0f);
	output.normal = float3(0.0f, 0.0f, 0.0f);
	
	// From paper:
	switch(i)
	{
		// Three actual vertices:
		
		// b(300)
		case 0:
		// b(030)
		case 1:
		// b(003)
		case 2:
		
			output.position = ip[i].position;
			output.normal = ip[i].normal;
			break;
		
		// Edge between v0 and v1
		
		// b(210)
		case 3:
		// b(120)
		case 4:
		
		// Edge between v1 and v2
		
		// b(021)
		case 5:
		// b(012)
		case 6:
		
		// Edge between v2 and v0
		
		// b(102)
		case 7:
		// b(201)
		case 8:
		
		// Middle of triangle
		
		// b(111)
		case 9:
		
			output.position = ip[0].position;
			output.normal = ip[0].normal;
			break;
	}
	

    return output;
}
//--------------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT dsMain( const OutputPatch<HS_OUTPUT, 13> TrianglePatch, float3 BarycentricCoordinates : SV_DomainLocation, HS_CONSTANT_DATA_OUTPUT input )
{
	DS_OUTPUT output;

	float u = BarycentricCoordinates.x;
	float v = BarycentricCoordinates.y;
	float w = BarycentricCoordinates.z;
	
	// Original Vertices
	float3 b300 = TrianglePatch[0].position;
	float3 b030 = TrianglePatch[1].position;
	float3 b003 = TrianglePatch[2].position;
	
	// Edge between v0 and v1
	float3 b210 = TrianglePatch[3].position;
	float3 b120 = TrianglePatch[4].position;
	
	// Edge between v1 and v2
	float3 b021 = TrianglePatch[5].position;
	float3 b012 = TrianglePatch[6].position;
	
	// Edge between v2 and v0
	float3 b102 = TrianglePatch[7].position;
	float3 b201 = TrianglePatch[8].position;
	
	// Middle of triangle
	float3 b111 = TrianglePatch[9].position;
	
	// Calculate this sample point
	float3 p = (b300 * pow(w,3)) + (b030 * pow(u,3)) + (b003 * pow(v,3))
			 + (b210 * 3.0f * pow(w,2) * u) 
			 + (b120 * 3.0f * w * pow(u,2)) 
			 + (b201 * 3.0f * pow(w,2) * v)
			 + (b021 * 3.0f * pow(u,2) * v) 
			 + (b102 * 3.0f * w * pow(v,2)) 
			 + (b012 * 3.0f * u * pow(v,2))
			 + (b111 * 6.0f * w * u * v);
	
	// Transform world position with viewprojection matrix
	output.Position = mul( float4(p, 1.0), mViewProj );
	
	// Compute the normal - LINEAR
	float3 vWorldNorm = w*TrianglePatch[0].normal + u*TrianglePatch[1].normal + v*TrianglePatch[2].normal;
	vWorldNorm = normalize( vWorldNorm );
	
	// Compute the normal - QUADRATIC
	float3 n200 = TrianglePatch[0].normal;
	float3 n020 = TrianglePatch[1].normal;
	float3 n002 = TrianglePatch[2].normal;
	
	float3 n110 = TrianglePatch[10].normal;
	float3 n011 = TrianglePatch[11].normal;
	float3 n101 = TrianglePatch[12].normal;
	
	vWorldNorm = (pow(w,2) * n200) + (pow(u,2) * n020) + (pow(v,2) * n002)
			   + (w * u * n110) + (u * v * n011) + (w * v * n101);
	
	// Perform the lighting calc
	float3 toCamera = normalize( cameraPosition.xyz );
	output.Colour = saturate( dot(vWorldNorm, toCamera) );
	
	//output.Colour = w*TrianglePatch[0].normal+u*TrianglePatch[1].normal+v*TrianglePatch[2].normal;
	//output.Colour = (vWorldNorm + 1.0f) / 2.0f;
	//output.Colour = float3(u,v,w);
	
	//output.Colour = TrianglePatch[0].normal.xyz;
	//output.Colour = (1.0f + output.Colour) / 2.0f;
	//output.Colour = float3(0.0f,0.0f,0.0f);
	
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

