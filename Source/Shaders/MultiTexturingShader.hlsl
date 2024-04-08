// Model matrix constant buffer.
cbuffer cbChangesPerFrame : register(b0)
{
    matrix modelMatrix;
}

// View matrix constant buffer.
cbuffer cbNeverChanges : register(b1)
{
    matrix viewMatrix;
}

// Projection matrix constant buffer.
cbuffer cbChangesRarely : register(b2)
{
    matrix projectionMatrix;
}

Texture2D inputTexture0 : register(t0); // The first texture that is uploaded to the GPU.
Texture2D inputTexture1 : register(t1); // The seconds texture that is uploaded to the GPU.
SamplerState textureSampler : register(s0); // The texture sampler that will be used to sample the color map.

struct VS_Input // The input to the vertex shader.
{
    float3 position : POSITION; // The position or interpolated position of the vertex.
    float2 texture0 : TEXCOORD0; // The texture coordinate or interpolated texture coordinate of the vertex.
};

struct PS_Input // The input to the pixel shader.
{
    float4 position : SV_POSITION; // The position or interpolated position of the vertex.
    float2 texture0 : TEXCOORD0; // The texture coordinate or interpolated texture coordinate of the vertex.
};

PS_Input VS_Main(VS_Input input)
{
    // Forward the input struct data to the pixel shader.
    PS_Input output = (PS_Input) 0;
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.texture0 = input.texture0;
    return output;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
    // Sample the first texture for its color using the vertex texture coordinate.
    const float4 textureColor0 = inputTexture0.Sample(textureSampler, input.texture0);
    
    // Sample the second texture for its color using the vertex texture coordinate.
    const float4 textureColor1 = inputTexture1.Sample(textureSampler, input.texture0);
    
    // Combine the two textures for the final pixel color.
    const float4 finalColor = 0.5f * textureColor0 + 0.5f * textureColor1;
    
    return finalColor;
}

