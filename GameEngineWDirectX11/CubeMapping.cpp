#include "pch.h"
#include "CubeMapping.h"
#include "Mesh.h"
#include "D3D11Utils.h"
#include "GeometryGenerator.h"

namespace Luna {
void CubeMapping::Initialize(ComPtr<ID3D11Device> &device, 
                             const wchar_t *envFilename,
                             const wchar_t *diffuseFilename, 
                             const wchar_t *specularFilename, 
                             const wchar_t *brdfFilename) {

    D3D11Utils::CreateDDSTexture(device, envFilename, true, m_envSRV);
    D3D11Utils::CreateDDSTexture(device, diffuseFilename, true, m_diffuseSRV);
    D3D11Utils::CreateDDSTexture(device, specularFilename, true, m_specularSRV);
    D3D11Utils::CreateDDSTexture(device, brdfFilename, false, m_brdfSRV);

    m_cubeMesh = std::make_shared<Mesh>();

    D3D11Utils::CreateConstantBuffer(device, m_vertexConstantData,
                                     m_cubeMesh->m_vertexConstantBuffer);

    D3D11Utils::CreateConstantBuffer(device, m_pixelConstantData,
                                     m_cubeMesh->m_pixelConstantBuffer);

    MeshData cubeMeshData = GeometryGenerator::MakeBox(40.0f);
    std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

    D3D11Utils::CreateVertexBuffer(device, cubeMeshData.vertices, m_cubeMesh->m_vertexBuffer);
    
    m_cubeMesh->m_indexCount = UINT(cubeMeshData.indices.size());
    D3D11Utils::CreateIndexBuffer(device, cubeMeshData.indices, m_cubeMesh->m_indexBuffer);

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SkyBox.vert.hlsl",
                                                 basicInputElements, m_vertexShader, m_inputLayout);

    D3D11Utils::CreatePixelShader(device, L"SkyBox.frag.hlsl", m_pixelShader);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the Sample State
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());
}

void CubeMapping::UpdateVertexConstantBuffers(ComPtr<ID3D11Device> &device,
                                              ComPtr<ID3D11DeviceContext> &context, 
                                              const Matrix &viewCol,
                                              const Matrix &projCol) {

    m_vertexConstantData.viewProj = projCol * viewCol;
    D3D11Utils::UpdateBuffer(device, context, m_vertexConstantData,
                             m_cubeMesh->m_vertexConstantBuffer);
}

void CubeMapping::UpdatePixelConstantBuffers(ComPtr<ID3D11Device> &device,
                                             ComPtr<ID3D11DeviceContext> &context) {
    D3D11Utils::UpdateBuffer(device, context, m_pixelConstantData,
                             m_cubeMesh->m_pixelConstantBuffer);
}

void CubeMapping::Render(ComPtr<ID3D11DeviceContext> &context) {

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetVertexBuffers(0, 1, m_cubeMesh->m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_cubeMesh->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->VSSetConstantBuffers(0, 1, m_cubeMesh->m_vertexConstantBuffer.GetAddressOf());

    std::vector<ID3D11ShaderResourceView*> srvs = {
        m_envSRV.Get(), m_specularSRV.Get(), m_diffuseSRV.Get(), 
    };

    context->PSSetShaderResources(0, UINT(srvs.size()), srvs.data());
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_cubeMesh->m_pixelConstantBuffer.GetAddressOf());
    context->DrawIndexed(m_cubeMesh->m_indexCount, 0, 0);
}
} // namespace Luna