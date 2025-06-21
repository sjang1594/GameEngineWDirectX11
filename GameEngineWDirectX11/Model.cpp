#include "pch.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "D3D11Utils.h"

namespace Luna {

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
         const std::string &basePath, const std::string &filename) {
    Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::vector<MeshData> &meshes) {
    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                       const std::string &basePath,
                       const std::string &filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &meshes) {

    m_meshConstsCPU.world = Matrix();
    D3D11Utils::CreateConstantBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::CreateConstantBuffer(device, m_materialConstsCPU, m_materialConstsGPU);

    for (const auto &meshData : meshes) {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->m_vertexBuffer);
        newMesh->m_vertexCount = UINT(meshData.vertices.size());    
        newMesh->m_indexCount = UINT(meshData.indices.size());
        newMesh->m_stride = UINT(sizeof(Vertex));
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

        // Metal & Roughness Map
        if (!meshData.roughnessTextureFilename.empty() ||
            !meshData.metallicTextureFilename.empty()) {
            std::cout << "Loading Metallic-Roughness texture:\n"
                      << "  Roughness: "
                      << (meshData.roughnessTextureFilename.empty() ? "" : meshData.roughnessTextureFilename) << "\n"
                      << "  Metallic : "
                      << (meshData.metallicTextureFilename.empty() ? "" : meshData.metallicTextureFilename) << "\n";
            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename, newMesh->m_metallicRoughnessTexture,
                newMesh->m_metallicRoughnessSRV);
        }

        if (!meshData.emissiveTextureFilename.empty()) {
            std::cout << "Loading emssive texture: " << meshData.emissiveTextureFilename
                      << std::endl;
            D3D11Utils::CreateTexture(device, context, meshData.emissiveTextureFilename,
                                      true, newMesh->m_emissiveTexture,
                                      newMesh->m_emissiveTextureSRV);
        }
        
        newMesh->m_vertexConstantBuffer = m_meshConstsGPU;
        newMesh->m_pixelConstantBuffer = m_materialConstsGPU;

        m_meshes.push_back(newMesh);
    }
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->VSSetConstantBuffers(0, 1, mesh->m_vertexConstantBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, mesh->m_pixelConstantBuffer.GetAddressOf());

        // Align
        vector<ID3D11ShaderResourceView *> resViews = {
            mesh->m_albedoTextureSRV.Get(),  mesh->m_normalTextureSRV.Get(),
            mesh->m_heightTextureSRV.Get(),  mesh->m_aoTextureSRV.Get(),
            mesh->m_metallicRoughnessSRV.Get(), mesh->m_emissiveTextureSRV.Get()};

        context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());

        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(),
                                    &mesh->m_stride, &mesh->m_offset);

        context->IASetIndexBuffer(mesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh->m_indexCount, 0, 0);
    }
}

void Model::RenderNormal(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->m_vertexBuffer.GetAddressOf(),
                                    &mesh->m_stride, &mesh->m_offset);
        context->Draw(mesh->m_vertexCount, 0);
    }
}

void Model::UpdateModelWorld(const Matrix &modelToWorld) { 
    m_modelWorld = modelToWorld;
    m_modelWorldIT = modelToWorld;
    m_modelWorldIT.Translation(Vector3(0.0f));
    m_modelWorldIT = m_modelWorldIT.Invert().Transpose();

    m_meshConstsCPU.world = m_modelWorld.Transpose();
    m_meshConstsCPU.worldIT = m_modelWorldIT.Transpose();
}
} // namespace Luna