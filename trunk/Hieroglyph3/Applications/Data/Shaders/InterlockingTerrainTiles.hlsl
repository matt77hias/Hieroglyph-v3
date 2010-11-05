// -----------------------
// D E C L A R A T I O N S
// -----------------------

cbuffer main
{
	matrix mWorld;
	matrix mViewProj;
	matrix mInvTposeWorld;
};

Texture2D				texHeightMap	: register( t0 );
Texture2D				texLODLookup	: register( t1 );
SamplerState			smpHeightMap	: register( s0 );

cbuffer patch
{
	float4 cameraPosition;	//  w unused
	float4 minMaxDistance;	// zw unused
	float4 minMaxLOD;		// zw unused
}

cbuffer sampleparams
{
	// xy = pixel dimensions
	// zw = geometry dimensions
	float4 heightMapDimensions;
}

struct VS_INPUT
{
	float3 position : CONTROL_POINT_POSITION;
	float2 texCoord : CONTROL_POINT_TEXCOORD;
};

struct VS_OUTPUT
{
	float3 position : WORLD_SPACE_CONTROL_POINT_POSITION;
	float2 texCoord : CONTROL_POINT_TEXCOORD;
};

struct HS_PER_PATCH_OUTPUT
{
	float edgeTesselation[4] : SV_TessFactor;
	float insideTesselation[2] : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 position : CONTROL_POINT_POSITION;
	float2 texCoord : CONTROL_POINT_TEXCOORD;
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

float3 ComputePatchMidPoint(float3 cp0, float3 cp1, float3 cp2, float3 cp3)
{
	return (cp0 + cp1 + cp2 + cp3) / 4.0f;
}

float ComputePatchLOD(float3 midPoint)
{
	return lerp(minMaxLOD.x, minMaxLOD.y, (1.0f - (saturate(distance(cameraPosition.xyz, midPoint) / minMaxDistance.y))));
}

float SampleHeightMap(float2 uv)
{
	// - Must use SampleLevel() so we can specify the mip-map. The DS has no
	//   gradient information it can use to derive this detail...
	
	// bias so that it varies between above/below the origin...
	const float SCALE = 3.0f;
	return SCALE * texHeightMap.SampleLevel( smpHeightMap, uv, 0.0f ).r;
}

float3 Sobel( float2 tc )
{
	// Constants (hack)
	float2 pxSz = float2( 1.0f / heightMapDimensions.x, 1.0f / heightMapDimensions.y );
	
	// Compute the necessary offsets:
	float2 o00 = tc + float2( -pxSz.x, -pxSz.y );
	float2 o10 = tc + float2(    0.0f, -pxSz.y );
	float2 o20 = tc + float2(  pxSz.x, -pxSz.y );

	float2 o01 = tc + float2( -pxSz.x, 0.0f    );
	float2 o21 = tc + float2(  pxSz.x, 0.0f    );

	float2 o02 = tc + float2( -pxSz.x,  pxSz.y );
	float2 o12 = tc + float2(    0.0f,  pxSz.y );
	float2 o22 = tc + float2(  pxSz.x,  pxSz.y );

	// Use of the sobel filter requires the eight samples
	// surrounding the current pixel:
	float h00 = SampleHeightMap(o00);
	float h10 = SampleHeightMap(o10);
	float h20 = SampleHeightMap(o20);

	float h01 = SampleHeightMap(o01);
	float h21 = SampleHeightMap(o21);

	float h02 = SampleHeightMap(o02);
	float h12 = SampleHeightMap(o12);
	float h22 = SampleHeightMap(o22);

	// Evaluate the Sobel filters
	float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
	float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;

	// Generate the missing Z
	float Gz = 0.01f * sqrt( max(0.0f, 1.0f - Gx * Gx - Gy * Gy ) );

	// Make sure the returned normal is of unit length
	return normalize( float3( 2.0f * Gx, Gz, 2.0f * Gy ) );
}

float4 ReadLookup(uint2 idx)
{
	return texLODLookup.Load( uint3( idx, 0 ) );
}

uint2 ComputeLookupIndex(uint patch, int xOffset, int zOffset)
{
	// For a 32x32 grid there will be 1024 patches
	// thus 'patch' is 0-1023, we need to decode
	// this into an XY coordinate
	uint2 p;
	
	p.x = patch % (uint)heightMapDimensions.z;
	p.y = patch / (uint)heightMapDimensions.w;
	
	// With the XY coordinate for the patch being rendered we
	// then need to offset it according to the parameters
	p.x = clamp(p.x + xOffset, 0, ((uint)heightMapDimensions.z) - 1);
	p.y = clamp(p.y + zOffset, 0, ((uint)heightMapDimensions.w) - 1);
	
	return p;
}

float ComputePatchLODUsingLookup(float3 midpoint, float4 lookup)
{
	float lod;
	
	// Compute the distance coefficient.
	// will be 0.0 when at the camera, 1.0 when furthest away
	float d = clamp( distance( cameraPosition.xyz, midpoint ), minMaxLOD.x, minMaxLOD.y );
	d = (d - minMaxLOD.x) / (minMaxLOD.y - minMaxLOD.x);
	
	// lookup:
	//	x,y,z = patch normal
	//	w = standard deviation from patch plane
	
	lod = dot( float2( 1.0f - d, lookup.w ), float2( 0.2f, 0.8f ) );
	
	lod = lerp( minMaxLOD.x, minMaxLOD.y, lod );
	
	return clamp( lod, minMaxLOD.x, minMaxLOD.y );
}

// ---------------------------
// V E R T E X   S H A D E R S
// ---------------------------

VS_OUTPUT vsMain( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	o.position = mul( float4( v.position, 1.0f ), mWorld ).xyz;
		
	o.texCoord = v.texCoord;
		
	return o;
}

// -----------------------
// H U L L   S H A D E R S
// -----------------------

HS_PER_PATCH_OUTPUT hsPerPatch( InputPatch<VS_OUTPUT, 12> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_PER_PATCH_OUTPUT o = (HS_PER_PATCH_OUTPUT)0;
    
    // Determine the mid-point of this patch
    float3 midPoints[] =
    {
		// Main quad
		ComputePatchMidPoint( ip[0].position, ip[1].position, ip[2].position, ip[3].position )
		
		// +x neighbour
		, ComputePatchMidPoint( ip[2].position, ip[3].position, ip[4].position, ip[5].position )
		
		// +z neighbour
		, ComputePatchMidPoint( ip[1].position, ip[3].position, ip[6].position, ip[7].position )
		
		// -x neighbour
		, ComputePatchMidPoint( ip[0].position, ip[1].position, ip[8].position, ip[9].position )
		
		// -z neighbour
		, ComputePatchMidPoint( ip[0].position, ip[2].position, ip[10].position, ip[11].position )
    };
    
    // Determine the appropriate LOD for this patch
    float dist[] = 
    {
		// Main quad
		ComputePatchLOD( midPoints[0] )
		
		// +x neighbour
		, ComputePatchLOD( midPoints[1] )
		
		// +z neighbour
		, ComputePatchLOD( midPoints[2] )
		
		// -x neighbour
		, ComputePatchLOD( midPoints[3] )
		
		// -z neighbour
		, ComputePatchLOD( midPoints[4] )
	};
    
    // Set it up so that this patch always has an interior matching
    // the patch LOD.
    o.insideTesselation[0] = 
		o.insideTesselation[1] = dist[0];
        
    // For the edges its more complex as we have to match
    // the neighbouring patches. The rule in this case is:
    //
    // - If the neighbour patch is of a lower LOD we
    //   pick that LOD as the edge for this patch.
    //
    // - If the neighbour patch is a higher LOD then 
    //   we stick with our LOD and expect them to blend down
    //   towards us
    
    o.edgeTesselation[0] = min( dist[0], dist[4] );    
    o.edgeTesselation[1] = min( dist[0], dist[3] );
	o.edgeTesselation[2] = min( dist[0], dist[2] );
	o.edgeTesselation[3] = min( dist[0], dist[1] );
	
    return o;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("hsPerPatch")]
HS_OUTPUT hsSimple( InputPatch<VS_OUTPUT, 12> p, 
                                     uint i : SV_OutputControlPointID,
                                     uint PatchID : SV_PrimitiveID )
{
	// NOTE: This code is executed for each control point
	//		 so must index on 'i'
	HS_OUTPUT o = (HS_OUTPUT)0;
	
	// Simply pass through the value for now
	o.position = p[i].position;
	o.texCoord = p[i].texCoord;
	
	return o;
}

HS_PER_PATCH_OUTPUT hsPerPatchWithLookup( InputPatch<VS_OUTPUT, 12> ip, uint PatchID : SV_PrimitiveID )
{	
    HS_PER_PATCH_OUTPUT o = (HS_PER_PATCH_OUTPUT)0;
    
    // Grab the lookup values for these patches
    float4 tileLookup[] = 
    {
		// Main quad
		ReadLookup( ComputeLookupIndex( PatchID, 0, 0 ) )
		
		// +x neighbour
		, ReadLookup( ComputeLookupIndex( PatchID, 0, 1 ) )
		
		// +z neighbour
		, ReadLookup( ComputeLookupIndex( PatchID, 1, 0 ) )
		
		// -x neighbour
		, ReadLookup( ComputeLookupIndex( PatchID, 0, -1 ) )
		
		// -z neighbour
		, ReadLookup( ComputeLookupIndex( PatchID, -1, 0 ) )
    };
    
    // Determine the mid-point of this patch
    float3 midPoints[] =
    {
		// Main quad
		ComputePatchMidPoint( ip[0].position, ip[1].position, ip[2].position, ip[3].position )
		
		// +x neighbour
		, ComputePatchMidPoint( ip[2].position, ip[3].position, ip[4].position, ip[5].position )
		
		// +z neighbour
		, ComputePatchMidPoint( ip[1].position, ip[3].position, ip[6].position, ip[7].position )
		
		// -x neighbour
		, ComputePatchMidPoint( ip[0].position, ip[1].position, ip[8].position, ip[9].position )
		
		// -z neighbour
		, ComputePatchMidPoint( ip[0].position, ip[2].position, ip[10].position, ip[11].position )
    };
    
    // Determine the LOD for each patch
    float detail[] = 
    {
		// Main quad
		ComputePatchLODUsingLookup( midPoints[0], tileLookup[0] )
		
		// +x neighbour
		, ComputePatchLODUsingLookup( midPoints[1], tileLookup[1] )
		
		// +z neighbour
		, ComputePatchLODUsingLookup( midPoints[2], tileLookup[2] )
		
		// -x neighbour
		, ComputePatchLODUsingLookup( midPoints[3], tileLookup[3] )
		
		// -z neighbour
		, ComputePatchLODUsingLookup( midPoints[4], tileLookup[4] )
    };
    
    // Set it up so that this patch always has an interior matching
    // the patch LOD.
    o.insideTesselation[0] = 
		o.insideTesselation[1] = detail[0];
        
    // For the edges its more complex as we have to match
    // the neighbouring patches. The rule in this case is:
    //
    // - If the neighbour patch is of a lower LOD we
    //   pick that LOD as the edge for this patch.
    //
    // - If the neighbour patch is a higher LOD then 
    //   we stick with our LOD and expect them to blend down
    //   towards us
    
    /*
    o.edgeTesselation[0] = min( detail[0], detail[3] );
    o.edgeTesselation[1] = min( detail[0], detail[4] );    
    o.edgeTesselation[2] = min( detail[0], detail[1] );
	o.edgeTesselation[3] = min( detail[0], detail[2] );
	*/
	
	o.edgeTesselation[0] = min( detail[0], detail[4] );    
    o.edgeTesselation[1] = min( detail[0], detail[3] );
	o.edgeTesselation[2] = min( detail[0], detail[2] );
	o.edgeTesselation[3] = min( detail[0], detail[1] );
	
    return o;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("hsPerPatchWithLookup")]
HS_OUTPUT hsComplex( InputPatch<VS_OUTPUT, 12> p, 
                                     uint i : SV_OutputControlPointID,
                                     uint PatchID : SV_PrimitiveID )
{
	// NOTE: This code is executed for each control point
	//		 so must index on 'i'
	HS_OUTPUT o = (HS_OUTPUT)0;
	
	// Simply pass through the value for now
	o.position = p[i].position;
	o.texCoord = p[i].texCoord;
	
	return o;
}

// ---------------------------
// D O M A I N   S H A D E R S
// ---------------------------

[domain("quad")]
DS_OUTPUT dsMain( HS_PER_PATCH_OUTPUT input, 
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
					   
	float2 texcoord    = patch[0].texCoord * (1.0f-uv.x) * (1.0f-uv.y)
					   + patch[1].texCoord * uv.x * (1.0f-uv.y)
					   + patch[2].texCoord * (1.0f-uv.x) * uv.y
					   + patch[3].texCoord * uv.x * uv.y	   
					   ;
					   
	// Determine the height from the texture
	finalVertexCoord.y = SampleHeightMap(texcoord); 
    
    // Store some debugging information
    o.uvw = float3(uv,0.0f);
    o.wPos = finalVertexCoord;
    o.edges = float4( input.edgeTesselation[0], input.edgeTesselation[1], input.edgeTesselation[2], input.edgeTesselation[3] );
    o.inside = float2( input.insideTesselation[0], input.insideTesselation[1] );
    
    // We then need to transform the world-space
    // coord to be a proper projection space output
    // that the rasterizer can deal with. Could delegate
    // to the GS, but no need this time!
    o.position = mul( float4( o.wPos, 1.0f ), mViewProj );
    
    #if defined( SHADING_SOLID )
    
		o.colour = float3( 0.0f, 0.0f, 0.0f );
    
    #elif defined( SHADING_SIMPLE )
    
		// Perform a sobel filter on the heightmap to determine an appropriate
		// normal vector
		float3 normal = Sobel( texcoord );
		normal = normalize( mul( float4(normal, 1.0f), mInvTposeWorld ).xyz );
		o.colour = min(0.75f, max(0.0f, dot( normal, float3( 0.0f, 1.0f, 0.0f ) ) ) );
    
    #elif defined( SHADING_DEBUG_LOD )
    
		float lod = (input.insideTesselation[0] - minMaxLOD.x) / (minMaxLOD.y - minMaxLOD.x);
    
		if( lod > 0.75f )
		{
			lod -= 0.75f;
			lod /= 0.25f;
			o.colour = lerp( float3(1.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 0.0f), lod);
		}
		else if( lod > 0.5f )
		{
			// High Detail
			lod -= 0.5f;
			lod /= 0.25f;
			o.colour = lerp( float3(0.0f, 1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f), lod);
		}
		else if( lod > 0.25f )
		{
			// Medium Detail
			lod -= 0.25f;
			lod /= 0.25f;
			o.colour = lerp( float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f), lod);
		}
		else
		{
			// Low Detail
			lod /= 0.25f;
			o.colour = lerp( float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), lod);
		}
    
    #endif
    
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