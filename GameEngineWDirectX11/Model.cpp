#include "pch.h"
#include "Model.h"

namespace Luna {
void Model::Initialize(ComPtr<ID3D11Device> &device, const std::string &basePath,
                       const std::string &filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);
    Initialize(device, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device, const std::vector<MeshData> &meshes) {
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    m_basicVertexConstantData.model = Matrix();
    m_basicVertexConstantData.view = Matrix();
    m_basicVertexConstantData.projection = Matrix();

    D3D11Utils::CreateConstantBuffer(device, m_basicVertexConstantData, m_vertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(device, m_basicPixelConstantData, m_pixelConstantBuffer);

    for (const auto &meshData : meshes) {
        auto newMesh = std::make_shared<Mesh>();
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->vertexBuffer);
        newMesh->m_indexCount = UINT(meshData.indices.size());
        D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->indexBuffer);

        if (!meshData.textureFilename.empty()) {

            std::cout << meshData.textureFilename << std::endl;
            D3D11Utils::CreateTexture(device, meshData.textureFilename, newMesh->texture,
                                      newMesh->textureResourceView);
        }

        newMesh->vertexConstantBuffer = m_vertexConstantBuffer;
        newMesh->pixelConstantBuffer = m_pixelConstantBuffer;

        this->m_meshes.push_back(newMesh);
    }

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"Base.vert.hlsl", basicInputElements,
                                                 m_basicVertexShader, m_basicInputLayout);
    D3D11Utils::CreatePixelShader(device, L"Base.frag.hlsl", m_basicPixelShader);
}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {

    D3D11Utils::UpdateBuffer(device, context, m_basicVertexConstantData, m_vertexConstantBuffer);

    D3D11Utils::UpdateBuffer(device, context, m_basicPixelConstantData, m_pixelConstantBuffer);
}

void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    context->VSSetShader(m_basicVertexShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->PSSetShader(m_basicPixelShader.Get(), 0, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    for (const auto &mesh : m_meshes) {

        context->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf());
        ID3D11ShaderResourceView *resViews[3] = {mesh->textureResourceView.Get(),
                                                 m_diffuseResView.Get(), m_specularResView.Get()};
        context->PSSetShaderResources(0, 3, resViews);

        context->PSSetConstantBuffers(0, 1, mesh->pixelConstantBuffer.GetAddressOf());

        context->IASetInputLayout(m_basicInputLayout.Get());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->m_indexCount, 0, 0);
    }
}
} // namespace Luna