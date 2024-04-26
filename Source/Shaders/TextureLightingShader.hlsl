// Model matrix constant buffer.
cbuffer cbChangesPerMesh : register(b0)
{
    matrix modelMatrix;
}

// View matrix constant buffer.
cbuffer cbChangesPerFrame : register(b1)
{
    matrix viewMatrix;
    float4 cameraPosition;
}

// Projection matrix constant buffer.
cbuffer cbChangesRarely : register(b2)
{
    matrix projectionMatrix;
}

Texture2D inputTexture0 : register(t0); // The first texture that is uploaded to the GPU.
SamplerState textureSampler : register(s0); // The texture sampler that will be used to sample the color map.

struct VS_Input // The input to the vertex shader.
{
    float3 position : POSITION;     // The position or interpolated position of the vertex.
    float3 normal   : NORMAL0;      // The normal or interpolated normal of the vertex.
    float2 texture0 : TEXCOORD0;    // The texture coordinate or interpolated texture coordinate of the vertex.
};

struct PS_Input // The input to the pixel shader.
{
    float4 position     : SV_POSITION;      // The position or interpolated position of the vertex.
    float3 normal       : NORMAL0;          // The normal or interpolated normal of the vertex.
    float2 texture0     : TEXCOORD0;        // The texture coordinate or interpolated texture coordinate of the vertex.
    float3 lightVector  : LIGHT_VECTOR;     // The direction vector from the vertex position to the light source.
    float3 viewVector   : VIEW_VECTOR;      // The direction vector from the vertex position to the camera position.
};

PS_Input VS_Main(VS_Input input)
{
    // Zero-out pixel shader input struct.
    PS_Input output = (PS_Input) 0;
    
    // Apply MVP transformation to the vertex coordinate.
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Forward the texture coordinate to the pixel shader.
    output.texture0 = input.texture0;
    
    // Rotate the surface normal to match the object's world orientation and normalize it.
    output.normal = mul(output.normal, (float3x3) modelMatrix);
    output.normal = normalize(input.normal);
    
    // Calculate the light vector direction that will be forwarded to the pixel shader.
    const float3 lightPosition = float3(0.0f, 0.0f, 0.0f);
    output.lightVector = normalize(lightPosition - input.position);
    
    // Calculate the camera direction vector that will be forwarded to the pixel shader.
    output.viewVector = normalize((float3)cameraPosition - input.position);
    
    // Forward the output data to the pixel shader.
    return output;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
    // The base color of light acting on the pixel.
    const float4 lightColor = float4(0.7f, 0.7f, 0.7f, 1.0f);
    
    // The ambient light strength acting on the pixel.
    const float4 ambientColor = float4(0.2f, 0.2f, 0.2f, 1.0f);
    
    // Calculate the diffuse light.
    const float diffuseTerm = clamp(dot(input.normal, input.lightVector), 0.0f, 1.0f);
    
    // Calculate the specular term.
    float specularTerm = 0;
    if (diffuseTerm > 0.0f)
    {
        // Calculate the vector between the light and view vectors.
        const float3 halfVector = normalize(input.lightVector + input.viewVector);
        
        // Calculate the final specular term value.
        specularTerm = pow(saturate(dot(input.normal, halfVector)), 25);
    }
    
    // Sample the first texture for its color using the vertex texture coordinate.
    const float4 textureColor0 = inputTexture0.Sample(textureSampler, input.texture0);
    
    // Calculate the final pixel color based on the texture color, light color, ambient light, and diffuse and specular terms.
    const float4 finalColor = textureColor0 * ambientColor + lightColor * diffuseTerm + lightColor * specularTerm;
    
    return finalColor;
}

