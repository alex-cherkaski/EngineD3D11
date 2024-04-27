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

struct VS_Input // The input to the vertex shader.
{
    float3 position : POSITION; // The position or interpolated position of the vertex.
    float3 normal   : NORMAL0;  // The normal or interpolated normal of the vertex.
};

struct PS_Input // The input to the pixel shader.
{
    float4 position     : SV_POSITION;  // The position or interpolated position of the vertex.
    float3 normal       : NORMAL0;      // The normal or interpolated normal of the vertex.
    float3 lightVector  : LIGHT_VECTOR; // The direction vector from the vertex position to the light source.
    float3 viewVector   : VIEW_VECTOR;  // The direction vector from the vertex to the camera position.
};

PS_Input VS_Main(VS_Input input)
{
    // Zero-out pixel shader input struct.
    PS_Input output = (PS_Input) 0;
    
    // Calculate the world position of the vertex or interpolated vertex.
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, modelMatrix);
    
    // Update the normal vector orientation in world space.
    output.normal = mul(input.normal, (float3x3)modelMatrix);
    output.normal = normalize(output.normal);
    
    // Calculate the light vector in world space.
    const float3 lightPosition = float3(0.0f, 0.0f, 1.0f);
    output.lightVector = normalize(lightPosition - (float3)output.position);
    
    // Calculate the view vector in world space.
    output.viewVector = (float3)normalize(cameraPosition - output.position);
    
    // Apply the view and projection matrices to the verte or interpolated vertex.
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Forward the output data to the pixel shader.
    return output;
}

float4 PS_Main(PS_Input input) : SV_TARGET
{
    // The base color of the object being shaded.
    float3 ambientColor = float3(0.2f, 0.2f, 0.2f);
    
    // The base color emitted by the light source.
    float3 lightColor = float3(0.7f, 0.7f, 0.7f);
    
    // Calculate the diffuse component.
    const float diffuse = clamp(dot(input.normal, input.lightVector), 0.0f, 1.0f);
    const float3 diffuseLight = lightColor * diffuse;
    
    // Calculate the specular component.
    float3 specularLight = float3(0.0f, 0.0f, 0.0f);
    if (diffuse > 0.0f)
    {
        // Calculate the unit half vector vetween the light and view vectors.
        const float3 halfVector = normalize(input.lightVector + input.viewVector);
        
        // Calculate the specular intensity.
        const float specular = pow(saturate(dot(input.normal, halfVector)), 5);
        
        // Calculate the final specular light.
        specularLight = lightColor * specular;
    }
    
    // Compute the final color based on the ambient and diffuse colors.
    const float3 finalColor = ambientColor + diffuseLight + specularLight;
    
    // Forward the final pixle color to the rasterizer.
    return float4(finalColor, 1.0f);
}

