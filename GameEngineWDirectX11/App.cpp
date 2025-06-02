#include "pch.h"
#include "App.h"
#include "GraphicsCommon.h"
#include "GeometryGenerator.h"
#include "Model.h"

namespace Luna {
App::App() : EngineBase() {}

bool App::Initialize() { 
    if (!EngineBase::Initialize())
        return false;

    EngineBase::InitCubeMaps(
        L"../Assets/Cubemaps/MorningSky_0/", 
        L"MorningSky_0EnvHDR.dds", L"MorningSky_0SpecularHDR.dds", 
        L"MorningSky_0DiffuseHDR.dds", L"MorningSky_0Brdf.dds");
    
    // Environment Setup
    SetupSkyBox(40.0f);

    InitializeObjects();

    return true; 
}

void App::Update(float dt) { 
    Matrix view = m_globalCamera->GetViewRow();
    Matrix proj = m_globalCamera->GetProjRow();
    Vector3 eyePosition = m_globalCamera->GetEyePos();

    EngineBase::UpdateGlobalConstants(eyePosition, view, proj);
}

void App::Render() { 
    EngineBase::SetMainViewport();
    m_d3dContext->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());
    m_d3dContext->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());

    vector<ID3D11ShaderResourceView *> commonSRVs = {m_environmentSRV.Get(), m_specularSRV.Get(),
                                                     m_irriadianceSRV.Get(), m_brdfSRV.Get()};
    m_d3dContext->PSSetShaderResources(10, UINT(commonSRVs.size()), commonSRVs.data());
    
    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    vector<ID3D11RenderTargetView *> rtvs = {m_floatRTV.Get()};   
    EngineBase::SetGlobalConstants(m_globalConstsGPU);
    m_skybox->Render(m_d3dContext);

    m_d3dContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);
    // Skybox
    m_skybox->Render(m_d3dContext);

    for (size_t i = 0; i < rtvs.size(); i++) {
        m_d3dContext->ClearRenderTargetView(rtvs[i], clearColor);
    }
    EngineBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                              : Graphics::defaultSolidPSO);
    EngineBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO : Graphics::skyboxSolidPSO);

    m_skybox->Render(m_d3dContext);
    m_d3dContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

   m_d3dContext->OMSetRenderTargets(1, m_floatRTV.GetAddressOf(), NULL);
}

void App::SetupSkyBox(const float& scale) 
{
    MeshData skyboxMesh = GeometryGenerator::MakeSphere(scale, 10, 10);
    std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
    m_skybox = std::make_shared<Model>(m_d3dDevice, m_d3dContext, vector{skyboxMesh});
}

void App::InitializeObjects() { 
    Vector3 center(0.0f, 0.5f, 1.0f);
    
}

void App::UpdateGUI() { 
    ImGui::Begin("InSpector");
    ImGui::End();
}
} // namespace Luna