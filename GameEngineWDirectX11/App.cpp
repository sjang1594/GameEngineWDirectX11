#include "pch.h"
#include "App.h"
#include "GeometryGenerator.h"

namespace Luna {
App::App() : EngineBase() {}

bool App::Initialize() { 
    if (!EngineBase::Initialize())
        return false;

    m_cubeMapping.Initialize(m_d3dDevice, 
                             L"../Assets/Cubemaps/skybox/cubemap_bgra.dds",
                             L"../Assets/Cubemaps/skybox/cubemap_diffuse.dds",
                             L"../Assets/Cubemaps/skybox/cubemap_specular.dds");

    MeshData ground = GeometryGenerator::MakeSquare(3.0f);
    ground.textureFilename = "../Assets/Texture/grass-texture-background.jpg";
    m_groundModel.Initialize(m_d3dDevice, std::vector<MeshData>{ground});
    m_groundModel.m_diffuseResView = m_cubeMapping.m_diffuseResView;
    m_groundModel.m_specularResView = m_cubeMapping.m_specularResView;

    Matrix modelMat = Matrix::CreateRotationX(DirectX::XM_PIDIV2);
    Matrix inverseTranspose = modelMat;
    inverseTranspose.Translation(Vector3(0.0f));
    inverseTranspose = inverseTranspose.Invert().Transpose();

    m_groundModel.m_basicVertexConstantData.model = modelMat.Transpose();
    m_groundModel.m_basicVertexConstantData.invTranspose = inverseTranspose.Transpose();
    m_groundModel.m_basicPixelConstantData.useTexture = true;
    m_groundModel.m_basicPixelConstantData.material.diffuse = Vector3(1.0f);
    m_groundModel.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
    return true; 
}

void App::Update(float dt) {
    if (!m_camera)
        return;

    if (m_keyPressed[87])
        m_camera->MoveForward(dt);
    if (m_keyPressed[83])
        m_camera->MoveForward(dt);
    if (m_keyPressed[68])
        m_camera->MoveRight(dt);
    if (m_keyPressed[65])
        m_camera->MoveRight(-dt);

    Matrix view = m_camera->GetViewRow();
    Matrix proj = m_camera->GetProjRow();
    Vector3 eyeWorld = m_camera->GetEyePos();

    m_cubeMapping.UpdateConstantBuffers(m_d3dDevice, m_d3dContext, view.Transpose(),
                                         proj.Transpose());
    m_groundModel.m_basicPixelConstantData.useTexture = true;
    m_groundModel.m_basicPixelConstantData.eyeWorld = eyeWorld;
    m_groundModel.m_basicVertexConstantData.view = view.Transpose();
    m_groundModel.m_basicVertexConstantData.projection = proj.Transpose();
    m_groundModel.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
}

void App::Render() {
    // READY
    SetViewport();
   
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), clearColor);
    m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(),
                                        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(),
                                     m_d3dDepthStencilView.Get());
    m_d3dContext->RSSetState(m_d3dRasterizerState.Get());
    m_groundModel.Render(m_d3dContext);
    m_cubeMapping.Render(m_d3dContext);

}

void App::UpdateGUI() {}
} // namespace Luna