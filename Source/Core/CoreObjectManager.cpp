#include "PCH.h"
#include "Core.h"
#include "CoreGPUDataManager.h"
#include "CoreObject.h"
#include "CoreObjectManager.h"

void CoreObjectManager::Initialize()
{
	// Generate a model data struct to fill with all the GPU relevant handles.
	CoreGPUDataManager& gpuDataManager = CoreGPUDataManager::GetInstanceWrite();
	CoreObject coreObject(gpuDataManager.GenerateGUID());
	GPUModelData& modelData = gpuDataManager.GetGPUModelDataWrite(coreObject.GetGPUDataGUID());

	// Set the shader of the core object.
	modelData.VertexShaderPath = L"./Source/Shaders/SingleBlendTextureShader.hlsl";
	modelData.PixelShaderPath = L"./Source/Shaders/SingleBlendTextureShader.hlsl";
	modelData.TextureFilePath = L"./Resources/Images/PNG_transparency_demonstration_1.png";

	const Window& window = Window::GetInstanceRead();
	const float clientWidth = (float)window.GetClientWidth();
	const float clientHeight = (float)window.GetClientHeight();

	// Set the vertex data of the core object
	modelData.Vertices = {
		{ XMFLOAT3(300, 200, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(300, 000, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(000, 000, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(000, 000, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(000, 200, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(300, 200, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateVertexBuffer(modelData);
	renderer.CreateVertexShader(modelData);
	renderer.CreateInputLayout(modelData);
	renderer.CreateConstantBuffer(modelData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(modelData);
	renderer.CreateShaderResourceViewFromFile(modelData);
	renderer.CreateSamplerState(modelData);
	renderer.CreateBlendState(modelData);

	// Set the output merger stage topology settings.
	modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Add two instances of the core object to run with.
	m_coreObjects.push_back(coreObject);
	m_coreObjects.push_back(coreObject);

	m_coreObjects[1].SetPosition({ 400.0f, 300.0f });
}

