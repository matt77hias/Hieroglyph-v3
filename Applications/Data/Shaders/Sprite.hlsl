//======================================================================================
// Constant buffers
//======================================================================================

cbuffer VSConstants
{
	float4 TexAndViewportSize;
}

//======================================================================================
// Samplers
//======================================================================================

Texture2D	SpriteTexture : register(t0);
SamplerState SpriteSampler : register(s0);


//======================================================================================
// Input/Output structs
//======================================================================================
struct VS_INPUT
{
	// Per-vertex data
    float2 Position : POSITION;
    float2 TexCoord : TEXCOORD;

	// Instance data
    float4x4 Transform : TRANSFORM;
    float4 Color : COLOR;
    float4 SourceRect : SOURCERECT;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

//-------------------------------------------------------------------------------------
// Functionality common to both vertex shaders
//-------------------------------------------------------------------------------------
VS_OUTPUT SpriteVS(in VS_INPUT input)
{
    // Scale the quad so that it's texture-sized
    float4 positionSS = float4(input.Position * input.SourceRect.zw, 0.0f, 1.0f);

    // Apply transforms in screen space
    positionSS = mul(positionSS, input.Transform);

    // Scale by the viewport size, flip Y, then rescale to device coordinates
    float4 positionDS = positionSS;
    positionDS.xy /= TexAndViewportSize.zw;
    positionDS = positionDS * 2 - 1;
    positionDS.y *= -1;

    // Figure out the texture coordinates
    float2 outTexCoord = input.TexCoord;
    outTexCoord.xy *= input.SourceRect.zw / TexAndViewportSize.xy;
    outTexCoord.xy += input.SourceRect.xy / TexAndViewportSize.xy;

    VS_OUTPUT output;
    output.Position = positionDS;
    output.TexCoord = outTexCoord;
    output.Color = input.Color;

    return output;
}

//======================================================================================
// Pixel Shader
//======================================================================================
float4 SpritePS(in VS_OUTPUT input) : SV_Target
{
    float4 texColor = SpriteTexture.Sample(SpriteSampler, input.TexCoord);
    texColor = texColor * input.Color;
    return texColor;
}
