
Texture2D inputTexture : register(t0);      // The texture that is uploaded to the GPU.
SamplerState textureSampler : register(s0); // The texture sampler that will be used to sample the color map.

struct VS_Input // The input to the vertex shader.
{
    float3 position : POSITION;     // The position or interpolated position of the vertex.
    float2 texture0 : TEXCOORD0;    // The texture coordinate or interpolated texture coordinate of the vertex.
};

struct PS_Input // The input to the pixel shader.
{
    float4 position : SV_POSITION;  // The position or interpolated position of the vertex.
    float2 texture0 : TEXCOORD0;    // The texture coordinate or interpolated texture coordinate of the vertex.
};

PS_Input VS_Main(VS_Input input)
{
    // Forward the input struct data to the pixel shader.
    PS_Input output = (PS_Input) 0;
    output.position = float4(input.position, 1.0f);
    output.texture0 = input.texture0;
    return output;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
    // Set the color of the pixel to be the interpolated color of the texture.
    return inputTexture.Sample(textureSampler, input.texture0);
}

