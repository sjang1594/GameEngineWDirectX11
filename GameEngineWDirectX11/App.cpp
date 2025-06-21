#include "pch.h"
#include "App.h"
#include "GeometryGenerator.h"
#include "GraphicsCommon.h"

namespace Luna {
App::App() : EngineBase() {}

bool App::Initialize() {
    if (!EngineBase::Initialize())
        return false;

    InitCubemaps(L"../Assets/Cubemaps/", L"NightSky_0/NightSky_0EnvHDR.dds",
                 L"NightSky_0/NightSky_0DiffuseHDR.dds",
                 L"NightSky_0/NightSky_0SpecularHDR.dds",
                 L"NightSky_0/NightSky_0Brdf.dds");

    // Env
    {
        MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
        std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
        m_skybox = make_shared<Model>(m_d3dDevice, m_d3dContext, vector{skyboxMesh});
    }

    // Light
    {
        m_light.position = Vector3(0.0f, 2.0f, 0.3f);
        m_light.radiance = Vector3(5.0f);
        m_light.fallOffEnd = 20.0f;
        MeshData sphere = GeometryGenerator::MakeSphere(0.01f, 10, 10);
        m_lightSphere = make_shared<Model>(m_d3dDevice, m_d3dContext, vector{sphere});
        m_lightSphere->UpdateModelWorld(Matrix::CreateTranslation(m_light.position));
        modelLists.emplace_back(m_lightSphere);
    }

    // Make this from Scene Loader
    // TODO: Load the Terrain First cave
    {
        MeshData groundMesh = GeometryGenerator::MakeSquare(1.0f, {1.0f, 1.0f});
        groundMesh.albedoTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_Color.png";
        groundMesh.normalTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_NormalDX.png";
        groundMesh.aoTextureFilename = "../Assets/Textures/Ground/Bricks075A_4K-PNG/"
                                   "Bricks075A_4K-PNG_AmbientOcclusion.png";
        groundMesh.heightTextureFilename = "../Assets/Textures/Ground/Bricks075A_4K-PNG/"
                                       "Bricks075A_4K-PNG_Displacement.png";
        groundMesh.roughnessTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_Roughness.png";

        m_groundModel = make_shared<Model>(m_d3dDevice, m_d3dContext, vector{groundMesh});
        m_groundModel->UpdateModelWorld(Matrix::CreateRotationX(3.141592f * 0.5f) *
                                        Matrix::CreateTranslation(0.0f, 0.0f, 0.0f));
        modelLists.emplace_back(m_groundModel);
    }

    {
        Vector3 center(0.0f, 0.0f, 0.0f);
        float radius = 0.4f;
        MeshData sphere = GeometryGenerator::MakeSphere(radius, 100, 100, {2.0f, 2.0f});
    }

    // Main Character
    {
        const string path = "../Assets/Characters/medieval_vagrant_knights/";
        auto mainCharacterMesh = GeometryGenerator::ReadFromFile(path, "scene.gltf");

        Vector3 center(0.0f, 0.5f, 0.0f);
        m_mainCharacter =
            make_shared<Model>(m_d3dDevice, m_d3dContext, mainCharacterMesh);
        // GLTF
        m_mainCharacter->m_materialConstsCPU.reverseNormalY = true;
        m_mainCharacter->UpdateModelWorld(Matrix::CreateTranslation(center));
        // TODO: Set BoundingBox or the Capsule
        modelLists.emplace_back(m_mainCharacter);
    }

    // Enemies (Monster)
    {

    }

    // Cursor
    {

    }

    return true;
}

void App::Update(float dt) {
    if (!m_camera)
        return;

    m_camera->UpdateKeyboard(dt, m_keyPressed);
    
    const Matrix view = m_camera->GetView();
    const Matrix proj = m_camera->GetProj();
    const Vector3 eyeWorld = m_camera->GetEyePos();

    m_globalConstsCPU.lights[1] = m_light;
    UpdateGlobalConstants(eyeWorld, view, proj);
    m_lightSphere->UpdateModelWorld(Matrix::CreateTranslation(m_light.position));
    m_lightSphere->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
    m_groundModel->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
    m_mainCharacter->UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
}

void App::Render() {
    // READY
    SetViewport();

    m_d3dContext->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());

    m_d3dContext->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                                Graphics::sampleStates.data());
    
    vector<ID3D11ShaderResourceView *> commonSRVs = {m_envSRV.Get(), m_specularSRV.Get(),
                                                  m_diffuseSRV.Get(), m_brdfSRV.Get()};
    // Allgn to 10
    m_d3dContext->PSSetShaderResources(10, UINT(commonSRVs.size()), commonSRVs.data());

    // Clear & Reset
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    vector<ID3D11RenderTargetView *> rtvs = {m_floatRTV.Get()};
    for (size_t i = 0; i < rtvs.size(); i++) {
        m_d3dContext->ClearRenderTargetView(rtvs[i], clearColor);
    }

    m_d3dContext->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(),
                                     m_d3dDepthStencilView.Get());

    m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(),
                                        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    EngineBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                           : Graphics::defaultSolidPSO);

    EngineBase::SetGlobalConstants(m_globalConstsGPU);

    for (auto &i : modelLists) {
        i->Render(m_d3dContext);
    }

    EngineBase::SetPipelineState(Graphics::normalsPSO);
    for (auto &i : modelLists) {
        if (i->m_drawNormals)
            i->RenderNormal(m_d3dContext);
    }

    EngineBase::SetGlobalConstants(m_globalConstsGPU);

    EngineBase::SetPipelineState(m_drawAsWire ? Graphics::skyboxWirePSO
                                           : Graphics::skyboxSolidPSO);
    m_skybox->Render(m_d3dContext);
    
    
    m_d3dContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0,
                                     DXGI_FORMAT_R16G16B16A16_FLOAT);
    EngineBase::SetPipelineState(Graphics::postProcessingPSO);
    m_postProcess.Render(m_d3dContext);
}

void App::UpdateGUI() {
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {
        ImGui::Checkbox("Wireframe", &m_drawAsWire);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Env Map")) {
        ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f, 5.0f);
        ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
        ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f, 10.0f);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Point Light")) {
        ImGui::SliderFloat3("Position", &m_light.position.x, -5.0f, 5.0f);
        ImGui::TreePop();
    }
    // TODO draw all objects normals
    ImGui::Checkbox("Draw Normals", &m_mainCharacter->m_drawNormals);
}
} // namespace Luna