#include "pch.h"
#include "App.h"
#include "GeometryGenerator.h"

namespace Luna {
App::App() : EngineBase() {}

bool App::Initialize() {
    if (!EngineBase::Initialize())
        return false;

    EngineBase::InitCubemaps(L"../Assets/Cubemaps/",
                             L"NightSky_0/NightSky_0EnvHDR.dds",
                             L"NightSky_0/NightSky_0DiffuseHDR.dds",
                             L"NightSky_0/NightSky_0SpecularHDR.dds",
                             L"NightSky_0/NightSky_0Brdf.dds");

    {
        MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);
        std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
        m_skybox = make_shared<Model>(m_d3dDevice, m_d3dContext, vector{skyboxMesh});
    }

    // Make this from Scene Loader
    // TODO: Load the Terrain Firstr cave
    {
        MeshData ground = GeometryGenerator::MakeSquare(1.0f, {1.0f, 1.0f});
        ground.albedoTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_Color.png";
        ground.normalTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_NormalDX.png";
        ground.aoTextureFilename = "../Assets/Textures/Ground/Bricks075A_4K-PNG/"
                                   "Bricks075A_4K-PNG_AmbientOcclusion.png";
        ground.heightTextureFilename = "../Assets/Textures/Ground/Bricks075A_4K-PNG/"
                                       "Bricks075A_4K-PNG_Displacement.png";
        ground.roughnessTextureFilename =
            "../Assets/Textures/Ground/Bricks075A_4K-PNG/Bricks075A_4K-PNG_Roughness.png";

        m_groundModel.Initialize(m_d3dDevice, m_d3dContext,
                                 std::vector<MeshData>{ground});
        m_groundModel.m_irradianceSRV = m_cubeMapping.m_diffuseSRV;
        m_groundModel.m_specularSRV = m_cubeMapping.m_specularSRV;
        m_groundModel.m_brdfSRV = m_cubeMapping.m_brdfSRV;
        m_groundModel.UpdateModelWorld(Matrix::CreateRotationX(DirectX::XM_PIDIV2));
        m_groundModel.m_basicPixelConstantData.reverseNormalMapY = false; // IF OPENGL, SET TRUE
        m_groundModel.m_basicPixelConstantData.isGLTF = false;
        m_groundModel.m_basicPixelConstantData.isParallax = true;
        m_groundModel.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
    }

    // TODO: Load Building
    {
        Vector3 center(0.0f, 0.0f, 0.0f);
        float radius = 0.4f;
        MeshData sphere = GeometryGenerator::MakeSphere(radius, 100, 100, {2.0f, 2.0f});
    }

    // Main Character
    {
        Vector3 center(0.0f, 0.5f, 0.0f);
        auto mainCharacterMesh = GeometryGenerator::ReadFromFile(
            "../Assets/Characters/medieval_vagrant_knights/", "scene.gltf");
        m_mainCharacter.m_basicPixelConstantData.reverseNormalMapY = true;
        m_mainCharacter.m_basicPixelConstantData.isGLTF = true;
        m_mainCharacter.m_basicPixelConstantData.isParallax = false;
        m_mainCharacter.Initialize(m_d3dDevice, m_d3dContext, mainCharacterMesh);
        m_mainCharacter.m_irradianceSRV = m_cubeMapping.m_diffuseSRV;
        m_mainCharacter.m_specularSRV = m_cubeMapping.m_specularSRV;
        m_mainCharacter.UpdateModelWorld(Matrix::CreateTranslation(center));
        m_mainCharacter.m_brdfSRV = m_cubeMapping.m_brdfSRV;
        m_mainCharacter.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
    }

    // Enemies (Monster)
    {
        //Vector3 center(-1.0f, 1.0f, 0.0f);
        //auto monsterMesh1 = GeometryGenerator::ReadFromFile(
        //    "../Assets/Characters/dark_souls/", "scene.gltf");

        //auto monsterMesh2 =
        //    GeometryGenerator::ReadFromFile("../Assets/Characters/valak/", "scene.gltf");
    }

    return true;
}

void App::Update(float dt) {
    if (!m_camera)
        return;

    if (m_keyPressed[87])
        m_camera->MoveForward(dt);
    if (m_keyPressed[83])
        m_camera->MoveForward(-dt);
    if (m_keyPressed[68])
        m_camera->MoveRight(dt);
    if (m_keyPressed[65])
        m_camera->MoveRight(-dt);

    Matrix view = m_camera->GetViewRow();
    Matrix proj = m_camera->GetProjRow();
    Vector3 eyeWorld = m_camera->GetEyePos();

    Matrix viewEnv = view;
    viewEnv.Translation(Vector3(0.0f));
    m_cubeMapping.UpdateVertexConstantBuffers(m_d3dDevice, m_d3dContext,
                                              viewEnv.Transpose(), proj.Transpose());
    m_groundModel.m_basicPixelConstantData.eyeWorld = eyeWorld;
    m_groundModel.m_basicVertexConstantData.view = view.Transpose();
    m_groundModel.m_basicVertexConstantData.projection = proj.Transpose();
    m_groundModel.m_basicPixelConstantData.heightScale = 0.02f;

    UpdateLight();

    m_mainCharacter.m_basicPixelConstantData.eyeWorld = eyeWorld;
    m_mainCharacter.m_basicVertexConstantData.view = view.Transpose();
    m_mainCharacter.m_basicVertexConstantData.projection = proj.Transpose();
}

void App::UpdateLight() {
    m_pointLight.position = m_lightPosition;
    m_pointLight.radiance = Vector3(1.0f);
    m_pointLight.fallOffEnd = 20.0f;

    // Ground
    m_groundModel.m_basicPixelConstantData.lights[1] = m_pointLight;
    m_groundModel.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);

    // Object
    m_mainCharacter.m_basicPixelConstantData.lights[1] = m_pointLight;
    m_mainCharacter.UpdateConstantBuffers(m_d3dDevice, m_d3dContext);
}

void App::Render() {
    // READY
    SetViewport();

    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_d3dContext->ClearRenderTargetView(m_floatRTV.Get(), clearColor);
    m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(),
                                        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    vector<ID3D11RenderTargetView *> renderTargetViews = {m_floatRTV.Get()};
    m_d3dContext->OMSetRenderTargets(UINT(renderTargetViews.size()),
                                     renderTargetViews.data(),
                                     m_d3dDepthStencilView.Get());

    m_d3dContext->OMSetDepthStencilState(m_d3dDepthStencilState.Get(), 0);

    m_d3dContext->RSSetState(m_solidRasterizerSate.Get());

    m_groundModel.Render(m_d3dContext);
    m_mainCharacter.Render(m_d3dContext);

    m_cubeMapping.Render(m_d3dContext);

    m_d3dContext->ResolveSubresource(m_resolvedBuffer.Get(), 0, m_floatBuffer.Get(), 0,
                                     DXGI_FORMAT_R16G16B16A16_FLOAT);

    m_postProcess.Render(m_d3dContext);
}

void App::UpdateGUI() {
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Env Map")) {
        int flag = 0;
        flag += ImGui::RadioButton("Env",
                                   &m_cubeMapping.m_pixelConstantData.textureToDraw, 0);
        ImGui::SameLine();
        flag += ImGui::RadioButton("Specular",
                                   &m_cubeMapping.m_pixelConstantData.textureToDraw, 1);
        ImGui::SameLine();
        flag += ImGui::RadioButton("Irradiance",
                                   &m_cubeMapping.m_pixelConstantData.textureToDraw, 1);
        flag += ImGui::SliderFloat(
            "Mip Level", &m_cubeMapping.m_pixelConstantData.mipLevel, 0.0f, 10.0f);
        if (flag) {
            m_cubeMapping.UpdatePixelConstantBuffers(m_d3dDevice, m_d3dContext);
        }
    }

    // TODO: Loop through all instances
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Normal Check")) {

        ImGui::Checkbox("Draw Normals", &m_mainCharacter.m_drawNormals);
    }
    ImGui::TreePop();
}
} // namespace Luna