#include "pch.h"
#include "App.h"
#include "GraphicsCommon.h"

namespace Luna {
App::App() : EngineBase() {}

bool App::Initialize() { 
    if (!EngineBase::Initialize())
        return false;
    return true; 
}

void App::Update(float dt) {
}

void App::Render() { 
    EngineBase::SetMainViewport();
    m_d3dContext->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());
    m_d3dContext->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    vector<ID3D11RenderTargetView *> rtvs = {m_floatRTV.Get()};

    EngineBase::SetGlobalConstants(m_globalConstsGPU);

    
    m_d3dContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

    m_d3dContext->OMSetRenderTargets(1, m_d3dBackBufferRTV.GetAddressOf(), NULL);
}

void App::UpdateGUI() {

}
} // namespace Luna