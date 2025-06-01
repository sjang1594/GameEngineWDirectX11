#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h> // ComPtr

namespace Luna {
using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw std::exception();
    }
}

class D3D11Utils {
  public:
    static void CreateVertexShaderAndInputLayout(
        ComPtr<ID3D11Device> &device, const wstring &filename,
        const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
        ComPtr<ID3D11VertexShader> &m_vertexShader,
        ComPtr<ID3D11InputLayout> &m_inputLayout);

    static void CreateHullShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                 ComPtr<ID3D11HullShader> &hullShader);

    static void CreateDomainShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                   ComPtr<ID3D11DomainShader> &domainShader);

    static void CreateGeometryShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                     ComPtr<ID3D11GeometryShader> &geometryShader);

    static void CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                  ComPtr<ID3D11PixelShader> &pixelShader);

    template <typename T>
    static void CreateVertexBuffer(ComPtr<ID3D11Device> &device, const vector<T> &vertices,
                                   ComPtr<ID3D11Buffer> &vertexBuffer);

    static void CreateIndexBuffer(ComPtr<ID3D11Device> &device, const vector<uint32_t> &indices,
                                  ComPtr<ID3D11Buffer> &indexBuffer);

    template <typename T> 
    static void CreateConstantBuffer(ComPtr<ID3D11Device> &device, const T &constantBufferData,
                                     ComPtr<ID3D11Buffer> &constantBuffer);

    template <typename T>
    static void UpdateBuffer(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                             const T &bufferData, ComPtr<ID3D11Buffer> &buffer);

    static void CreateTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                              const std::string filename, const bool useSRGB,
                              ComPtr<ID3D11Texture2D> &texture,
                              ComPtr<ID3D11ShaderResourceView> &srv);

    static void CreateMetallicRoughnessTexture(ComPtr<ID3D11Device> &device,
                                               ComPtr<ID3D11DeviceContext> &context,
                                               const std::string metallicFileName,
                                               const std::string roughnessFileName,
                                               ComPtr<ID3D11Texture2D> &texture,
                                               ComPtr<ID3D11ShaderResourceView> &srv);

    static void CreateTextureArray(ComPtr<ID3D11Device> &device,
                                   ComPtr<ID3D11DeviceContext> &context,
                                   const std::vector<std::string> filenames,
                                   ComPtr<ID3D11Texture2D> &texture,
                                   ComPtr<ID3D11ShaderResourceView> &srv) {}

    static void CreateDDSTexture(ComPtr<ID3D11Device> &device, const wchar_t *fileName,
                                 const bool isCubeMap, ComPtr<ID3D11ShaderResourceView> &srv);
};

template <typename T>
inline void D3D11Utils::CreateVertexBuffer(ComPtr<ID3D11Device> &device, const vector<T> &vertices,
                                           ComPtr<ID3D11Buffer> &vertexBuffer) {
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Don't Change After Init
    bufferDesc.ByteWidth = UINT(sizeof(T) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(T);

    D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
    vertexBufferData.pSysMem = vertices.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    ThrowIfFailed(
        device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf()));
}

template <typename T>
inline void D3D11Utils::CreateConstantBuffer(ComPtr<ID3D11Device> &device,
                                             const T &constantBufferData,
                                             ComPtr<ID3D11Buffer> &constantBuffer) {
    // NOTE
    static_assert((sizeof(T) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ByteWidth = sizeof(constantBufferData);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = &constantBufferData;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    ThrowIfFailed(device->CreateBuffer(&desc, &initData, constantBuffer.GetAddressOf()));
}

template <typename T>
inline void D3D11Utils::UpdateBuffer(ComPtr<ID3D11Device> &device,
                                     ComPtr<ID3D11DeviceContext> &context, const T &bufferData,
                                     ComPtr<ID3D11Buffer> &buffer) {
    if (!buffer) {
        std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, &bufferData, sizeof(bufferData));
    context->Unmap(buffer.Get(), NULL);
}
} // namespace Luna
