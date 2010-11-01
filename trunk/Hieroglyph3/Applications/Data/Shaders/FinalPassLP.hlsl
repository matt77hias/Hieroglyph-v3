//--------------------------------------------------------------------------------
// FinalPassLP
//
// Vertex shader and pixel shader for rendering the final pass of a light prepass
// deffered renderer
//--------------------------------------------------------------------------------

cbuffer Transforms
{	
	matrix WorldMatrix;
	matrix WorldViewMatrix;
	matrix WorldViewProjMatrix;
};

struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORDS0;
};

struct VSOutput
{
	float4 PositionCS	: SV_Position;
	float2 TexCoord		: TEXCOORD;
};

struct PSInput
{		
	float4 ScreenPos 	: SV_Position;
	float2 TexCoord		: TEXCOORD;	
};

//-------------------------------------------------------------------------------------------------
// Textures 
//-------------------------------------------------------------------------------------------------
Texture2D       	  	DiffuseMap : register( t0 );
Texture2DMS<float4, 4> 	LightTexture : register( t1 );
SamplerState    		AnisoSampler : register( s0 );

//-------------------------------------------------------------------------------------------------
// Vertex shader entry point
//-------------------------------------------------------------------------------------------------
VSOutput VSMain( in VSInput input )
{
	VSOutput output;
	
	// Calculate the clip-space position
	output.PositionCS = mul( input.Position, WorldViewProjMatrix );

	// Pass along the texture coordinate
	output.TexCoord = input.TexCoord;

	return output;
}

//-------------------------------------------------------------------------------------------------
// This contains the actual body of the pixel shader. Samples the diffuse map, and applies it
// to the values samples from the light render target.
//-------------------------------------------------------------------------------------------------
float4 FinalPass( in PSInput input, in uint subSampleIndex )
{
	// Sample the diffuse map
	float3 diffuseAlbedo = DiffuseMap.Sample( AnisoSampler, input.TexCoord ).rgb;
	float3 specularAlbedo = float3( 0.7f, 0.7f, 0.7f );
	
	// Determine our indices for sampling the texture based on the current screen position
	int2 sampleIndices = int2( input.ScreenPos.xy );
	
	// Sample the light target for the current sub-sample
	float4 lighting = LightTexture.Load( sampleIndices, subSampleIndex );
	
	// Apply the diffuse and specular albedo to the lighting value
	float3 diffuse = lighting.xyz * diffuseAlbedo;
	float3 specular = lighting.w * specularAlbedo;	
	
	// Final output is the sum of diffuse + specular
	return float4( diffuse + specular, 1.0f );
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-pixel
//-------------------------------------------------------------------------------------------------
float4 PSMain( in PSInput input ) : SV_Target0
{	
	return FinalPass( input, 0 );	
}

//-------------------------------------------------------------------------------------------------
// Pixel shader entry point. This shader runs per-sample
//-------------------------------------------------------------------------------------------------
float4 PSMainPerSample( in PSInput input, in uint subSampleIndex : SV_SampleIndex ) : SV_Target0
{	
	return FinalPass( input, subSampleIndex );	
}




