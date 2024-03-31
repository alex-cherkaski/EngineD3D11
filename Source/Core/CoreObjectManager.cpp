#include "PCH.h"
#include "CoreObjectManager.h"
#include "Core.h"

void CoreObjectManager::Initialize()
{
	CoreObject coreObject;

	// Set the shader of the core object.
	coreObject.gpuMeshData.VertexShaderPath = L"./Source/Shaders/SingleTextureShader.fx";
	coreObject.gpuMeshData.PixelShaderPath = L"./Source/Shaders/SingleTextureShader.fx";
	coreObject.gpuMeshData.TextureFilePath = L"./Resources/Images/PNG_transparency_demonstration_1.png";

	// Set the vertex data of the core object
	coreObject.gpuMeshData.Vertices = {
		{ XMFLOAT3 (0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3( 0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
	};

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateVertexBuffer(coreObject.gpuMeshData);
	renderer.CreateVertexShader(coreObject.gpuMeshData);
	renderer.CreateInputLayout(coreObject.gpuMeshData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(coreObject.gpuMeshData);
	renderer.CreateShaderResourceViewFromFile(coreObject.gpuMeshData);
	renderer.CreateSamplerState(coreObject.gpuMeshData);

	// Set the output merger stage topology settings.
	coreObject.gpuMeshData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_coreObjects.push_back(coreObject);
}

