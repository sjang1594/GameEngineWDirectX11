#include "pch.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "D3D11Utils.h"

namespace Luna {
void Model::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                       const std::string &basePath,
                       const std::string &filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &meshes) {

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&samplerDesc, m_linearSamplerState.GetAddressOf());

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&samplerDesc, m_clmapSamplerState.GetAddressOf());

    for (const auto &meshData : meshes) {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->m_vertexBuffer);
        newMesh->m_vertexCount = UINT(meshData.vertices.size());    
        newMesh->m_indexCount = UINT(meshData.indices.size());
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->m_indexBuffer);
        
        // Color (Albedo) Map
        if (!meshData.albedoTextureFilename.empty()) {
            std::cout << "Loading albedo texture: " << meshData.albedoTextureFilename << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.albedoTextureFilename, true, newMesh->m_albedoTexture,
                                      newMesh->m_albedoTextureSRV);
        }

        // Normal Map
        if (!meshData.normalTextureFilename.empty()) {
            std::cout << "Loading normal texture: " << meshData.normalTextureFilename << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.normalTextureFilename, false,
                                      newMesh->m_normalTexture,
                                      newMesh->m_normalTextureSRV);
        }

        // Height Map
        if (!meshData.heightTextureFilename.empty()) {
            std::cout << "Loading height texture: " << meshData.heightTextureFilename << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.heightTextureFilename, false,
                                      newMesh->m_heightTexture,
                                      newMesh->m_heightTextureSRV);
        }

        // Ambient Occlusion Map
        if (!meshData.aoTextureFilename.empty()) {
            std::cout << "Loading AO texture: " << meshData.aoTextureFilename << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.aoTextureFilename, false, 
                                      newMesh->m_aoTexture, newMesh->m_aoTextureSRV);
        }

        // Roughness Map
        if (!meshData.roughnessTextureFilename.empty()) {
            std::cout << "Loading roughness texture: " << meshData.roughnessTextureFilename
                      << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.roughnessTextureFilename, false,
                                      newMesh->m_roughnessTexture,
                                      newMesh->m_roughnessTextureSRV);
        }

        // Metalic Map
        if (!meshData.metalicTextureFilename.empty()) {
            std::cout << "Loading metalic texture: " << meshData.metalicTextureFilename
                      << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.metalicTextureFilename,
                                      false, newMesh->m_metalicTexture,
                                      newMesh->m_metalicTextureSRV);
        }

        if (!meshData.emissiveTextureFilename.empty()) {
            std::cout << "Loading emssive texture: " << meshData.emissiveTextureFilename
                      << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.emissiveTextureFilename,
                                      true, newMesh->m_emissiveTexture,
                                      newMesh->m_emissiveTextureSRV);
        }

        newMesh->m_vertexConstantBuffer = m_vertexConstantBuffer;
        newMesh->m_pixelConstantBuffer = m_pixelConstantBuffer;

        m_meshes.push_back(newMesh);
    }

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 4x3
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},  // 4x3 + 4*3
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"Base.vert.hlsl", basicInputElements,
                                                 m_basicVertexShader, m_basicInputLayout);
    D3D11Utils::CreatePixelShader(device, L"Base.frag.hlsl", m_basicPixelShader);

    // Debug Purpose
    // Certainly possible to create inputlayout for normal, but it's better to use same as above
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"Normal.vert.hlsl",
                                                 basicInputElements, m_normalVertexShader,
                                                 m_basicInputLayout);
    D3D11Utils::CreateGeometryShader(device, L"Normal.geom.hlsl", m_normalGeometryShader);
    D3D11Utils::CreatePixelShader(device, L"Normal.frag.hlsl", m_normalPixelShader);
    D3D11Utils::CreateConstantBuffer(device, m_normalVertexConstantData,
                                     m_normalConstantBuffer);
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {

    D3D11Utils::UpdateBuffer(device, context, m_basicVertexConstantData, m_vertexConstantBuffer);
    D3D11Utils::UpdateBuffer(device, context, m_basicPixelConstantData, m_pixelConstantBuffer);
    if (m_drawNormals && m_drawNormalsDirtyFlags) {
        D3D11Utils::UpdateBuffer(device, context, m_normalVertexConstantData,
                                 m_normalConstantBuffer);
        m_drawNormalsDirtyFlags = false;
    }
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    for (const auto &mesh : m_meshes) {
        context->VSSetShader(m_basicVertexShader.Get(), 0, 0);
        context->VSSetConstantBuffers(0, 1, mesh->m_vertexConstantBuffer.GetAddressOf());
        
        vector<ID3D11SamplerState *> samplerStates = {m_linearSamplerState.Get(),
                                                      m_clmapSamplerState.Get()};

        context->PSSetSamplers(0, UINT(samplerStates.size()), samplerStates.data());
        context->PSSetShader(m_basicPixelShader.Get(), 0, 0);

        // Align
        vector<ID3D11ShaderResourceView *> resViews = {m_specularSRV.Get(),
                                                       m_irradianceSRV.Get(),
                                                       m_brdfSRV.Get(),
                                                       mesh->m_albedoTextureSRV.Get(),
                                                       mesh->m_normalTextureSRV.Get(),
                                                       mesh->m_heightTextureSRV.Get(),
                                                       mesh->m_aoTextureSRV.Get(),
                                                       mesh->m_metalicTextureSRV.Get(),
                                                       mesh->m_roughnessTextureSRV.Get(),
                                                       mesh->m_emissiveTextureSRV.Get()};

        context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());

        context->PSSetConstantBuffers(0, 1, mesh->m_pixelConstantBuffer.GetAddressOf());

        context->IASetInputLayout(m_basicInputLayout.Get());
        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(), &stride, &offset);

        context->IASetIndexBuffer(mesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->m_indexCount, 0, 0);

        if (m_drawNormals) {
            context->VSSetShader(m_normalVertexShader.Get(), 0, 0);
            ID3D11Buffer *pptr[2] = {m_vertexConstantBuffer.Get(),
                                     m_normalConstantBuffer.Get()};
            context->GSSetConstantBuffers(0, 2, pptr);
            context->GSSetShader(m_normalGeometryShader.Get(), 0, 0);
            context->PSSetShader(m_normalPixelShader.Get(), 0, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            context->Draw(mesh->m_vertexCount, 0);
            context->GSSetShader(nullptr, 0, 0);
        }
    }
}

void Model::RenderNormal(ComPtr<ID3D11DeviceContext> &context) {

}

void Model::UpdateModelWorld(const Matrix &modelToWorld) { 
    m_modelWorld = modelToWorld;
    m_modelWorldIT = modelToWorld;
    m_modelWorldIT.Translation(Vector3(0.0f));
    m_modelWorldIT = m_modelWorldIT.Invert().Transpose();

    m_basicVertexConstantData.model = m_modelWorld.Transpose();
    m_basicVertexConstantData.invTranspose = m_modelWorldIT.Transpose();
}
} // namespace Luna