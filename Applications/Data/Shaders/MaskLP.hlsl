//-------------------------------------------------------------------------------------------------
// MaskLP
//
// Vertex shaders and pixel shaders for performing generating a stencil mask indicating edge pixels
// for MSAA deferred rendering
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Textures
//-------------------------------------------------------------------------------------------------
Texture2DMS<float4, 4> GBufferTexture : register( t0 );

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
float4 VSMain( in float4 position : POSITION ) : SV_Position
{
	// Just pass through the vertex position
	return position;
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. Runs at per-pixel frequency.
//-------------------------------------------------------------------------------------------------
float4 PSMain( in float4 screenPos : SV_Position ) : SV_Target0
{
	// For all 4 samples, sample the mask from the G-Buffer. If the mask from all 4 samples is 0,
	// then we clip the pixel so that the stencil buffer doesn't get incremented.
	int2 sampleIndices = int2( screenPos.xy );
	float mask = 0.0f;

	const uint NumMSAASamples = 4;
	for ( uint i = 0; i < NumMSAASamples; ++i )
		mask += GBufferTexture.Load( sampleIndices, i ).w;

	if ( mask == 0.0f )
		clip( -1.0f );

	return 0.0f;
}

