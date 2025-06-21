#include "pch.h"
#include "EngineBase.h"
#include "Camera.h"
#include "D3D11Utils.h"
#include "GraphicsCommon.h"

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
    : m_fullscreen(false), m_enableDepthTest(true), m_enableDepthWrite(false),
      m_screenWidth(1280), m_screenHeight(960), m_mainWindow(0),
      m_screenViewport(D3D11_VIEWPORT()) {

    g_engeinBase = this;
    m_camera = std::make_shared<Camera>();
    m_camera->SetAspectRatio(GetAspectRatio());
}

EngineBase::~EngineBase() {
    g_engeinBase = nullptr;

    /* Clean Up */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(m_mainWindow);
}

float EngineBase::GetAspectRatio() const {
    return float(m_screenWidth - m_guiWidth) / m_screenHeight;
}

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
            ImGui::Begin("DEBUG");
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

#ifdef _DEBUG
            UpdateGUI();
#endif 
            m_guiWidth = 0;
            ImGui::End();
            ImGui::Render(); 

            Update(ImGui::GetIO().DeltaTime);

            Render();

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            m_d3dSwapChain->Present(1, 0);
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

    SetForegroundWindow(m_mainWindow);
    
    return true;
}

void EngineBase::OnMouseMove(int mouseX, int mouseY) {
    m_cursor_ndc_x = mouseX * 2.0f / m_screenWidth - 1.0f;
    m_cursor_ndc_y = -mouseY * 2.0f / m_screenHeight + 1.0f;

    m_cursor_ndc_x = std::clamp(m_cursor_ndc_x, -1.0f, 1.0f);
    m_cursor_ndc_y = std::clamp(m_cursor_ndc_y, -1.0f, 1.0f);
    m_camera->UpdateMouse(m_cursor_ndc_x, m_cursor_ndc_y);
}

bool EngineBase::UpdateMouseCtrl(const BoundingSphere &boundingSphere, Quaternion &qr,
                                 Vector3 &dragTranslation, Vector3 &pickPoint) {
    const Matrix view = m_camera->GetView();
    const Matrix proj = m_camera->GetProj();

    static float prevRatio = 0.0f;
    static Vector3 prevPos(0.0f);
    static Vector3 prevVector(0.0f);
    return false;
}

LRESULT EngineBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (m_d3dSwapChain) {
            m_screenWidth = int(LOWORD(lParam));
            m_screenHeight = int(HIWORD(lParam));
            m_guiWidth = 0;

            m_backBufferRTV.Reset();
            m_d3dSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                          DXGI_FORMAT_UNKNOWN, 0);

            CreateBuffers();
            SetViewport();
            m_camera->SetAspectRatio(this->GetAspectRatio());
        }
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_MOUSEMOVE:
        OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_LBUTTONUP:
        break;
    case WM_RBUTTONUP:
        break;
    case WM_KEYUP:
        m_keyPressed[wParam] = false;
        break;
    case WM_KEYDOWN:
        m_keyPressed[wParam] = true;
        if (wParam == 27) {
            DestroyWindow(hwnd);
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void EngineBase::InitCubemaps(const wstring &basePath, const wstring &envFilename,
                              const wstring &specularFilename,
                              const wstring &irradianceFilename,
                              const wstring &brdfFilename) {

    D3D11Utils::CreateDDSTexture(m_d3dDevice, (basePath + envFilename).c_str(), true,
                                 m_envSRV);
    D3D11Utils::CreateDDSTexture(m_d3dDevice, (basePath + specularFilename).c_str(), true,
                                 m_specularSRV);
    D3D11Utils::CreateDDSTexture(m_d3dDevice, (basePath + irradianceFilename).c_str(),
                                 true, m_diffuseSRV);
    D3D11Utils::CreateDDSTexture(m_d3dDevice, (basePath + brdfFilename).c_str(), false,
                                 m_brdfSRV);
}

void EngineBase::UpdateGlobalConstants(const Vector3 eyeWorld, const Matrix &view,
                                       const Matrix &proj) {
    m_globalConstsCPU.eyeWorld = eyeWorld;
    m_globalConstsCPU.view = view.Transpose();
    m_globalConstsCPU.proj = proj.Transpose();
    m_globalConstsCPU.viewProj = (view * proj).Transpose();

    D3D11Utils::UpdateBuffer(m_d3dDevice, m_d3dContext, m_globalConstsCPU,
                             m_globalConstsGPU);
}

// For reflective object: such as mirror
void EngineBase::UpdateGlobalConstants(const Vector3 eyeWorld, const Matrix &view,
                                       const Matrix &proj, const Matrix &refl) {
    m_globalConstsCPU.eyeWorld = eyeWorld;
    m_globalConstsCPU.view = view.Transpose();
    m_globalConstsCPU.proj = proj.Transpose();
    m_globalConstsCPU.viewProj = (view * proj).Transpose();
    
    // TODO:
}

void EngineBase::SetGlobalConstants(ComPtr<ID3D11Buffer> &globalConstantsGPU) {
    m_d3dContext->VSSetConstantBuffers(1, 1, globalConstantsGPU.GetAddressOf());
    m_d3dContext->PSSetConstantBuffers(1, 1, globalConstantsGPU.GetAddressOf());
    m_d3dContext->GSSetConstantBuffers(1, 1, globalConstantsGPU.GetAddressOf());
}

void EngineBase::CreateDepthBuffers() {
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = m_screenWidth;
    desc.Height = m_screenHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    if (m_numQualityLevels > 0) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = m_numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    ThrowIfFailed(
        m_d3dDevice->CreateTexture2D(&desc, 0, depthStencilBuffer.GetAddressOf()));
    ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(), 
        NULL, m_d3dDepthStencilView.GetAddressOf()));
}

void EngineBase::SetPipelineState(const GraphicsPSO &pso) {
    m_d3dContext->VSSetShader(pso.m_vertexShader.Get(), 0, 0);
    m_d3dContext->PSSetShader(pso.m_pixelShader.Get(), 0, 0);
    m_d3dContext->HSSetShader(pso.m_hullShader.Get(), 0, 0);
    m_d3dContext->DSSetShader(pso.m_domainShader.Get(), 0, 0);
    m_d3dContext->GSSetShader(pso.m_geometryShader.Get(), 0, 0);
    m_d3dContext->IASetInputLayout(pso.m_inputLayout.Get());
    m_d3dContext->RSSetState(pso.m_rasterizerState.Get());
    m_d3dContext->OMSetBlendState(pso.m_blendState.Get(), pso.m_blendFactor, 0xffffffff);
    m_d3dContext->OMSetDepthStencilState(pso.m_depthStencilState.Get(), pso.m_stencilRef);
    m_d3dContext->IASetPrimitiveTopology(pso.m_primitiveTopology);
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
    wcex.lpszClassName = L"LunaEngine - DirectX11";             
    wcex.hIconSm = LoadIcon(nullptr, MAKEINTRESOURCE(IDC_ICON));   
    
    if (!RegisterClassEx(&wcex)) {
        std::cout << "Failed to register window class: LunaEngine - DirectX11" << std::endl;
        return false;
    }

    RECT wr = {0, 0, m_screenWidth, m_screenHeight};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    m_mainWindow = CreateWindow(wcex.lpszClassName, 
                                L"LunaEngine Example", 
                                WS_OVERLAPPEDWINDOW,
                                100,                // X position
                                100,                // Y position
                                wr.right - wr.left, // Window width
                                wr.bottom - wr.top, // Window height
                                nullptr, nullptr, 
                                wcex.hInstance, nullptr);

    if (!m_mainWindow) {
        std::cout << "CreateWindow() failed: LunaEngine Example" << std::endl;
        return false;
    }

    ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
    UpdateWindow(m_mainWindow);

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

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to Create DXGI Factory");
        return false;
    }

    ComPtr<IDXGIAdapter1> adapter;
    bool deviceCreated = false;
    for (UINT i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        std::wcout << L"Adapter: " << desc.Description << std::endl;

        if (desc.VendorId == 0x8086) {
            std::wcout << L"Skipping Intel adapter: " << desc.Description << std::endl;
            continue;
        }

        hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags,
                               featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                               &m_d3dDevice, &featureLevel, &m_d3dContext);

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

    // BackBuffer -> Monitor 
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = m_screenWidth;
    sd.BufferDesc.Height = m_screenHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = m_mainWindow;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    hr = m_dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, &m_d3dSwapChain);
    if (FAILED(hr)) {
        PrintErrorMessage(hr, "Failed to create Swap Chain");
        return false;
    }

    Graphics::InitCommonStates(m_d3dDevice);
    CreateBuffers();
    SetViewport();
    
    D3D11Utils::CreateConstantBuffer(m_d3dDevice, m_globalConstsCPU, m_globalConstsGPU);
    return true;
}

bool EngineBase::InitGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(float(m_screenWidth), float(m_screenHeight));
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplDX11_Init(m_d3dDevice.Get(), m_d3dContext.Get())) {
        return false;
    }

    if (!ImGui_ImplWin32_Init(m_mainWindow)) {
        return false;
    }

    return true;
}

void EngineBase::SetViewport() {
    ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
    m_screenViewport.TopLeftX = 0;
    m_screenViewport.TopLeftY = 0;
    m_screenViewport.Width = float(m_screenWidth);
    m_screenViewport.Height = float(m_screenHeight);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;
    m_d3dContext->RSSetViewports(1, &m_screenViewport);
}

void EngineBase::CreateBuffers() {
    ComPtr<ID3D11Texture2D> backBuffer;
    
    ThrowIfFailed(m_d3dSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
   
    ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), NULL,
                                                      m_backBufferRTV.GetAddressOf()));
    
    ThrowIfFailed(m_d3dDevice->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels
    ));

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.MipLevels = desc.ArraySize = 1;
    // desc.BindFlags = D3D11_BIND_RENDER_TARGET
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    if (m_numQualityLevels) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = m_numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }

    // Float Buffer -> Resolved Buffer
    ThrowIfFailed(m_d3dDevice->CreateTexture2D(&desc, NULL, m_floatBuffer.GetAddressOf()));

    ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(m_floatBuffer.Get(), NULL,
                                                        m_floatSRV.GetAddressOf()));

    ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_floatBuffer.Get(), NULL, m_floatRTV.GetAddressOf()));

    // FLOAT MSAA Resolve Buffer for SRV/RTV
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    ThrowIfFailed(m_d3dDevice->CreateTexture2D(&desc, NULL, m_resolvedBuffer.GetAddressOf()));
    ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(m_resolvedBuffer.Get(), NULL,
                                                        m_resolvedSRV.GetAddressOf()));
    ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_resolvedBuffer.Get(), NULL,
                                                      m_resolvedRTV.GetAddressOf()));

    CreateDepthBuffers();

    m_postProcess.Initialize(m_d3dDevice, m_d3dContext, {m_resolvedSRV},
                             {m_backBufferRTV},
                             m_screenWidth, m_screenHeight);
}
} // namespace Luna