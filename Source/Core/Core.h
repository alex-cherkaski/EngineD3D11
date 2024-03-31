#pragma once
#include "PCH.h"
#include "CoreStructs.h"
#include "Macros.h"

struct VertexData;
struct GPUModelData;

class Window final
{
	SINGLETON(Window);

public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Initialize(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
	void ProcessInput();
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

private:
	bool m_isRunning = false;
};

class Renderer final
{
	SINGLETON(Renderer);

public:
	void Initialize();
	void Render();
	void Shutdown();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateViewPort();
	void Present(const GPUModelData& gpuModelData);

public:
	void CreateVertexBuffer(GPUModelData& gpuModelData);
	void CreateIndexBuffer(GPUModelData& gpuModelData);
	void CreateVertexShader(GPUModelData& gpuModelData);
	void CreateInputLayout(GPUModelData& gpuModelData);

	void CreatePixelShader(GPUModelData& gpuModelData);
	void CreateShaderResourceViewFromFile(GPUModelData& gpuModelData);
	void CreateSamplerState(GPUModelData& gpuModelData);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_id3d11Device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_id3d11DeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_idxgiSwapChain = nullptr;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_id3d11RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBufferTexture = nullptr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView = nullptr;

	D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_NULL;
};

