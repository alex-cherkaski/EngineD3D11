#include "PCH.h"
#include "Core.h"
#include "CoreObject.h"
#include "CoreObjectManager.h"
#include "CoreGPUDataManager.h"
#include "Logger/Logger.h"
#include "Camera/Camera.h"

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct = { nullptr };
	HDC deviceContext = nullptr;

	switch (message)
	{
	case WM_PAINT:
		// D3D11 will handle all the rendering.
		deviceContext = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;

	case WM_DESTROY:
		// Post a WM_QUIT message to the window message queue and stop the engine.
		PostQuitMessage(EXIT_SUCCESS);
		Engine::GetInstanceWrite().IsRunning(false);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			// Post a WM_QUIT message to the window message queue and stop the engine.
			PostQuitMessage(EXIT_SUCCESS);
			Engine::GetInstanceWrite().IsRunning(false);
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Window::Initialize(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	RegisterEngineWindowClass(hInstance, lpCmdLine, nShowCmd);
	CreateEngineWindow(hInstance, lpCmdLine, nShowCmd);
	CalculateWindowDimensions();
	CalculateFrameBufferDimensions();

	ShowWindow(m_windowHandle, nShowCmd);
}

void Window::ProcessInput()
{
	// Process window messages.
	MSG windowMessage = { nullptr };
	while (PeekMessage(&windowMessage, m_windowHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&windowMessage);	// Translate the message from virtual key to character message.
		DispatchMessage(&windowMessage);	// Forward the message to the window procedure callback.
	}
}

void Window::Shutdown()
{
}

void Window::RegisterEngineWindowClass(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	// Initialize the window class struct.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);				// The size of the window struct.
	windowClass.style = CS_HREDRAW | CS_VREDRAW;			// Redraw canvas on horizontal and vertical redraws.
	windowClass.lpfnWndProc = Window::WndProc;				// Window message callback function.
	windowClass.hInstance = hInstance;						// Pointer to application hIntance.
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);	// Load default cursor image.
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// The color to repaint the background of the window with.
	windowClass.lpszMenuName = nullptr;						// Resource name of the class menu.
	windowClass.lpszClassName = TEXT("Engine");				// Window class name.

	// Attempt to register the window class and error check its creation.
	const ATOM registerResult = RegisterClassEx(&windowClass);
	ENGINE_ASSERT(registerResult != 0, "Failed to register window class!");
}

void Window::CreateEngineWindow(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	const UINT windowStyleFlags = WS_OVERLAPPEDWINDOW;

	// Attempt to create the window.
	m_windowHandle = CreateWindow(
		TEXT("Engine"),				// Window class name.
		TEXT("Engine"),				// Window title.
		windowStyleFlags,			// Window flags.
		CW_USEDEFAULT,				// Default x position.
		CW_USEDEFAULT,				// Default y position.
		CW_USEDEFAULT,				// Default width.
		CW_USEDEFAULT,				// Default height.
		nullptr,					// Pointer to the parent window.
		nullptr,					// Pointer to the window menu.
		hInstance,					// Pointer to the application hInstance.
		nullptr
	);

	// Error check window creation.
	ENGINE_ASSERT(m_windowHandle, "Failed to create Engine window.\n");
}

void Window::CalculateWindowDimensions()
{
	// Get the current window dimensions;
	RECT windowRect = { 0 };
	GetWindowRect(m_windowHandle, &windowRect);

	// Cache the current window dimensions.
	m_windowWidth = windowRect.right - windowRect.left;
	m_windowHeight = windowRect.bottom - windowRect.top;
}

void Window::CalculateFrameBufferDimensions()
{
	// Get the current client area dimensions;
	RECT clientRect = { 0 };
	GetClientRect(m_windowHandle, &clientRect);

	// Cache the current client dimensions.
	m_clientWidth = clientRect.right - clientRect.left;
	m_clientHeight = clientRect.bottom - clientRect.top;
}

void Engine::Initialize(_In_ HINSTANCE hInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	// Initialize COM and error check.
	const HRESULT initCOMResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	ENGINE_ASSERT_HRESULT(initCOMResult);

	// Get the performance counter frequency.
	const BOOL queryPerformanceFrequencyResult = QueryPerformanceFrequency(&m_performanceCounterFrequency);
	ENGINE_ASSERT(queryPerformanceFrequencyResult, "Failed to QueryPerformanceFrequency.");

	// Set the thread's affinity to run on the same CPU core.
	const HANDLE currentThreadHandle = GetCurrentThread();
	const DWORD threadAffinityMask = 1 << GetCurrentProcessorNumber();
	const DWORD_PTR setThreadAffinityMaskResult = SetThreadAffinityMask(
		currentThreadHandle,	// The handle to the thread we are setting.
		threadAffinityMask		// The mask representing the CPU core the thread will be tied to.
	);

	// Error check main thread affinity mask setting.
	ENGINE_ASSERT(queryPerformanceFrequencyResult, "Failed to SetThreadAffinityMask.");

	Logger::GetInstanceWrite().Initialize();
	Window::GetInstanceWrite().Initialize(hInstance, lpCmdLine, nShowCmd);
	Renderer::GetInstanceWrite().Initialize();
}

void Engine::Run()
{
	Setup();
	while (m_isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Engine::Shutdown()
{	
	Window::GetInstanceWrite().Shutdown();
	Logger::GetInstanceWrite().Shutdown();
	CoUninitialize(); // Shutdown COM.
}

void Engine::Setup()
{
	CoreObjectManager::GetInstanceWrite().Initialize();

	m_isRunning = true;
}

void Engine::ProcessInput()
{
	Window::GetInstanceWrite().ProcessInput();
}

void Engine::Update()
{
	const float delatTime = ENGINE_CLAMP_F(ComputeDeltaTime(), 0.0f, 0.05f);

	Camera& camera = Camera::GetInstanceWrite();
	camera.Update(delatTime);
}

void Engine::Render()
{
	static Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.Render();
}

float Engine::ComputeDeltaTime()
{
	// Get the performance counter value for the current frame.
	LARGE_INTEGER currentPerformanceCounter = { };
	const BOOL queryPerformanceCounterResult = QueryPerformanceCounter(&currentPerformanceCounter);
	ENGINE_ASSERT(queryPerformanceCounterResult, "Failed to QueryPerformanceCounter.");

	// Calculate the delta time.
	const float deltaTime =
		(float)(currentPerformanceCounter.QuadPart - m_lastFramePerformanceCount.QuadPart)
		/ (float)(m_performanceCounterFrequency.QuadPart);

	// Cache the current frame's performance count for the next frame.
	m_lastFramePerformanceCount = currentPerformanceCounter;

#ifdef _DEBUG // Clamp the deltaTime in case of breakpoints.
	return ENGINE_CLAMP_F(deltaTime, 0.0f, 0.05f);
#else
	return deltaTime;
#endif // _DEBUG
}

void Renderer::Initialize()
{
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilView();
	CreateBlendState();
	CreateSamplerState();
	CreateConstantBuffer();
	CreateMVPConstantBuffers();
	CreateViewPort();

	UpdateViewMatrix();
	UpdateProjectionMatrix();
	UpdateViewProjectionMatrix();

	Logger::GetInstanceWrite().Log(Logger::Message, "Successfully initialized renderer.");
}

void Renderer::Render()
{
	Clear();

	static CoreObjectManager& coreObjectManager = CoreObjectManager::GetInstanceWrite();
	for (const CoreObject& coreObject : coreObjectManager.GetCoreSpritesRead())
	{
		DrawSprites(coreObject);
	}
	for (const CoreObject& coreObject : coreObjectManager.GetCoreUITextsRead())
	{
		DrawUITexts(coreObject);
	}
	for (const CoreObject& coreObject : coreObjectManager.GetCore3DModelsRead())
	{
		Draw3DModels(coreObject);
	}

	Present();
}

void Renderer::Shutdown()
{
}

void Renderer::CreateDeviceAndSwapChain()
{
	const Window& window = Window::GetInstanceRead();

	// The set of driver types to try and create in descending order.
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,	// Hardware accelerated driver.
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_WARP,		// An optimized software rasterizer.
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE	// Accurate but slow software rasterizer.
	};

	// The set of feature levels to try and create a graphics device with, in descending order.
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0
	};

	// Initialize the swap chain description struct.
	DXGI_SWAP_CHAIN_DESC swapChainDescription = { 0 };
	swapChainDescription.BufferCount = 1;												// The number of buffers in the swap chain. Windowed swap chains have the desktop as the front buffer.
	swapChainDescription.BufferDesc.Width = window.GetClientWidth();					// The width of the back buffer.
	swapChainDescription.BufferDesc.Height = window.GetClientHeight();					// The height of the back buffer.
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;	// The data format of the back buffer.
	swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;							// The expected refresh rate of the back buffer.
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;						// The expected refresh rate of the back buffer.
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;					// How the back buffer will be used.
	swapChainDescription.OutputWindow = window.GetWindowHandle();						// The output window of the swap chain.
	swapChainDescription.Windowed = TRUE;												// Is the swap chain initially windowed.
	swapChainDescription.SampleDesc.Count = 1;											// The number of multi-samples to take.
	swapChainDescription.SampleDesc.Quality = 0;										// The quality of the multi-sample taken.

	// The device creation flags.
#ifdef _DEBUG
	const UINT deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
#else
	const UINT deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif // _DEBUG

	// Attempt to create the device and swap chain.
	HRESULT createDeviceAndSwapChainResult = E_FAIL;
	for (D3D_DRIVER_TYPE driverType : driverTypes)
	{
		createDeviceAndSwapChainResult = D3D11CreateDeviceAndSwapChain(
			nullptr,								// Pointer to the graphics adapter. Null means use default.
			driverType,								// The driver type to create the device with.
			nullptr,								// Pointer to the software module used to create a software renderer.
			deviceFlags,							// The device creation flags.
			featureLevels,							// The array of feature levels to attempt to create the device with.
			ARRAYSIZE(featureLevels),				// The number of entires in the feature level array.
			D3D11_SDK_VERSION,						// The SDK version to create the device with.
			&swapChainDescription,					// The swap chain descriptor struct.
			m_idxgiSwapChain.GetAddressOf(),		// Pointer to the resulting swap chain interface.
			m_id3d11Device.GetAddressOf(),			// Pointer to the resulting device interface.
			&m_featureLevel,						// The feature level the device was created with.
			m_id3d11DeviceContext.GetAddressOf()	// Pointer to the resulting device context interface.
		);

		// Check if the device creation succeeded.
		if (SUCCEEDED(createDeviceAndSwapChainResult))
		{
			Logger::GetInstanceWrite().Log(Logger::Message, "Successfully create D3D device.");
			m_driverType = driverType;
			break;
		}
	}

	// Error check device, device context, and swap chain creation.
	ENGINE_ASSERT_HRESULT(createDeviceAndSwapChainResult);
}

void Renderer::CreateRenderTargetView()
{
	// Get the swap chain's back buffer.
	const HRESULT getBufferResult = m_idxgiSwapChain->GetBuffer(
		0,												// The index of the buffer we are retrieving.
		__uuidof(ID3D11Texture2D),						// The universal unique identifier of the interface we want to get the resource as.
		(void**)m_backBufferTexture.GetAddressOf()		// Pointer to the resulting interface.
	);

	// Error check back buffer retrieval.
	ENGINE_ASSERT_HRESULT(getBufferResult);

	// Attempt to create the render target view.
	const HRESULT createRenderTargetViewResult = m_id3d11Device->CreateRenderTargetView(
		m_backBufferTexture.Get(),					// Pointer to the resource interface we want to create a render target view for.
		nullptr,									// Optional pointer to the render target view description struct.
		m_id3d11RenderTargetView.GetAddressOf()		// Pointer to the resulting render target view interface.
	);

	// Error check render target view creation.
	ENGINE_ASSERT_HRESULT(createRenderTargetViewResult);
}

void Renderer::CreateDepthStencilView()
{
	const Window& window = Window::GetInstanceRead();

	// Initialize the depth stencil texture descriptor struct.
	D3D11_TEXTURE2D_DESC depthStencilTextureDescriptor = { 0 };
	depthStencilTextureDescriptor.Width = window.GetClientWidth();			// The width of the depth stencil buffer.
	depthStencilTextureDescriptor.Height = window.GetClientHeight();		// The height of the depth stencil buffer.
	depthStencilTextureDescriptor.MipLevels = 1;							// The maximum number of mipmap levels for this texture.
	depthStencilTextureDescriptor.ArraySize = 1;							// The number of textures in the array to create.
	depthStencilTextureDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT; // The format of each depth stencil texture.
	depthStencilTextureDescriptor.SampleDesc.Count = 1;						// The number of multi samples to take.
	depthStencilTextureDescriptor.SampleDesc.Quality = 0;					// The quality of the multisamples to take.
	depthStencilTextureDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;	// The GPU will have read and write access to this texture.
	depthStencilTextureDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL; // How the texture should be bound.
	depthStencilTextureDescriptor.CPUAccessFlags = 0;						// The CPU will not need read or write access to this flag.
	depthStencilTextureDescriptor.MiscFlags = 0;							// No additional special properties.

	// Attempt to create the depth stencil texture.
	const HRESULT createTexture2DResult = m_id3d11Device->CreateTexture2D(
		&depthStencilTextureDescriptor,				// Depth stencil texture descriptor.
		nullptr,									// Optional pointer to struct with initial data.
		m_depthStencilTexture.GetAddressOf()		// Pointer to the returned interface of the texture.
	);

	// Error check depth stencil texture creation.
	ENGINE_ASSERT_HRESULT(createTexture2DResult);

	// Initialize the depth stencil view descriptor struct.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescriptor = { };
	depthStencilViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT; // The format of the texture this view is for.
	depthStencilViewDescriptor.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D; // How the resource will be accessed.
	depthStencilViewDescriptor.Texture2D.MipSlice = 0;

	// Attempt to create the depth stencil view.
	const HRESULT createDepthStencilViewResult = m_id3d11Device->CreateDepthStencilView(
		m_depthStencilTexture.Get(),				// Pointer to resource interface the view is being created for.
		&depthStencilViewDescriptor,				// Pointer to the depth stencil view descriptor struct.
		m_id3d11DepthStencilView.GetAddressOf()		// Pointer to the resulting depth stencil view interface.
	);

	// Error check depth stencil view creation.
	ENGINE_ASSERT_HRESULT(createDepthStencilViewResult);
}

void Renderer::CreateConstantBuffer()
{
	// Initialize constant buffer descriptor struct.
	D3D11_BUFFER_DESC constantBufferDescriptor = { 0 };
	constantBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;	// How the buffer should be bound to the pipeline.
	constantBufferDescriptor.ByteWidth = sizeof(XMMATRIX);								// The size of the constant buffer.
	constantBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;					// How the buffer will be read from and written to. This one will require read and write access by the GPU.

	// Attempt to create the constant buffer.
	const HRESULT createBufferResult = m_id3d11Device->CreateBuffer(
		&constantBufferDescriptor,					// Pointer to the buffer descriptor struct.
		nullptr,									// Pointer to optional initial data struct.
		m_id3d11ConstantBuffer.GetAddressOf()		// Pointer to resulting interface.
	);

	// Error check constant buffer creation.
	ENGINE_ASSERT_HRESULT(createBufferResult);
}

void Renderer::CreateBlendState()
{
	// Initialize blend state descriptor struct.
	D3D11_BLEND_DESC blendDescriptor = { 0 };
	blendDescriptor.RenderTarget[0].BlendEnable = TRUE;										// Is blending enabled.
	blendDescriptor.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;			// How to combined the source and destination images.
	blendDescriptor.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;			// The operation to perform on the pixel shader RGB output.
	blendDescriptor.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;				// The operation to perform on the RGB of the current render target.
	blendDescriptor.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;		// How to combine the source and destination alpha values.
	blendDescriptor.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;			// The operation to perform on the pixel shader alpha value.
	blendDescriptor.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;			// The operation to perform on the current render target alpha value.
	blendDescriptor.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;	// Which of RGBA to write the blend to. 

	// Attempt to create blend state.
	const HRESULT createBlendStateResult = m_id3d11Device->CreateBlendState(
		&blendDescriptor,						// Pointer to the blend state descriptor struct.
		m_id3d11BlendState.GetAddressOf()		// Pointer to the resulting blend state interface.
	);

	// Error check blend state creation.
	ENGINE_ASSERT_HRESULT(createBlendStateResult);

	// Blend factor to modulate the values of pixel shader, render target, or both.
	constexpr float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set the blend state of the output merger stage.
	m_id3d11DeviceContext->OMSetBlendState(
		m_id3d11BlendState.Get(),		// Pointer to the blend state interface to set.
		blendFactor,					// The blend factor for D3D11_BLEND_BLEND_FACTOR or D3D11_BLEND_INV_BLEND_FACTOR.
		0xffffffff						// Which samples get updated in the currently active render target.
	);
}

void Renderer::CreateViewPort()
{
	const Window& window = Window::GetInstanceRead();

	// Create the viewport description struct.
	D3D11_VIEWPORT viewPortDescription = { 0 };
	viewPortDescription.Width = (FLOAT)window.GetClientWidth();
	viewPortDescription.Height = (FLOAT)window.GetClientHeight();
	viewPortDescription.TopLeftX = 0.0f;
	viewPortDescription.TopLeftY = 0.0f;
	viewPortDescription.MinDepth = 0.0f;
	viewPortDescription.MaxDepth = 1.0f;

	// Set the window viewport.
	m_id3d11DeviceContext->RSSetViewports(
		1,						// The number of view ports to set.
		&viewPortDescription	// Pointer to the array of viewport descriptors.
	);
}

void Renderer::Clear()
{
	// Clear the back buffer through the render target view.
	constexpr FLOAT clearColor[4] = { 0.09f, 0.09f, 0.09f, 1.0f };
	m_id3d11DeviceContext->ClearRenderTargetView(m_id3d11RenderTargetView.Get(), clearColor);

	// Clear the depth stencil buffer through the render target view.
	constexpr UINT clearFlags = D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL;
	m_id3d11DeviceContext->ClearDepthStencilView(
		m_id3d11DepthStencilView.Get(),		// Pointer to the depth stencil view interface to clear the depth stencil buffer.
		clearFlags,							// Flags stating what parts of the depth stencil texture to clear.
		1.0f,								// Value to clear the depth part of the texture with.
		0									// Value to clear the stencil part of the texture with.
	);
}

void Renderer::DrawSprites(const CoreObject& coreObject)
{
	static CoreGPUDataManager& coreGPUDataManager = CoreGPUDataManager::GetInstanceWrite();
	const GPUModelData& gpuModelData = coreGPUDataManager.GetGPUModelDataRead(coreObject.GetGPUDataGUID());

	// Calculate each vertex element stride and position.
	const UINT stride = sizeof(VertexData);
	const UINT offset = 0;

	// Set the input layout for the current model
	m_id3d11DeviceContext->IASetInputLayout(gpuModelData.InputLayout.Get());

	// Set the vertex buffer for the current model.
	m_id3d11DeviceContext->IASetVertexBuffers(
		0,											// The slot to be used for this set of vertex buffer.
		1,											// The number of vertex buffers in the vertex buffer array.
		gpuModelData.VertexBuffer.GetAddressOf(),	// The array of vertex buffers to set.
		&stride,									// The stride from one vertex element to the next.
		&offset										// The offset until the first vertex element in the buffer.
	);

	// Set the primitive topology setting to draw the vertices with.
	m_id3d11DeviceContext->IASetPrimitiveTopology(gpuModelData.PrimitiveTopology);

	const XMMATRIX mvp = XMMatrixMultiplyTranspose(coreObject.GetWorldMatrix(), m_viewProjectionMatrix);

	// Update the constant buffer with the mvp matrix.
	m_id3d11DeviceContext->UpdateSubresource(
		m_id3d11ConstantBuffer.Get(),		// Pointer to interface of the GPU buffer we want to copy to.
		0,									// Index of the subresource we want to update.
		nullptr,							// Optional pointer to the destination resource box that defines what portion of the subresource should be updated.
		&mvp,								// Pointer to the buffer of data we wish to copy to the subresource.
		sizeof(XMMATRIX::r),				// The size of one row of the source data.
		0									// The size of the depth slice of the source data.
	);

	// Set the constant buffer.
	m_id3d11DeviceContext->VSSetConstantBuffers(0, 1, m_id3d11ConstantBuffer.GetAddressOf());

	// Set the vertex shader to draw with.
	m_id3d11DeviceContext->VSSetShader(
		gpuModelData.VertexShader.Get(),	// Pointer to the vertex shader interface to set.
		nullptr,							// Optional array of ID3D11ClassInstance.
		0									// Number of entries in the optional array of ID3D11ClassInstance.
	);

	// Set the pixel shader to draw with.
	m_id3d11DeviceContext->PSSetShader(
		gpuModelData.PixelShader.Get(),		// Pointer to the pixel shader interface to set.
		nullptr,							// Optional array of ID3D11ClassInstance.
		0									// Number of entries in the optional array of ID3D11ClassInstance.
	);

	// Set the pixel shader resource view to use to set the shader texture.
	m_id3d11DeviceContext->PSSetShaderResources(
		0,																	// The index of the shader resource we want to set.
		1,																	// The number of shader resources in the share resource array.
		gpuModelData.GPUTextureDatas[0].ShaderResourceView.GetAddressOf()	// Pointer to the array of shader resources.
	);

	// Set the pixel shader sampler state to sample the texture.
	m_id3d11DeviceContext->PSSetSamplers(
		0,										// The index of the shader sampler to set.
		1,										// The number of sampler states in the sampler state array.
		m_id3d11SamplerState.GetAddressOf()		// Pointer to the array of sampler states.
	);

	// Set the render target view that will be sued to access and write to the back buffer.
	m_id3d11DeviceContext->OMSetRenderTargets(
		1,											// The number of render target views to set.
		m_id3d11RenderTargetView.GetAddressOf(),	// The array of pointers to the render target views to set.
		m_id3d11DepthStencilView.Get()				// Optional pointerto depth stencil view interface.
	);

	// Draw the model.
	m_id3d11DeviceContext->Draw(
		(UINT)gpuModelData.Vertices.size(),		// The number of vertices to draw.
		0										// The index of the first vertex to draw.
	);
}

void Renderer::DrawUITexts(const CoreObject& coreObject)
{
	WriteUITextData(coreObject);
	DrawSprites(coreObject);
}

void Renderer::Draw3DModels(const CoreObject& coreObject)
{
	static CoreGPUDataManager& coreGPUDataManager = CoreGPUDataManager::GetInstanceWrite();
	const GPUModelData& gpuModelData = coreGPUDataManager.GetGPUModelDataRead(coreObject.GetGPUDataGUID());

	// Calculate each vertex element stride and position.
	const UINT stride = sizeof(VertexData);
	const UINT offset = 0;

	// Set the input layout for the current model
	m_id3d11DeviceContext->IASetInputLayout(gpuModelData.InputLayout.Get());

	// Set the vertex buffer for the current model.
	m_id3d11DeviceContext->IASetVertexBuffers(
		0,											// The slot to be used for this set of vertex buffer.
		1,											// The number of vertex buffers in the vertex buffer array.
		gpuModelData.VertexBuffer.GetAddressOf(),	// The array of vertex buffers to set.
		&stride,									// The stride from one vertex element to the next.
		&offset										// The offset until the first vertex element in the buffer.
	);

	// Set the index buffer for the current model.
	m_id3d11DeviceContext->IASetIndexBuffer(
		gpuModelData.IndexBuffer.Get(),			// Pointer to the index buffer interface to set.
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT,		// The format of the index buffer to set.
		0										// Offset from the start of the index buffer to the first index to use.
	);

	// Set the primitive topology setting to draw the vertices with.
	m_id3d11DeviceContext->IASetPrimitiveTopology(gpuModelData.PrimitiveTopology);

	// Update the world matrix constant data.
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.7f, 0.7f);
	const XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	const XMMATRIX worldMatrix = XMMatrixMultiplyTranspose(
		coreObject.GetWorldMatrix(),
		XMMatrixMultiply(rotationMatrix, translationMatrix)
	);
	m_id3d11DeviceContext->UpdateSubresource(
		m_modelMatrixBuffer.Get(),		// Pointer to interface of the GPU buffer we want to copy to.
		0,								// Index of the subresource we want to update.
		nullptr,						// Optional pointer to the destination resource box that defines what portion of the subresource should be updated.
		&worldMatrix,					// Pointer to the buffer of data we wish to copy to the subresource.
		sizeof(XMMATRIX::r),			// The size of one row of the source data.
		0								// The size of the depth slice of the source data.
	);

	// Update the view matrix constant data.
	static const Camera& camera = Camera::GetInstanceRead();
	const XMMATRIX viewMatrix = XMMatrixTranspose(camera.GetViewMatrix());
	m_id3d11DeviceContext->UpdateSubresource(
		m_viewMatrixBuffer.Get(),		// Pointer to interface of the GPU buffer we want to copy to.
		0,								// Index of the subresource we want to update.
		nullptr,						// Optional pointer to the destination resource box that defines what portion of the subresource should be updated.
		&viewMatrix,					// Pointer to the buffer of data we wish to copy to the subresource.
		sizeof(XMMATRIX::r),			// The size of one row of the source data.
		0								// The size of the depth slice of the source data.
	);

	// Update the projection matrix constant data.
	const XMMATRIX projectionMatrix = XMMatrixTranspose(m_projectionMatrix);
	m_id3d11DeviceContext->UpdateSubresource(
		m_projectionMatrixBuffer.Get(),		// Pointer to interface of the GPU buffer we want to copy to.
		0,									// Index of the subresource we want to update.
		nullptr,							// Optional pointer to the destination resource box that defines what portion of the subresource should be updated.
		&projectionMatrix,					// Pointer to the buffer of data we wish to copy to the subresource.
		sizeof(XMMATRIX::r),				// The size of one row of the source data.
		0									// The size of the depth slice of the source data.
	);

	// Set the world matrix constant buffer.
	m_id3d11DeviceContext->VSSetConstantBuffers(
		0,										// The slot index that we are setting.
		1,										// The number of buffers that we are setting.
		m_modelMatrixBuffer.GetAddressOf()		// Pointer to the array of constant buffers to set.
	);

	// Set the view matrix constant buffer.
	m_id3d11DeviceContext->VSSetConstantBuffers(
		1,										// The slot index that we are setting.
		1,										// The number of buffers that we are setting.
		m_viewMatrixBuffer.GetAddressOf()		// Pointer to the array of constant buffers to set.
	);

	// Set the projection matrix constant buffer.
	m_id3d11DeviceContext->VSSetConstantBuffers(
		1,										// The slot index that we are setting.
		2,										// The number of buffers that we are setting.
		m_viewMatrixBuffer.GetAddressOf()		// Pointer to the array of constant buffers to set.
	);

	// Set the vertex shader to draw with.
	m_id3d11DeviceContext->VSSetShader(
		gpuModelData.VertexShader.Get(),	// Pointer to the vertex shader interface to set.
		nullptr,							// Optional array of ID3D11ClassInstance.
		0									// Number of entries in the optional array of ID3D11ClassInstance.
	);

	// Set the pixel shader to draw with.
	m_id3d11DeviceContext->PSSetShader(
		gpuModelData.PixelShader.Get(),		// Pointer to the pixel shader interface to set.
		nullptr,							// Optional array of ID3D11ClassInstance.
		0									// Number of entries in the optional array of ID3D11ClassInstance.
	);

	for (size_t i = 0; i < gpuModelData.GPUTextureDatas.size() && gpuModelData.GPUTextureDatas[i].ShaderResourceView != nullptr; ++i)
	{
		// Set the pixel shader resource view to use to set the shader texture.
		m_id3d11DeviceContext->PSSetShaderResources(
			(UINT)i,															// The index of the shader resource we want to set.
			1,																	// The number of shader resources in the share resource array.
			gpuModelData.GPUTextureDatas[i].ShaderResourceView.GetAddressOf()	// Pointer to the array of shader resources.
		);
	}

	// Set the pixel shader sampler state to sample the texture.
	m_id3d11DeviceContext->PSSetSamplers(
		0,										// The index of the shader sampler to set.
		1,										// The number of sampler states in the sampler state array.
		m_id3d11SamplerState.GetAddressOf()		// Pointer to the array of sampler states.
	);

	// Set the render target view that will be used to access and write to the back buffer.
	m_id3d11DeviceContext->OMSetRenderTargets(
		1,											// The number of render target views to set.
		m_id3d11RenderTargetView.GetAddressOf(),	// The array of pointers to the render target views to set.
		m_id3d11DepthStencilView.Get()				// Optional pointer to depth stencil view interface.
	);

	// Draw the model.
	m_id3d11DeviceContext->DrawIndexed(
		(UINT)gpuModelData.Indices.size(),	// The number of indices to draw.
		0,									// The index of the first index value.
		0									// Value added to each index before reading from the vertex buffer.
	);
}

void Renderer::Present()
{
	// Present the contents of the back buffer to the screen.
	m_idxgiSwapChain->Present(0, 0);
}

void Renderer::CreateDefaultVertexBuffer(GPUModelData& gpuModelData)
{
	// Initialize the vertex buffer descriptor struct.
	D3D11_BUFFER_DESC vertexBufferDecription = { 0 };
	vertexBufferDecription.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;								// How the buffer will be accessed.
	vertexBufferDecription.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;					// How the buffer should be set by the driver.
	vertexBufferDecription.ByteWidth = (UINT)(gpuModelData.Vertices.size() * sizeof(VertexData));	// The size of the buffer.

	// Initialize the vertex buffer initial data struct.
	D3D11_SUBRESOURCE_DATA initialBufferData = { nullptr };
	initialBufferData.pSysMem = gpuModelData.Vertices.data(); // Initial vertex data.

	// Attempt to create the vertex buffer.
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	const HRESULT createBufferResult = m_id3d11Device->CreateBuffer(
		&vertexBufferDecription,					// The buffer description struct.
		&initialBufferData,							// The initial buffer data.
		gpuModelData.VertexBuffer.GetAddressOf()	// Pointer to the resulting buffer interface.
	);

	// Error check vertex buffer creation.
	ENGINE_ASSERT_HRESULT(createBufferResult);
}

void Renderer::CreateDynamicVertexBuffer(GPUModelData& gpuModelData)
{
	// Initialize the vertex buffer descriptor struct.
	D3D11_BUFFER_DESC vertexBufferDecriptor = { 0 };
	vertexBufferDecriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;									// How the buffer will be accessed.
	vertexBufferDecriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;			// What sort of access the CPU needs to the vertex buffer on the GPU.
	vertexBufferDecriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;					// How the buffer should be set by the driver.
	vertexBufferDecriptor.ByteWidth = (UINT)(6 * 24 * sizeof(VertexData));								// The size of the buffer.

	// Attempt to create the vertex buffer.
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	const HRESULT createBufferResult = m_id3d11Device->CreateBuffer(
		&vertexBufferDecriptor,						// The buffer description struct.
		nullptr,									// The initial buffer data. We will update the buffer contents dynamically at runtime.
		gpuModelData.VertexBuffer.GetAddressOf()	// Pointer to the resulting buffer interface.
	);

	// Error check vertex buffer creation.
	ENGINE_ASSERT_HRESULT(createBufferResult);
}

void Renderer::CreateIndexBuffer(GPUModelData& gpuModelData)
{
	// Initialize the index buffer descriptor struct.
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;							// The GPU will have read and write access to the buffer.
	indexBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;				// How the buffer will be used by the driver.
	indexBufferDescriptor.ByteWidth = (UINT)(sizeof(UINT) * gpuModelData.Indices.size());	// The size of the allocated index buffer.

	// Initialize the initial index data struct.
	D3D11_SUBRESOURCE_DATA initialIndexData = { nullptr };
	initialIndexData.pSysMem = gpuModelData.Indices.data();

	// Attempt to create the initial index buffer.
	const HRESULT createBufferResult = m_id3d11Device->CreateBuffer(
		&indexBufferDescriptor,						// The index buffer descriptor struct.
		&initialIndexData,							// Pointer to struct with initial index buffer data.
		gpuModelData.IndexBuffer.GetAddressOf()		// Pointer to the returned vertex buffer interface.
	);

	// Error check index buffer creation.
	ENGINE_ASSERT_HRESULT(createBufferResult);
}

void Renderer::CreateVertexShader(GPUModelData& gpuModelData)
{
	// Shader compilation flags.
#ifdef _DEBUG
	const DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	const DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif // _DEBUG
	
	// Attempt to compile the vertex shader.
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	const HRESULT compileResult = D3DCompileFromFile(
		gpuModelData.VertexShaderPath.c_str(),		// Shader source path.
		nullptr,									// Optional shader macros array.
		nullptr,									// Optional D3D11Include pointer.
		"VS_Main",									// Shader entry point.
		"vs_5_0",									// Vertex shader standards to compile against.
		shaderFlags,								// Shader compilation flags.
		0,											// Additional misc flags.
		gpuModelData.VertexBlob.GetAddressOf(),		// Compiled vertex shade code.
		errorBlob.GetAddressOf()					// Error blob on error.
	);

	// Error check shader compilation.
	ENGINE_ASSERT_HRESULT_SHADER(compileResult, errorBlob);
	
	// Attempt to create the vertex shader.
	const HRESULT createVertexShaderResult = m_id3d11Device->CreateVertexShader(
		gpuModelData.VertexBlob->GetBufferPointer(),	// Pointer to compiled vertex shader byte code.
		gpuModelData.VertexBlob->GetBufferSize(),		// The size of the compiled byte code.
		nullptr,										// Optional ID3D11ClassLinkage pointer.
		gpuModelData.VertexShader.GetAddressOf()		// Pointer to the resulting shader interface.
	);

	// Error check vertex shader creation.
	ENGINE_ASSERT_HRESULT(createVertexShaderResult);
}

void Renderer::CreateInputLayout(GPUModelData& gpuModelData)
{
	// Array of vertex element descriptors.
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{
			"POSITION",													// The semantic name of the vertex element.
			0,															// The semantic index of the vertex element.
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,					// The format of the semantic element.
			0,															// The index of the vertex buffer for this element.
			0,															// The offset of this element in the vertex buffer.
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,	// The element is a per vertex element.
			0															// Instance data step rate. 0 for vertex data.
		},
		{
			"TEXCOORD",													// The semantic name of the vertex element.
			0,															// The semantic index of the vertex element.
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,						// The format of the semantic element.
			0,															// The index of the vertex buffer for this element.
			(UINT)sizeof(VertexData::m_position),						// The offset of this element in the vertex buffer.
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,	// The element is a per vertex element.
			0															// Instance data step rate. 0 for vertex data.
		}
	};
	
	// Attempt to create the input layout.
	const HRESULT createInputLayoutResult = m_id3d11Device->CreateInputLayout(
		inputElements,									// Array of vertex shader input elements.
		ARRAYSIZE(inputElements),						// Number of elements in the input element array.
		gpuModelData.VertexBlob->GetBufferPointer(),	// Pointer to the compiled shader byte code.
		gpuModelData.VertexBlob->GetBufferSize(),		// The size of the compiled shader byte code.
		gpuModelData.InputLayout.GetAddressOf()			// Pointer to the resulting input layout interface.
	);

	// Error check input layout creation.
	ENGINE_ASSERT_HRESULT(createInputLayoutResult);
}

void Renderer::CreatePixelShader(GPUModelData& gpuModelData)
{
	// Shader compilation flags.
#ifdef _DEBUG
	const DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	const DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif // _DEBUG

	// Attempt to compile the pixel shader.
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	const HRESULT compileResult = D3DCompileFromFile(
		gpuModelData.PixelShaderPath.c_str(),		// Path to pixel shader source code.
		nullptr,									// Optional array of D3D_SHADER_MACRO.
		nullptr,									// Optional ID3DInclude pointer.
		"PS_Main",									// Shader entry point.
		"ps_5_0",									// Shader standard to compile against.
		shaderFlags,								// Shader compilation flags.
		0,											// Additional misc compilation flags.
		gpuModelData.PixelBlob.GetAddressOf(),		// Pointer to the resulting pixel shader interface.
		errorBlob.GetAddressOf()					// Shader compilation error data.
	);

	// Error check shader compilation.
	ENGINE_ASSERT_HRESULT_SHADER(compileResult, errorBlob);

	// Attempt to create the pixel shader.
	const HRESULT createPixelShaderResult = m_id3d11Device->CreatePixelShader(
		gpuModelData.PixelBlob->GetBufferPointer(),		// Pointer to compiled pixel shader byte code.
		gpuModelData.PixelBlob->GetBufferSize(),		// The size of the compiled shader byte code.
		nullptr,										// Optional ID3D11ClassLinkage pointer.
		gpuModelData.PixelShader.GetAddressOf()			// Pointer to resulting pixel shader interface.
	);

	// Error check pixel shader creation.
	ENGINE_ASSERT_HRESULT(createPixelShaderResult);
}

void Renderer::CreateShaderResourceViewFromFile(GPUTextureData& gpuTextureData)
{
	if (gpuTextureData.IsDDS)
	{
		// Attempt to create the DDS texture.
		const HRESULT createDDCTextyreFromFileResult = CreateDDSTextureFromFile(
			m_id3d11Device.Get(),										// The device to use to create the texture view.
			gpuTextureData.TextureFilePath.c_str(),						// The path to the DDS texture we wish to create.
			(ID3D11Resource**)gpuTextureData.Texture2D.GetAddressOf(),	// Optional pointer to fill the texture interface.
			gpuTextureData.ShaderResourceView.GetAddressOf()			// Pointer to the returned shader resource.
		);

		// Error check shader resource view creation.
		ENGINE_ASSERT_HRESULT(createDDCTextyreFromFileResult);

		// Cache texture description struct.
		gpuTextureData.Texture2D->GetDesc(&gpuTextureData.Texture2DDescriptor);
	}

	else
	{
		// Attempt to create the non DDS texture.
		const HRESULT createWICTextureFromFileResult = CreateWICTextureFromFile(
			m_id3d11Device.Get(),										// The device to use when creating the texture view.
			m_id3d11DeviceContext.Get(),								// The device context to use when creating the texture view.
			gpuTextureData.TextureFilePath.c_str(),						// The path to the texture the view should be create for.
			(ID3D11Resource**)gpuTextureData.Texture2D.GetAddressOf(),	// Optional pointer to the resulted texture interface.
			gpuTextureData.ShaderResourceView.GetAddressOf()			// Pointer to the resulting shader resource view.
		);

		// Error check shader resource view creation.
		ENGINE_ASSERT_HRESULT(createWICTextureFromFileResult);

		// Cache texture description struct.
		gpuTextureData.Texture2D->GetDesc(&gpuTextureData.Texture2DDescriptor);
	}
}

void Renderer::CreateSamplerState()
{
	// Initialize the sampler descriptor struct.
	D3D11_SAMPLER_DESC samplerDescriptor = { };
	samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;  // Resolve U texture coordinates outside of the 0-1 range by tiling.
	samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;  // Resolve V texture coordinates outside of the 0-1 range by tiling.
	samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;  // Resolve W texture coordinates outside of the 0-1 range by tiling.
	samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;	  // Do not compare the sampled data to the previously sampled data.
	samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;			  // How to filter when sampling the texture. Use Lerp for all three.
	samplerDescriptor.MaxLOD = D3D11_FLOAT32_MAX;										  // The minimum LOD level. 0 is the most detailed.

	// Attempt to create the sampler state.
	const HRESULT createSamplerStateResult = m_id3d11Device->CreateSamplerState(
		&samplerDescriptor,						// The sampler state descriptor struct.
		m_id3d11SamplerState.GetAddressOf()		// Pointer to the resulting sampler state interface.
	);

	// Error check sampler state creation.
	ENGINE_ASSERT_HRESULT(createSamplerStateResult);
}

void Renderer::CreateMVPConstantBuffers()
{
	// Initialize constant buffer descriptor struct.
	D3D11_BUFFER_DESC constantBufferDescriptor = { 0 };
	constantBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;	// How the buffer should be bound to the pipeline.
	constantBufferDescriptor.ByteWidth = sizeof(XMMATRIX);								// The size of the constant buffer.
	constantBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;					// How the buffer will be read from and written to. This one will require read and write access by the GPU.

	// Attempt to create the model matrix constant buffer.
	const HRESULT createModelMatrixBuffer = m_id3d11Device->CreateBuffer(
		&constantBufferDescriptor,				// Pointer to the buffer descriptor struct.
		nullptr,								// Pointer to optional initial data struct.
		m_modelMatrixBuffer.GetAddressOf()		// Pointer to resulting interface.
	);

	// Error check model matrix constant buffer creation.
	ENGINE_ASSERT_HRESULT(createModelMatrixBuffer);

	// Attempt to create the view matrix constant buffer.
	const HRESULT createViewMatrixBuffer = m_id3d11Device->CreateBuffer(
		&constantBufferDescriptor,				// Pointer to the buffer descriptor struct.
		nullptr,								// Pointer to optional initial data struct.
		m_viewMatrixBuffer.GetAddressOf()		// Pointer to resulting interface.
	);

	// Error check view matrix constant buffer creation.
	ENGINE_ASSERT_HRESULT(createViewMatrixBuffer);

	// Attempt to create the projection matrix constant buffer.
	const HRESULT createProjectionMatrixBuffer = m_id3d11Device->CreateBuffer(
		&constantBufferDescriptor,					// Pointer to the buffer descriptor struct.
		nullptr,									// Pointer to optional initial data struct.
		m_projectionMatrixBuffer.GetAddressOf()		// Pointer to resulting interface.
	);

	// Error check projection matrix constant buffer creation.
	ENGINE_ASSERT_HRESULT(createProjectionMatrixBuffer);
}

void Renderer::UpdateViewMatrix()
{
	m_viewMatrix = XMMatrixIdentity();
}

void Renderer::UpdateProjectionMatrix()
{
	static const Window& window = Window::GetInstanceRead();

	// Construct the window dimensions based on window's client dimensions.
// 	m_projectionMatrix = XMMatrixOrthographicOffCenterLH(
// 		0.0f,									// Left end of the view volume.
// 		(float)window.GetClientWidth(),			// Right end of the view volume.
// 		0.0f,									// Bottom end of the view volume.
// 		(float)window.GetClientHeight(),		// Top end of the view volume.
// 		0.1f,									// Distance of the near plane from the camera.
// 		100.0f									// Distance of the far plane from the camera.
// 	);

	m_projectionMatrix = XMMatrixPerspectiveFovLH(
		XM_PIDIV4,													// The vertical field of view.
		(float)window.GetClientWidth() / window.GetClientHeight(),	// The aspect ratio of the window.
		0.1f,														// Distance from the origin to the near clipping plane.
		100.0f														// Distance form the origin to the far clipping plane.
	);
}

void Renderer::UpdateViewProjectionMatrix()
{
	m_viewProjectionMatrix = XMMatrixMultiply(m_viewMatrix, m_projectionMatrix);
}

void Renderer::WriteUITextData(const CoreObject& coreObject)
{
	static CoreGPUDataManager& coreGPUDataManager = CoreGPUDataManager::GetInstanceWrite();
	GPUModelData& gpuModelData = coreGPUDataManager.GetGPUModelDataWrite(coreObject.GetGPUDataGUID());

	// Attempt to lock the vertex buffer and retrieve its data pointer.
	D3D11_MAPPED_SUBRESOURCE vertexBufferData = { nullptr };
	const HRESULT mapResult = m_id3d11DeviceContext->Map(
		(ID3D11Resource*)gpuModelData.VertexBuffer.Get(),	// Pointer to the device interface to lock and retrieve data of.
		0,													// The index slot of the resource we wish to map.
		D3D11_MAP::D3D11_MAP_WRITE_DISCARD,					// Discard the last entry of the resource and use what is written.
		0,													// Additional mapping create flags.
		&vertexBufferData
	);

	// Error check vertex buffer data retrieval and locking.
	ENGINE_ASSERT_HRESULT(mapResult);

	// Get the vertex buffer data pointer.
	VertexData* vertexData = (VertexData*)vertexBufferData.pData;

	// Copy the contents of the vertex data into the vertex buffer pointer.
	for (UINT i = 0; i < gpuModelData.Vertices.size(); ++i)
	{
		vertexData[i].m_position = gpuModelData.Vertices[i].m_position;
		vertexData[i].m_texture = gpuModelData.Vertices[i].m_texture;
	}

	// Unlock the vertex buffer and invalidate the mapped subresource pointer previously used.
	m_id3d11DeviceContext->Unmap(
		(ID3D11Resource*)gpuModelData.VertexBuffer.Get(),	// Pointer to resource to unlock and invalidate mapped subresource pointer of.
		0													// The slot of the GPU resource that is being unlocked.
	);
}

