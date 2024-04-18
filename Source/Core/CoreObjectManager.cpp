#include "PCH.h"
#include "Core.h"
#include "CoreGPUDataManager.h"
#include "CoreObject.h"
#include "CoreObjectManager.h"
#include "MeshManager/MeshManager.h"

void CoreObjectManager::Initialize()
{
	//InitializeSprites();
	//InitializeUITexts();
	//Initialize3DModels();
	Initialize3DMeshes();
}

void CoreObjectManager::InitializeSprites()
{
	/*
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
	renderer.CreateDefaultVertexBuffer(modelData);
	renderer.CreateVertexShader(modelData);
	renderer.CreateInputLayout(modelData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(modelData);
	renderer.CreateShaderResourceViewFromFile(modelData);

	// Set the output merger stage topology settings.
	modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Add two instances of the core object to run with.
	//m_coreSprites.push_back(coreObject);
	//m_coreSprites.push_back(coreObject);

	//m_coreSprites[1].SetPosition({ 400.0f, 300.0f });
	*/
}

void CoreObjectManager::InitializeUITexts()
{
	/*
	// Generate a model data struct to fill with all the GPU relevant handles.
	CoreGPUDataManager& gpuDataManager = CoreGPUDataManager::GetInstanceWrite();
	CoreObject coreObject(gpuDataManager.GenerateGUID());
	GPUModelData& modelData = gpuDataManager.GetGPUModelDataWrite(coreObject.GetGPUDataGUID());

	modelData.GPUTextureDatas.reserve(16);

	// Set the shader of the core object.
	modelData.VertexShaderPath = L"./Source/Shaders/SingleBlendTextureShader.hlsl";
	modelData.PixelShaderPath = L"./Source/Shaders/SingleBlendTextureShader.hlsl";
	modelData.GPUTextureDatas[0].TextureFilePath = L"./Resources/Images/Font.dds";
	modelData.GPUTextureDatas[0].IsDDS = true;

	modelData.Message = "HELLO WORLD!";

	SetUIText(modelData);

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateDynamicVertexBuffer(modelData);
	renderer.CreateVertexShader(modelData);
	renderer.CreateInputLayout(modelData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(modelData);
	renderer.CreateShaderResourceViewFromFile(modelData.GPUTextureDatas[0]);

	// Set the output merger stage topology settings.
	modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Add two instances of the core object to run with.
	//m_coreUITexts.push_back(coreObject);
	//m_coreUITexts.push_back(coreObject);

	//m_coreUITexts[0].SetPosition({ 0.0f, 600.0f });
	//m_coreUITexts[1].SetPosition({ 0.0f, 0.0f });

	//m_coreUITexts[0].SetScale({ 2.0f, 1.0f });
	*/
}

void CoreObjectManager::Initialize3DModels()
{
	//// Generate a model data struct to fill with all the GPU relevant handles.
	//CoreGPUDataManager& gpuDataManager = CoreGPUDataManager::GetInstanceWrite();
	//CoreObject coreObject(gpuDataManager.GenerateGUID());
	//GPUModelData& modelData = gpuDataManager.GetGPUModelDataWrite(coreObject.GetGPUDataGUID());

	//modelData.GPUTextureDatas.resize(16);

	//// Set the shader of the core object.
	//modelData.VertexShaderPath = L"./Source/Shaders/MultiTexturingShader.hlsl";
	//modelData.PixelShaderPath = L"./Source/Shaders/MultiTexturingShader.hlsl";
	//modelData.GPUTextureDatas[0].TextureFilePath = L"./Resources/Images/Stone_Wall.jpg";
	//modelData.GPUTextureDatas[1].TextureFilePath = L"./Resources/Images/PNG_transparency_demonstration_1.png";

	//// Set the vertex data of the core object
	//modelData.Vertices = {
	//	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	//	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	//	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
	//};

	//// Set the index data of the core object.
	//modelData.Indices = {
	//	3, 1, 0, 2, 1, 3,
	//	6, 4, 5, 7, 4, 6,
	//	11, 9, 8, 10, 9, 11,
	//	14, 12, 13, 15, 12, 14,
	//	19, 17, 16, 18, 17, 19,
	//	22, 20, 21, 23, 20, 22
	//};

	//Renderer& renderer = Renderer::GetInstanceWrite();

	//// Create the vertex relevant GPU data.
	//renderer.CreateDefaultVertexBuffer(modelData);
	//renderer.CreateIndexBuffer(modelData);
	//renderer.CreateVertexShader(modelData);
	//renderer.CreateInputLayout(modelData);

	//// Create the pixel relevant GPU data.
	//renderer.CreatePixelShader(modelData);
	//renderer.CreateShaderResourceViewFromFile(modelData.GPUTextureDatas[0]);
	//renderer.CreateShaderResourceViewFromFile(modelData.GPUTextureDatas[1]);

	//// Set the output merger stage topology settings.
	//modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//// Add an instance of the core object to run with.
	//m_core3DModels.push_back(coreObject);
}

void CoreObjectManager::Initialize3DMeshes()
{
	// Generate a model data struct to fill with all the GPU relevant handles.
	CoreGPUDataManager& gpuDataManager = CoreGPUDataManager::GetInstanceWrite();
	CoreObject coreObject(gpuDataManager.GenerateGUID());
	GPUModelData& modelData = gpuDataManager.GetGPUModelDataWrite(coreObject.GetGPUDataGUID());

	modelData.GPUTextureDatas.resize(16);

	// Set the shader of the core object.
	modelData.VertexShaderPath = L"./Source/Shaders/LightingShader.hlsl";
	modelData.PixelShaderPath = L"./Source/Shaders/LightingShader.hlsl";
	modelData.GPUTextureDatas[0].TextureFilePath = L"./Resources/Textures/spot_texture.png";

	const MeshManager& meshManager = MeshManager::GetInstanceRead();
	const Mesh& mesh = meshManager.GetMesh(L"Spot");

	// Set the vertex data of the core object
	modelData.Vertices.resize(mesh.vertices.size());
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		modelData.Vertices[i].Position = mesh.vertices[i].position;
		modelData.Vertices[i].Normal = mesh.vertices[i].normal;
		modelData.Vertices[i].Texture = mesh.vertices[i].textureCoordinate;
	}

	// Set the index data of the core object.
	modelData.Indices = mesh.indices;

	Renderer& renderer = Renderer::GetInstanceWrite();

	// Create the vertex relevant GPU data.
	renderer.CreateDefaultVertexBuffer(modelData);
	renderer.CreateIndexBuffer(modelData);
	renderer.CreateVertexShader(modelData);
	renderer.CreateInputLayout(modelData);

	// Create the pixel relevant GPU data.
	renderer.CreatePixelShader(modelData);
	renderer.CreateShaderResourceViewFromFile(modelData.GPUTextureDatas[0]);

	// Set the output merger stage topology settings.
	modelData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Add an instance of the core object to run with.
	m_core3DModels.push_back(coreObject);
}

void CoreObjectManager::SetUIText(GPUModelData& gpuModelData)
{
	constexpr byte CHAR_WIDTH = 10;
	constexpr byte CHAR_HEIGHT = 20;

	constexpr float TEXTURE_WIDTH = 950;
	constexpr float TEXTURE_HEIGHT = 20;

	constexpr byte TEXTURE_OFFSET_X_START = ' ';

	constexpr byte TEXTURE_START_Y = 0;
	constexpr byte TEXTURE_END_Y = 1;

	// Construct a rectangular mesh for every character in the string.
	for (UINT i = 0; i < gpuModelData.Message.size(); ++i)
	{
		// The start of the vertex rectangle.
		const float vertexStartX = (float)(i * CHAR_WIDTH);

		// The end of the vertex rectangle.
		const float vertexEndX = (float)(i * CHAR_WIDTH + CHAR_WIDTH);

		const byte vertexStartY = 0;
		const byte vertexEndY = 20;

		// How many character widths into the buffer we should start reading the current char. 
		const byte currentCharMultiple = gpuModelData.Message[i] - TEXTURE_OFFSET_X_START;

		// The bottom left coordinate of the texture rectangle to read.
		const float textureStartX = currentCharMultiple * CHAR_WIDTH / TEXTURE_WIDTH;

		// The bottom right coordinate of the texture rectangle to read.
		const float textureEndX = (currentCharMultiple * CHAR_WIDTH + CHAR_WIDTH) / TEXTURE_WIDTH;

		// Create the vertex element data.
		/*gpuModelData.Vertices.push_back({ XMFLOAT3(vertexEndX,   vertexEndY, 0.5f),   XMFLOAT2(textureEndX,   TEXTURE_START_Y) });
		gpuModelData.Vertices.push_back({ XMFLOAT3(vertexEndX,   vertexStartY, 0.5f), XMFLOAT2(textureEndX,   TEXTURE_END_Y) });
		gpuModelData.Vertices.push_back({ XMFLOAT3(vertexStartX, vertexStartY, 0.5f), XMFLOAT2(textureStartX, TEXTURE_END_Y) });
		gpuModelData.Vertices.push_back({ XMFLOAT3(vertexStartX, vertexStartY, 0.5f), XMFLOAT2(textureStartX, TEXTURE_END_Y) });
		gpuModelData.Vertices.push_back({ XMFLOAT3(vertexStartX, vertexEndY, 0.5f),	  XMFLOAT2(textureStartX, TEXTURE_START_Y) });
		gpuModelData.Vertices.push_back({ XMFLOAT3(vertexEndX,   vertexEndY, 0.5f),   XMFLOAT2(textureEndX,   TEXTURE_START_Y) });*/
	}
}

