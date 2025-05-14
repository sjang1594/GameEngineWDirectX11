#include "pch.h"
#include <comdef.h>
#include "EngineBase.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);
namespace Luna {
EngineBase *g_engeinBase = nullptr;

void PrintErrorMessage(HRESULT hr, const std::string &msg) {
    _com_error err(hr);
    std::cout << msg << " Error: " << err.ErrorMessage() << std::endl;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return g_engeinBase->MsgProc(hWnd, msg, wParam, lParam);
}

EngineBase::EngineBase()
    : _fullscreen(false), _enableDepthTest(true), _enableDepthWrite(false), _wireframe(false),
      _screenWidth(1280), _screenHeight(960), _mainWindow(0), _screenViewport(D3D11_VIEWPORT()) {

    g_engeinBase = this;
}

EngineBase::~EngineBase() {
    g_engeinBase = nullptr;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(_mainWindow);
}

float EngineBase::GetAspectRatio() const { return float(_screenWidth) / _screenHeight; }

int EngineBase::Run() {
    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            ImGui_ImplDX11_NewFrame(); 
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("Scene Control");

            // ImGui가 측정해주는 Framerate 출력
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

            UpdateGUI();

            ImGui::End();
            ImGui::Render(); 

            Update(ImGui::GetIO().DeltaTime);

            Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            _d3dSwapChain->Present(1, 0);
        }
    }

    return 0;
}

bool EngineBase::Initialize() {

    if (!InitMainWindow())
        return false;

    if (!InitD3D())
        return false;

    if (!InitGUI())
        return false;

    return true;
}

LRESULT EngineBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_MOUSEMOVE:
        break;
    case WM_LBUTTONUP:
        break;
    case WM_RBUTTONUP:
        break;
    case WM_KEYDOWN:
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

bool EngineBase::InitMainWindow() {
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(nullptr);   
    wcex.hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(IDC_ICON)); 
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);  
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;                     
    wcex.lpszClassName = L"LunaEngine - DriectX11";             
    wcex.hIconSm = LoadIcon(nullptr, MAKEINTRESOURCE(IDC_ICON));   
    
    if (!RegisterClassEx(&wcex)) {
        std::cout << "Failed to register window class: LunaEngine - DirectX11" << std::endl;
        return false;
    }

    RECT wr = {0, 0, _screenWidth, _screenHeight};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    _mainWindow = CreateWindow(wcex.lpszClassName, L"LunaEngine Example", WS_OVERLAPPEDWINDOW,
                                100,                // X position
                                100,                // Y position
                                wr.right - wr.left, // Window width
                                wr.bottom - wr.top, // Window height
                                nullptr, nullptr, wcex.hInstance, nullptr);

    if (!_mainWindow) {
        std::cout << "CreateWindow() failed: LunaEngine Example" << std::endl;
        return false;
    }

    ShowWindow(_mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(_mainWindow);

    return true;
}

bool EngineBase::InitD3D() { 
    const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
    UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL featureLevels[2] = {
        D3D_FEATURE_LEVEL_11_0, 
        D3D_FEATURE_LEVEL_9_3};

    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to Create DXGI Factory");
        return false;
    }

    ComPtr<IDXGIAdapter1> adapter;
    bool deviceCreated = false;
    for (UINT i = 0; _dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        std::wcout << L"Adapter: " << desc.Description << std::endl;

        if (desc.VendorId == 0x8086) {
            std::wcout << L"Skipping Intel adapter: " << desc.Description << std::endl;
            continue;
        }

        hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags,
                               featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                               &_d3dDevice, &featureLevel, &_d3dContext);

        if (SUCCEEDED(hr)) {
            std::cout << "Direct3D 11 device created successfully with adapter" << std::endl;
            deviceCreated = true;
            break;
        }
    }

    if (!deviceCreated) {
        PrintErrorMessage(hr, "Failed to create D3D11 device");
        return false;
    }

    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create D3D11 device with selected adapter");
        return false;
    }

    std::cout << "Direct3D 11 device created successfully with feature level: " << featureLevel
              << std::endl;

    UINT maxSampleCount = 8;
    UINT numQualityLevels = 0;
    for (UINT sampleCount = maxSampleCount; sampleCount > 0; sampleCount /= 2) {
        hr = _d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount,
                                                       &numQualityLevels);
        if (SUCCEEDED(hr) && numQualityLevels > 0) {
            std::cout << "MSAA supported with " << sampleCount << " samples and "
                      << numQualityLevels << " quality levels." << std::endl;
            break;
        }
    }

    if (numQualityLevels == 0) {
        std::cout << "MSAA not supported. Disabling MSAA." << std::endl;
    }

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = _screenWidth;
    sd.BufferDesc.Height = _screenHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = _mainWindow;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = _fullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
    sd.SampleDesc.Count = (numQualityLevels > 1) ? 4 : 1;
    sd.SampleDesc.Quality = (numQualityLevels > 1) ? (numQualityLevels - 1) : 0;

    hr = _dxgiFactory->CreateSwapChain(_d3dDevice.Get(), &sd, &_d3dSwapChain);
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Swap Chain");
        return false;
    }

    // Create Rasterizer State
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;

    hr = _d3dDevice->CreateRasterizerState(&rastDesc, &_d3dRasterizerState);
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Rasterizer State");
        return false;
    }

    // Render target view
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = _d3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to get the backBuffer");
        return false;
    }

    hr = _d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &_d3dRenderTargetView);
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Render Target View");
        return false;
    }

    // Depth stencil buffer and view
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
    depthStencilBufferDesc.Width = _screenWidth;
    depthStencilBufferDesc.Height = _screenHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.SampleDesc.Count = sd.SampleDesc.Count;
    depthStencilBufferDesc.SampleDesc.Quality = sd.SampleDesc.Quality;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = _d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, _d3dDepthStencilBuffer.GetAddressOf());
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Depth Stencil Buffer");
        return false;
    }

    hr = _d3dDevice->CreateDepthStencilView(_d3dDepthStencilBuffer.Get(), nullptr,
                                            &_d3dDepthStencilView);
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Depth Stencil View");
        return false;
    }

    // Create depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilDesc.DepthEnable = true; // false
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = _d3dDevice->CreateDepthStencilState(&depthStencilDesc,
                                             _d3dDepthStencilState.GetAddressOf());
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Depth Stencil State");
        return false;
    }

    ZeroMemory(&_screenViewport, sizeof(D3D11_VIEWPORT));
    _screenViewport.TopLeftX = 0;
    _screenViewport.TopLeftY = 0;
    _screenViewport.Width = float(_screenWidth);
    _screenViewport.Height = float(_screenHeight);
    // m_screenViewport.Width = static_cast<float>(m_screenHeight);
    _screenViewport.MinDepth = 0.0f;
    _screenViewport.MaxDepth = 1.0f; // Note: important for depth buffering

    _d3dContext->RSSetViewports(1, &_screenViewport);
    return true;
}

bool EngineBase::InitGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(_screenWidth), float(_screenHeight));
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplDX11_Init(_d3dDevice.Get(), _d3dContext.Get())) {
        return false;
    }

    if (!ImGui_ImplWin32_Init(_mainWindow)) {
        return false;
    }

    return true;
}

void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
    if (FAILED(hr)) {
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }

        if (errorBlob) {
            std::cout << "Shader compile error\n" << (char *)errorBlob->GetBufferPointer() << std::endl;
        }
    }
}

void EngineBase::CreateVertexShaderAndInputLayout(
    const wstring &filename, const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
    ComPtr<ID3D11VertexShader> &vertexShader, ComPtr<ID3D11InputLayout> &inputLayout) {
    
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr =
        D3DCompileFromFile(filename.c_str(), 0, 0, "main", "vs_5_0", 0, 0, &shaderBlob, &errorBlob);
    CheckResult(hr, errorBlob.Get());

    _d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
                                   NULL,
                                 &vertexShader);

    _d3dDevice->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
                                shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
                                &inputLayout);
}

void EngineBase::CreatePixelShader(const wstring &filename,
                                   ComPtr<ID3D11PixelShader> &pixelShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr =
        D3DCompileFromFile(filename.c_str(), 0, 0, "main", "ps_5_0", 0, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    _d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                                &pixelShader);
}

void EngineBase::CreateIndexBuffer(const vector<uint16_t> &indices,
                                   ComPtr<ID3D11Buffer> &_indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(uint16_t);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    _d3dDevice->CreateBuffer(&bufferDesc, &indexBufferData, _indexBuffer.GetAddressOf());
}

} // namespace Luna