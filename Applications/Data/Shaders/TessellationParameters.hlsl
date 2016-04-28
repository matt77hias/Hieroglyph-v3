// -----------------------
// D E C L A R A T I O N S
// -----------------------

cbuffer main
{
	matrix WorldMatrix;
	matrix ViewProjMatrix;
	float4 vEdgeWeights;
	float2 vInsideWeights;
};

struct VS_INPUT
{
	float3 position : CONTROL_POINT_POSITION;
	float4 colour : COLOUR;
};

struct VS_OUTPUT
{
	float3 position : WORLD_SPACE_CONTROL_POINT_POSITION;
	float3 colour : COLOUR;
};

struct HS_PER_TRI_PATCH_OUTPUT
{
	float edgeTesselation[3] : SV_TessFactor;
	float insideTesselation[1] : SV_InsideTessFactor;
};

struct HS_PER_QUAD_PATCH_OUTPUT
{
	float edgeTesselation[4] : SV_TessFactor;
	float insideTesselation[2] : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 position : CONTROL_POINT_POSITION;
	float3 colour : COLOUR;
};

struct DS_OUTPUT
{
	float4 position : SV_Position;
	float3 colour : COLOUR;
	float3 uvw : DOMAIN_SHADER_LOCATION;
	float3 wPos : WORLD_POSITION;
	float4 edges : EDGE_FACTORS;
	float2 inside : INNER_FACTORS;
};

// -------------------------------
// S H A R E D   F U N C T I O N S
// -------------------------------

// ---------------------------
// V E R T E X   S H A D E R S
// ---------------------------

VS_OUTPUT vsMain( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	o.position = mul( float4( v.position, 1.0f ), WorldMatrix ).xyz;
	
	o.colour = v.colour.rgb;
		
	return o;
}

// -----------------------
// H U L L   S H A D E R S
// -----------------------

HS_PER_TRI_PATCH_OUTPUT hsPerTriPatch( InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_PER_TRI_PATCH_OUTPUT o = (HS_PER_TRI_PATCH_OUTPUT)0;
    
    o.edgeTesselation[0] = vEdgeWeights.x;
    o.edgeTesselation[1] = vEdgeWeights.y;
    o.edgeTesselation[2] = vEdgeWeights.z;
		
	o.insideTesselation[0] = vInsideWeights.x;
    
    return o;
}

[domain("tri")]
#if defined(POW2_PARTITIONING)
[partitioning("pow2")]
#elif defined(FRAC_ODD_PARTITIONING)
[partitioning("fractional_odd")]
#elif defined(FRAC_EVEN_PARTITIONING)
[partitioning("fractional_even")]
#elif defined(INTEGER_PARTITIONING)
[partitioning("integer")]
#endif
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hsPerTriPatch")]
HS_OUTPUT hsTriangleMain( InputPatch<VS_OUTPUT, 3> p, 
                                     uint i : SV_OutputControlPointID,
                                     uint PatchID : SV_PrimitiveID )
{
	// NOTE: This code is executed for each control point
	//		 so must index on 'i'
	HS_OUTPUT o = (HS_OUTPUT)0;
	
	// Simply pass through the value for now
	o.position = p[i].position;
	o.colour = p[i].colour;
	
	return o;
}

HS_PER_QUAD_PATCH_OUTPUT hsPerQuadPatch( InputPatch<VS_OUTPUT, 4> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_PER_QUAD_PATCH_OUTPUT o = (HS_PER_QUAD_PATCH_OUTPUT)0;
    
	float4 edges;
	float2 inner;
	float2 unrounded_inner;
    
    Process2DQuadTessFactorsAvg( vEdgeWeights, vInsideWeights, edges, inner, unrounded_inner );
    
    o.edgeTesselation[0] = edges.x;
    o.edgeTesselation[1] = edges.y;
    o.edgeTesselation[2] = edges.z;
    o.edgeTesselation[3] = edges.w;
    
    o.insideTesselation[0] = inner.x;
    o.insideTesselation[1] = inner.y;
    
    return o;
}

[domain("quad")]
#if defined(POW2_PARTITIONING)
[partitioning("pow2")]
#elif defined(FRAC_ODD_PARTITIONING)
[partitioning("fractional_odd")]
#elif defined(FRAC_EVEN_PARTITIONING)
[partitioning("fractional_even")]
#elif defined(INTEGER_PARTITIONING)
[partitioning("integer")]
#endif
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("hsPerQuadPatch")]
HS_OUTPUT hsQuadMain( InputPatch<VS_OUTPUT, 4> p, 
                                     uint i : SV_OutputControlPointID,
                                     uint PatchID : SV_PrimitiveID )
{
	// NOTE: This code is executed for each control point
	//		 so must index on 'i'
	HS_OUTPUT o = (HS_OUTPUT)0;
	
	// Simply pass through the value for now
	o.position = p[i].position;
	o.colour = p[i].colour;
	
	return o;
}

// ---------------------------
// D O M A I N   S H A D E R S
// ---------------------------

[domain("tri")]
DS_OUTPUT dsTriangleMain( HS_PER_TRI_PATCH_OUTPUT input, 
                        float3 uvw : SV_DomainLocation,
                        const OutputPatch<HS_OUTPUT, 3> patch )
{
    DS_OUTPUT o = (DS_OUTPUT)0;
    
    // We need to take the three world space
    // coordinates in patch[] and the interpolation
    // values in uvw (barycentric coords) and determine 
    // the appropriate interpolated position.
    float3 finalVertexCoord = float3( 0.0f, 0.0f, 0.0f );
    
	
	// Sum up the original edge vertices
	finalVertexCoord += (patch[0].position * uvw.x);
	finalVertexCoord += (patch[1].position * uvw.y);
	finalVertexCoord += (patch[2].position * uvw.z);
    
    // Store some debugging information
    o.uvw = uvw;
    o.wPos = finalVertexCoord;
    o.edges = float4( input.edgeTesselation[0], input.edgeTesselation[1], input.edgeTesselation[2], 0.0f );
    o.inside = float2( input.insideTesselation[0], 0.0f );
    
    // We then need to transform the world-space
    // coord to be a proper projection space output
    // that the rasterizer can deal with. Could delegate
    // to the GS, but no need this time!
    o.position = mul( float4( o.wPos, 1.0f ), ViewProjMatrix );
    
    // We also need to interpolate the colour in
    // the same way
    float3 finalVertexColour = float3( 0.0f, 0.0f, 0.0f );
    
    finalVertexColour += (patch[0].colour * uvw.x);
	finalVertexColour += (patch[1].colour * uvw.y);
	finalVertexColour += (patch[2].colour * uvw.z);
    
    // Store this in the output so that the rasterizer
    // can interpolate it across the surface of new
    // triangles and feed it into the pixel shader
    o.colour = finalVertexColour;
    
    return o;    
}

[domain("quad")]
DS_OUTPUT dsQuadMain( HS_PER_QUAD_PATCH_OUTPUT input, 
                        float2 uv : SV_DomainLocation,
                        const OutputPatch<HS_OUTPUT, 4> patch )
{
    DS_OUTPUT o = (DS_OUTPUT)0;
    
    // We need to take the three world space
    // coordinates in patch[] and the interpolation
    // values in uvw (barycentric coords) and determine 
    // the appropriate interpolated position.
    float3 finalVertexCoord = float3( 0.0f, 0.0f, 0.0f );
      
    // u,v  
    // 0,0 is patch[0].position
    // 1,0 is patch[1].position
    // 0,1 is patch[2].position
    // 1,1 is patch[3].position
    
    /*
    0--1
      /
     /
    2--3
    */
        
    finalVertexCoord.xz = patch[0].position.xz * (1.0f-uv.x) * (1.0f-uv.y)
					   + patch[1].position.xz * uv.x * (1.0f-uv.y)
					   + patch[2].position.xz * (1.0f-uv.x) * uv.y
					   + patch[3].position.xz * uv.x * uv.y	   
					   ;
    
    // Store some debugging information
    o.uvw = float3(uv,0.0f);
    o.wPos = finalVertexCoord;
    o.edges = float4( input.edgeTesselation[0], input.edgeTesselation[1], input.edgeTesselation[2], input.edgeTesselation[3] );
    o.inside = float2( input.insideTesselation[0], input.insideTesselation[1] );
    
    // We then need to transform the world-space
    // coord to be a proper projection space output
    // that the rasterizer can deal with. Could delegate
    // to the GS, but no need this time!
    o.position = mul( float4( o.wPos, 1.0f ), ViewProjMatrix );
    
    // We also need to interpolate the colour in
    // the same way
    float3 finalVertexColour = float3( 0.0f, 0.0f, 0.0f );
    
    finalVertexColour += (patch[0].colour * uv.x);
	finalVertexColour += (patch[1].colour * uv.y);
	//finalVertexColour += (patch[2].colour * uv.z);
    
    // Store this in the output so that the rasterizer
    // can interpolate it across the surface of new
    // triangles and feed it into the pixel shader
    o.colour = finalVertexColour;
    
    return o;    
}

// -------------------------------
// G E O M E T R Y   S H A D E R S
// -------------------------------

[maxvertexcount(3)]
void gsMain( triangle DS_OUTPUT input[3], inout TriangleStream<DS_OUTPUT> TriangleOutputStream )
{
    TriangleOutputStream.Append( input[0] );
    TriangleOutputStream.Append( input[1] );
    TriangleOutputStream.Append( input[2] );
    TriangleOutputStream.RestartStrip();
}

// -------------------------
// P I X E L   S H A D E R S
// -------------------------

float4 psMain( in DS_OUTPUT f ) : SV_TARGET
{
	return float4( f.colour, 1.0f );
}