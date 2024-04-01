#include "PCH.h"
#include "CoreObjectManager.h"
#include "Core.h"
#include "CoreGPUDataManager.h"

void CoreObjectManager::Initialize()
{
	// Generate a model data struct to fill with all the GPU relevant handles.
	CoreGPUDataManager& gpuDataManager = CoreGPUDataManager::GetInstanceWrite();
	CoreObject coreObject(gpuDataManager.GenerateGUID());
	GPUModelData& modelData = gpuDataManager.GetGPUModelDataWrite(coreObject.m_gpuMeshDataGUID);

	// Set the shader of the core object.
	modelData.VertexShaderPath = L"./Source/Shaders/SingleTextureShader.fx";
	modelData.PixelShaderPath = L"./Source/Shaders/SingleTextureShader.fx";
	modelData.TextureFilePath = L"./Resources/Images/PNG_transparency_demonstration_1.png";

	// Set the vertex data of the core object
	modelData.Vertices = {
		{ XMFLOAT3 (0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3( 0.5f,  0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
	};

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateVertexBuffer(modelData);
	renderer.CreateVertexShader(modelData);
	renderer.CreateInputLayout(modelData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(modelData);
	renderer.CreateShaderResourceViewFromFile(modelData);
	renderer.CreateSamplerState(modelData);

	// Set the output merger stage topology settings.
	modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_coreObjects.push_back(coreObject);
}

