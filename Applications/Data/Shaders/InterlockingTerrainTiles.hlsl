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
};

// -------------------------------
// S H A R E D   F U N C T I O N S
// -------------------------------

float3 ComputePatchMidPoint(float3 cp0, float3 cp1, float3 cp2, float3 cp3)
{
	return (cp0 + cp1 + cp2 + cp3) / 4.0f;
}

float ComputeScaledDistance(float3 from, float3 to)
{
	// Compute the raw distance from the camera to the mid-point of this patch
	float d = distance( from, to );
	
	// Scale this to be 0.0 (at the min dist) and 1.0 (at the max dist)
	return (d - minMaxDistance.x) / (minMaxDistance.y - minMaxDistance.x);
}

float ComputePatchLOD(float3 midPoint)
{	
	// Compute the scaled distance
	float d = ComputeScaledDistance( cameraPosition.xyz, midPoint );
	
	// Transform this 0.0-1.0 distance scale into the desired LOD's
	// note: invert the distance so that close = high detail, far = low detail
	return lerp( minMaxLOD.x, minMaxLOD.y, 1.0f - d );
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
	// Useful aliases
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

float ComputePatchLODUsingLookup(float3 midPoint, float4 lookup)
{
	// Compute the scaled distance
	float d = ComputeScaledDistance( cameraPosition.xyz, midPoint );
	
	// lookup:
	//	x,y,z = patch normal
	//	w = standard deviation from patch plane
	
	// It's quite rare to get a std.dev of 1.0, or much above 0.75
	// so we apply a small fudge-factor here. Can be tweaked or removed
	// according to your chosen dataset...
	float sd = lookup.w / 0.75; 
	
	// The final LOD is the per-patch Std.Dev. scaled by the distance
	// from the viewer.
	// -> This computation won't ever increase LOD *above* that of the
	//    naieve linear form (see ComputePatchLOD) but it will work
	//    to ensure extra triangles aren't generated for patches that
	//    don't need them.
	float lod = sd * (1.0f - d);
	
	// Finally, transform this value into an actual LOD
	return lerp( minMaxLOD.x, minMaxLOD.y, clamp(lod, 0.0f, 1.0f) );
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
                                     uint i : SV_OutputControlPointID )
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
                                     uint i : SV_OutputControlPointID )
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
    
    // We then need to transform the world-space
    // coord to be a proper clip space output
    // that the rasterizer can deal with. Could delegate
    // to the GS, but no need this time!
    o.position = mul( float4( finalVertexCoord, 1.0f ), mViewProj );
    
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
		
		// 0% = black
		// 33% = blue
		// 66% = green
		// 100% = red
    
		if( lod > 0.66f )
		{
			lod -= 0.66f;
			lod /= 0.34f;
			o.colour = lerp( float3(0.0f, 1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f), lod);
		}
		else if( lod > 0.33f )
		{
			// High Detail
			lod -= 0.33f;
			lod /= 0.33f;
			o.colour = lerp( float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f), lod);
		}
		else
		{
			// Low Detail
			lod /= 0.33f;
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