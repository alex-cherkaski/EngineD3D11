#include "PCH.h"
#include "CoreObjectManager.h"
#include "Core.h"

void CoreObjectManager::Initialize()
{
	CoreObject coreObject;

	// Set the shader of the core object.
	coreObject.gpuMeshData.VertexShaderPath = L"./Source/Shaders/SolidColorShader/SolidColorVertex.hlsl";
	coreObject.gpuMeshData.PixelShaderPath = L"./Source/Shaders/SolidColorShader/SolidColorPixel.hlsl";

	// Set the vertex data of the core object
	coreObject.gpuMeshData.Vertices = {
		{ XMFLOAT3 (0.5f,  0.5f, 0.5f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f) },

		{ XMFLOAT3( 0.5f,  0.5f, 0.5f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.5f) },
	};

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateVertexBuffer(coreObject.gpuMeshData);
	renderer.CreateVertexShader(coreObject.gpuMeshData);
	renderer.CreateInputLayout(coreObject.gpuMeshData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(coreObject.gpuMeshData);

	// Set the output merger stage topology settings.
	coreObject.gpuMeshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_coreObjects.push_back(coreObject);
}

