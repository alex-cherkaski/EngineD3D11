// Model matrix constant buffer.
cbuffer cbChangesPerMesh : register(b0)
{
    matrix modelMatrix;
}

// Orthographic matrix constant buffer.
cbuffer cbChangesRarely : register(b3)
{
    matrix orthographicMatrix;
}

Texture2D inputTexture0 : register(t0); // The first texture that is uploaded to the GPU.
SamplerState textureSampler : register(s0); // The texture sampler that will be used to sample the color map.

struct VS_Input // The input to the vertex shader.
{
    float4 position : POSITION; // The position or interpolated position of the vertex.
    float4 texture0 : TEXCOORD0; // The texture coordinate or interpolated texture coordinate of the vertex.
};

struct PS_Input // The input to the pixel shader.
{
    float4 position : SV_POSITION; // The position or interpolated position of the vertex.
    float2 texture0 : TEXCOORD0; // The texture coordinate or interpolated texture coordinate of the vertex.
};

PS_Input VS_Main(VS_Input input)
{
    // Zero-out pixel shader input struct.
    PS_Input output = (PS_Input) 0;
    
    // Apply MVP transformation to the vertex coordinate.
    output.position = input.position; //float4(input.position, 1.0f);
    output.position = mul(output.position, modelMatrix);
    output.position = mul(output.position, orthographicMatrix);
    
    // Forward the texture coordinate to the pixel shader.
    output.texture0 = input.texture0;
    
    // Forward the output data to the pixel shader.
    return output;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
    // Sample the first texture for its color using the vertex texture coordinate.
    const float4 finalColor = inputTexture0.Sample(textureSampler, input.texture0);
    return finalColor;
}

