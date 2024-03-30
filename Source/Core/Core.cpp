#include "PCH.h"
#include "Core.h"
#include "Logger/Logger.h"
#include "CoreObjectManager.h"

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

	default:
		break;
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
	ENGINE_ASSERT(SUCCEEDED(initCOMResult), "Failedto initialize COM library.");

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
}

void Engine::Render()
{
	static Renderer& renderer = Renderer::GetInstanceWrite();
	renderer.Render();
}

void Renderer::Initialize()
{
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateViewPort();

	Logger::GetInstanceWrite().Log(Logger::Message, "Successfully initialized renderer.");
}

void Renderer::Render()
{
	const CoreObjectManager& coreObjectManager = CoreObjectManager::GetInstanceRead();

	for (const CoreObject& coreObject : coreObjectManager.GetCoreObjectsRead())
	{
		Present(coreObject.gpuMeshData);
	}
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
	D3D_FEATURE_LEVEL resultingFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_1_0_CORE;
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
			&resultingFeatureLevel,					// The feature level the device was created with.
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

	// Set the render target view that will be sued to access and write to the back buffer.
	m_id3d11DeviceContext->OMSetRenderTargets(
		1,											// The number of render target views to set.
		m_id3d11RenderTargetView.GetAddressOf(),	// The array of pointers to the render target views to set.
		nullptr										// Optional depth stencil view.
	);
}

void Renderer::CreateViewPort()
{
	const Window& window = Window::GetInstanceRead();

	// Create the view port description struct.
	D3D11_VIEWPORT viewPortDescription = { 0 };
	viewPortDescription.Width = (FLOAT)window.GetClientWidth();
	viewPortDescription.Height = (FLOAT)window.GetClientHeight();
	viewPortDescription.TopLeftX = 0;
	viewPortDescription.TopLeftY = 0;
	viewPortDescription.MinDepth = 1.0f;
	viewPortDescription.MaxDepth = 1.0f;

	// Set the window viewport.
	m_id3d11DeviceContext->RSSetViewports(
		1,
		&viewPortDescription
	);
}

void Renderer::Present(const GPUMeshData& gpuModelData)
{
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

	// Clear the back buffer through the render target view.
	constexpr FLOAT clearColor[4] = { 0.09f, 0.09f, 0.09f, 1.0f };
	m_id3d11DeviceContext->ClearRenderTargetView(m_id3d11RenderTargetView.Get(), clearColor);

	// Draw the model.
	m_id3d11DeviceContext->Draw(
		(UINT)gpuModelData.Vertices.size(),		// The number of vertices to draw.
		0										// The index of the first vertex to draw.
	);

	// Present the contents of the back buffer to the screen.
	m_idxgiSwapChain->Present(0, 0);
}

void Renderer::CreateVertexBuffer(GPUMeshData& gpuModelData)
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
		&vertexBufferDecription,		// The buffer description struct.
		&initialBufferData,				// The initial buffer data.
		vertexBuffer.GetAddressOf()		// Pointer to the resulting buffer interface.
	);

	// Error check vertex buffer creation.
	ENGINE_ASSERT_HRESULT(createBufferResult);
	gpuModelData.VertexBuffer = vertexBuffer;
}

void Renderer::CreateIndexBuffer(GPUMeshData& gpuModelData)
{
}

void Renderer::CreateVertexShader(GPUMeshData& gpuModelData)
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
		"main",										// Shader entry point.
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

void Renderer::CreateInputLayout(GPUMeshData& gpuModelData)
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

void Renderer::CreatePixelShader(GPUMeshData& gpuModelData)
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
		"main",									// Shader entry point.
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

