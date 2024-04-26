#pragma once
#include "Macros.h"

struct MeshData;
struct ShaderData;
struct TextureData;

class Window final
{
	SINGLETON(Window);

public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Initialize(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
	void ProcessMessages();
	void Shutdown();

	HWND GetWindowHandle() const { return m_windowHandle; }
	LONG GetWindowWidth() const { return m_windowWidth; }
	LONG GetWindowHeight() const { return m_windowHeight; }
	LONG GetClientWidth() const { return m_clientWidth; }
	LONG GetClientHeight() const { return m_clientHeight; }

private:
	void RegisterEngineWindowClass(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
	void CreateEngineWindow(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
	void CalculateWindowDimensions();
	void CalculateFrameBufferDimensions();

private:
	HWND m_windowHandle = nullptr;
	LONG m_windowWidth = 0;
	LONG m_windowHeight = 0;
	LONG m_clientWidth = 0;
	LONG m_clientHeight = 0;
};

class Engine final
{
	SINGLETON(Engine);

public:
	void Initialize(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
	void Run();
	void Shutdown();

	bool IsRunning() const { return m_isRunning; }
	void IsRunning(bool isRunning) { m_isRunning = isRunning; }

private:
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void EndFrame();

	void InitializeCOM();
	void InitializePerformanceFrequency();
	void InitializeThreadAffinity();

	float ComputeDeltaTime();

private:
	LARGE_INTEGER m_performanceCounterFrequency = { };	// The constant number of ticks every second.
	LARGE_INTEGER m_lastFramePerformanceCount = { };

	bool m_isRunning = false;
};

class Renderer final
{
	SINGLETON(Renderer);

public:
	void Initialize();
	void PreRender();
	void Render();
	void Shutdown();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateBlendState();
	void CreateSamplerState();
	void CreateConstantBuffers();
	void CreateViewPort();
	void CreateRasterizerState();

	void Clear();
	void Present();

	void CreatePerMeshConstantBuffer();
	void CreatePerFrameConstantBuffer();
	void CreateProjectionConstantBuffer();

public:
	void CreateDefaultVertexBuffer(MeshData& meshData);
	//void CreateDynamicVertexBuffer(GPUModelData& gpuModelData);
	void CreateIndexBuffer(MeshData& meshData);

	void CreateInputLayout(ShaderData& shaderData);
	void CreateVertexShader(ShaderData& shaderData);
	void CreatePixelShader(ShaderData& shaderData);

	void CreateShaderResourceViewFromFile(TextureData& textureData);

	void UpdatePerMeshConstantBuffer(const XMFLOAT4X4& worldMatrix);

	void DrawMesh(const MeshData* meshData, const ShaderData* shaderData, const TextureData* textureData = nullptr);

private:
	void UpdatePerFrameConstantBuffer();
	void UpdateProjectionConstantBuffer();

private:
	//void WriteUITextData(const CoreObject& coreObject);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_id3d11Device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_id3d11DeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_idxgiSwapChain = nullptr;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_id3d11RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBufferTexture = nullptr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_id3d11ShaderResourceView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_id3d11DepthStencilView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_id3d11BlendState = nullptr;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_id3d11SamplerState = nullptr;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_id3d11RasterizerState = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbChangesPerMesh = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbChangesPerFrame = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbChangesRarely = nullptr;

	D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE;
};

