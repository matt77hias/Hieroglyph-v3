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
	float3 cameraLookAt;
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
float ComputeWeight(InputPatch<VS_OUTPUT, 6> inPatch, int i, int j)
{
	return dot(inPatch[j].position - inPatch[i].position, inPatch[i].normal);
}
//--------------------------------------------------------------------------------
float ComputeWeight(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	return dot(inPatch[j].position - inPatch[i].position, inPatch[i].normal);
}
//--------------------------------------------------------------------------------
float3 ComputeEdgePosition(InputPatch<VS_OUTPUT, 6> inPatch, int i, int j)
{
	return ( 
			(2.0f * inPatch[i].position) + inPatch[j].position 
			- (ComputeWeight(inPatch, i, j) * inPatch[i].normal)
			) / 3.0f;
}
//--------------------------------------------------------------------------------
float3 ComputeEdgePosition(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	return ( 
			(2.0f * inPatch[i].position) + inPatch[j].position 
			- (ComputeWeight(inPatch, i, j) * inPatch[i].normal)
			) / 3.0f;
}
//--------------------------------------------------------------------------------
float3 ComputeEdgeNormal(InputPatch<VS_OUTPUT, 6> inPatch, int i, int j)
{
	float t = dot
			  (
				inPatch[j].position - inPatch[i].position
				, inPatch[i].normal + inPatch[j].normal
			   );
	
	float b = dot
			  (
				inPatch[j].position - inPatch[i].position
				, inPatch[j].position - inPatch[i].position
			   );
			   
	float v = 2.0f * (t / b);
	
	return normalize
			(
				inPatch[i].normal + inPatch[j].normal 
				- v * (inPatch[j].position - inPatch[i].position)
			);
}
//--------------------------------------------------------------------------------
float3 ComputeEdgeNormal(InputPatch<VS_OUTPUT, 3> inPatch, int i, int j)
{
	float t = dot
			  (
				inPatch[j].position - inPatch[i].position
				, inPatch[i].normal + inPatch[j].normal
			   );
	
	float b = dot
			  (
				inPatch[j].position - inPatch[i].position
				, inPatch[j].position - inPatch[i].position
			   );
			   
	float v = 2.0f * (t / b);
	
	return normalize
			(
				inPatch[i].normal + inPatch[j].normal 
				- v * (inPatch[j].position - inPatch[i].position)
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
    
    float3 faceNormal = normalize(cross(ip[2].position - ip[0].position, ip[1].position - ip[0].position));
    float3 viewDirection = normalize(cameraLookAt - cameraPosition);
    
    float backFace = sign(0.2+dot(faceNormal,viewDirection));

	// Insert code to compute Output here
	output.Edges[0] = EdgeFactors.x * backFace;
	output.Edges[1] = EdgeFactors.y * backFace;
	output.Edges[2] = EdgeFactors.z * backFace;
	
	output.Inside = EdgeFactors.w * backFace;

    return output;
}
//--------------------------------------------------------------------------------
HS_CONSTANT_DATA_OUTPUT hsConstantFuncAdaptive( InputPatch<VS_OUTPUT, 6> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_CONSTANT_DATA_OUTPUT output;
    
    float3 faceNormal = normalize(cross(ip[2].position - ip[0].position, ip[1].position - ip[0].position));
    float3 viewDirection = normalize(cameraLookAt - cameraPosition);
    
    float backFace = sign(0.2+dot(faceNormal,viewDirection));
    
    float3 a0 = normalize(cross(ip[1].position - ip[3].position, ip[0].position - ip[3].position));
    float3 a1 = normalize(cross(ip[2].position - ip[4].position, ip[1].position - ip[4].position));
    float3 a2 = normalize(cross(ip[0].position - ip[5].position, ip[2].position - ip[5].position));
    
    float e0 = saturate( (1.0f - dot(faceNormal, a0)) / 2.0f );
    float e1 = saturate( (1.0f - dot(faceNormal, a1)) / 2.0f );
    float e2 = saturate( (1.0f - dot(faceNormal, a2)) / 2.0f );

	// Insert code to compute Output here
	//output.Edges[0] = EdgeFactors.x * backFace;
	//output.Edges[1] = EdgeFactors.y * backFace;
	//output.Edges[2] = EdgeFactors.z * backFace;
	
	e0 = saturate( (1.0f - dot(viewDirection, normalize(ip[0].normal + ip[1].normal))) / 2.0f );
	e2 = saturate( (1.0f - dot(viewDirection, normalize(ip[1].normal + ip[2].normal))) / 2.0f );
	e1 = saturate( (1.0f - dot(viewDirection, normalize(ip[2].normal + ip[0].normal))) / 2.0f );
	
	output.Edges[0] = lerp(1.0f, EdgeFactors.x, e0);
	output.Edges[1] = 10.0f;//lerp(1.0f, EdgeFactors.y, e1);
	output.Edges[2] = lerp(1.0f, EdgeFactors.z, e2);
	
	/*output.Edges[0] *= backFace;
	output.Edges[1] *= backFace;
	output.Edges[2] *= backFace;*/
	
	output.Inside = EdgeFactors.w * backFace;

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
			
		// Normals
			
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
[outputcontrolpoints(13)]
[patchconstantfunc("hsConstantFuncAdaptive")]
HS_OUTPUT hsSilhouette( InputPatch<VS_OUTPUT, 6> ip, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;

    // Must provide a default definition just in
    // case we don't match any branch below
	output.position = float3(0.0f, 0.0f, 0.0f);
	output.normal = float3(0.0f, 0.0f, 0.0f);	
	
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
			
		// Normals
			
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
DS_OUTPUT dsMain( const OutputPatch<HS_OUTPUT, 13> TrianglePatch, float3 BarycentricCoordinates : SV_DomainLocation, HS_CONSTANT_DATA_OUTPUT input )
{
	DS_OUTPUT output;

	float u = BarycentricCoordinates.x;
	float v = BarycentricCoordinates.y;
	float w = BarycentricCoordinates.z;
	
	// Original Vertices
	float3 p300 = TrianglePatch[0].position;
	float3 p030 = TrianglePatch[1].position;
	float3 p003 = TrianglePatch[2].position;
	
	// Edge between v0 and v1
	float3 p210 = TrianglePatch[3].position;
	float3 p120 = TrianglePatch[4].position;
	
	// Edge between v1 and v2
	float3 p021 = TrianglePatch[5].position;
	float3 p012 = TrianglePatch[6].position;
	
	// Edge between v2 and v0
	float3 p102 = TrianglePatch[7].position;
	float3 p201 = TrianglePatch[8].position;
	
	// Middle of triangle
	float3 p111 = TrianglePatch[9].position;
	
	// Calculate this sample point
	float3 p = (p300 * pow(w,3)) + (p030 * pow(u,3)) + (p003 * pow(v,3))
			 + (p210 * 3.0f * pow(w,2) * u) 
			 + (p120 * 3.0f * w * pow(u,2)) 
			 + (p201 * 3.0f * pow(w,2) * v)
			 + (p021 * 3.0f * pow(u,2) * v) 
			 + (p102 * 3.0f * w * pow(v,2)) 
			 + (p012 * 3.0f * u * pow(v,2))
			 + (p111 * 6.0f * w * u * v);
	
	//p = w*TrianglePatch[0].position + u*TrianglePatch[1].position + v*TrianglePatch[2].position;
	
	// Transform world position with viewprojection matrix
	output.Position = mul( float4(p, 1.0), mViewProj );
	
	// Compute the normal - LINEAR
	float3 vWorldNorm = w*TrianglePatch[0].normal + u*TrianglePatch[1].normal + v*TrianglePatch[2].normal;
	
	// Compute the normal - QUADRATIC
	float3 n200 = TrianglePatch[0].normal;
	float3 n020 = TrianglePatch[1].normal;
	float3 n002 = TrianglePatch[2].normal;
	
	float3 n110 = TrianglePatch[10].normal;
	float3 n011 = TrianglePatch[11].normal;
	float3 n101 = TrianglePatch[12].normal;
	
	vWorldNorm = (pow(w,2) * n200) + (pow(u,2) * n020) + (pow(v,2) * n002)
			   + (w * u * n110) + (u * v * n011) + (w * v * n101);
	
	vWorldNorm = normalize( vWorldNorm );
	
	// Perform the lighting calc
	float3 toCamera = normalize( cameraPosition.xyz );
	output.Colour = saturate( dot(vWorldNorm, toCamera) ) * float3( 0.4, 0.4, 1.0 );
	
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

