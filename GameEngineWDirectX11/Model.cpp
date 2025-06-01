#include "pch.h"
#include "GeometryGenerator.h"
#include "Model.h"
#include "D3D11Utils.h"

namespace Luna {
Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::string &basePath, const std::string &filename) {
    Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::vector<class MeshData> &meshes) {
    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                       const std::string &basePath, const std::string &filename) {
    std::vector<MeshData> meshes = GeometryGenerator::ReadFromFile(basePath, filename);
    Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, 
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &meshes) {
    m_meshConstsCPU.world = Matrix();
    
    D3D11Utils::CreateConstantBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::CreateConstantBuffer(device, m_materialConstsCPU, m_materialConstsGPU);

    for (const auto &meshData : meshes) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, mesh->vertexBuffer);
        mesh->vertexCount = UINT(meshData.vertices.size());
        mesh->indexCount = UINT(meshData.indices.size());
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, mesh->indexBuffer);

        if (!meshData.albedoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context, meshData.albedoTextureFilename, true,
                                      mesh->albedoTexture, mesh->albedoSRV);
            m_materialConstsCPU.useAlbedoMap = true;
        }

        if (!meshData.emissiveTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context, meshData.emissiveTextureFilename, true,
                                      mesh->emissiveTexture, mesh->emissiveSRV);
            m_materialConstsCPU.useEmissiveMap = true;
        }

        if (!meshData.normalTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context, meshData.normalTextureFilename, false,
                                      mesh->normalTexture, mesh->normalSRV);
            m_materialConstsCPU.useNormalMap = true;
        }

        if (!meshData.heightTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context, meshData.heightTextureFilename, false,
                                      mesh->heightTexture, mesh->heightSRV);
            m_meshConstsCPU.useHeightMap = true;
        }

        if (!meshData.aoTextureFilename.empty()) {
            D3D11Utils::CreateTexture(device, context, meshData.aoTextureFilename, false,
                                      mesh->aoTexture, mesh->aoSRV);
            m_materialConstsCPU.useAOMap = true;
        }

        if (!meshData.metallicTextureFilename.empty() ||
            !meshData.roughnessTextureFilename.empty()) {
            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename, mesh->metallicRoughnessTexture,
                mesh->metallicRoughnessSRV);
        }

        if (!meshData.metallicTextureFilename.empty()) {
            m_materialConstsCPU.useMetallicMap = true;
        }

        if (!meshData.roughnessTextureFilename.empty()) {
            m_materialConstsCPU.useRoughnessMap = true;
        }

        mesh->vertexConstBuffer = m_meshConstsGPU;
        mesh->pixelConstBuffer = m_materialConstsGPU;

        this->m_meshes.push_back(mesh);
    }
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);

    D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU, m_materialConstsGPU);
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->VSSetConstantBuffers(0, 1, mesh->vertexConstBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, mesh->pixelConstBuffer.GetAddressOf());

        context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

        vector<ID3D11ShaderResourceView *> resViews = {
            mesh->albedoSRV.Get(), mesh->normalSRV.Get(), mesh->aoSRV.Get(),
            mesh->metallicRoughnessSRV.Get(), mesh->emissiveSRV.Get()};
        context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());

        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride,
                                    &mesh->offset);

        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mesh->indexCount, 0, 0);
    }
}

void Model::RenderNormals(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &mesh->stride,
                                    &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}

void Model::UpdateWorldRow(const Matrix &world) {
    m_world = world;
    m_worldIT = world;
    m_worldIT.Translation(Vector3(0.0f));
    m_worldIT = m_worldIT.Invert().Transpose();

    m_meshConstsCPU.world = world.Transpose();
    m_meshConstsCPU.worldIT = m_worldIT.Transpose();
}
} // namespace Luna