#pragma once

#include <d3d11.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h>

namespace Luna {

using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring;

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // Set a breakpoint on this line to catch DirectX API errorss
        throw std::exception();
    }
}

class D3D11Utils {
  public:
    // Vertex Shader
    static void
    CreateVertexShaderAndInputLayout(ComPtr<ID3D11Device> &device, const wstring &filename,
                                     const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                     ComPtr<ID3D11VertexShader> &m_vertexShader,
                                     ComPtr<ID3D11InputLayout> &m_inputLayout);
    // Pixel Shader
    static void CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                  ComPtr<ID3D11PixelShader> &m_pixelShader);
    
    static void CreateGeometryShader(ComPtr<ID3D11Device> &device,
                                     const wstring &filename,
                                     ComPtr<ID3D11GeometryShader> &m_geometryShader);

    static void CreateIndexBuffer(ComPtr<ID3D11Device> &device, const vector<uint32_t> &indices,
                                  ComPtr<ID3D11Buffer> &indexBuffer);

    template <typename T_VERTEX>
    static void CreateVertexBuffer(ComPtr<ID3D11Device> &device, const vector<T_VERTEX> &vertices,
                                   ComPtr<ID3D11Buffer> &vertexBuffer) {

        // D3D11_USAGE enumeration (d3d11.h)
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(T_VERTEX);

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        const HRESULT hr =
            device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
        ThrowIfFailed(hr);
    }

    template <typename T_CONSTANT>
    static void CreateConstantBuffer(ComPtr<ID3D11Device> &device,
                                     const T_CONSTANT &constantBufferData,
                                     ComPtr<ID3D11Buffer> &constantBuffer) {
        // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
        // D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
        // D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
        // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.

        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth = sizeof(constantBufferData);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        auto hr = device->CreateBuffer(&cbDesc, &initData, constantBuffer.GetAddressOf());
        ThrowIfFailed(hr);
    }

    template <typename T_DATA>
    static void UpdateBuffer(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                             const T_DATA &bufferData, ComPtr<ID3D11Buffer> &buffer) {

        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized." << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        context->Unmap(buffer.Get(), NULL);
    }

    static void CreateTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                              const std::string filename, const bool useSRGB,
                              ComPtr<ID3D11Texture2D> &texture,
                              ComPtr<ID3D11ShaderResourceView> &textureResourceView);

    static void CreateMetallicRoughnessTexture(ComPtr<ID3D11Device> &device,
                                               ComPtr<ID3D11DeviceContext> &context,
                                               const std::string metallicFilename,
                                               const std::string roughnessFilename,
                                               ComPtr<ID3D11Texture2D> &texture,
                                               ComPtr<ID3D11ShaderResourceView> &srv);

    static void CreateTextureArray(ComPtr<ID3D11Device> &device,
                                   ComPtr<ID3D11DeviceContext> &context,
                                   const std::vector<std::string> filenames,
                                   ComPtr<ID3D11Texture2D> &texture,
                                   ComPtr<ID3D11ShaderResourceView> &textureResourceView);

    static void CreateDDSTexture(ComPtr<ID3D11Device> &device, 
                                 const wchar_t *filename, const bool bIsCubeMap,
                                 ComPtr<ID3D11ShaderResourceView> &textureResourceView);

  private:
    static void CreateTextureImpl(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context, const int width,
                                  const int height, const vector<uint8_t> &image,
                                  const DXGI_FORMAT pixelFormat,
                                  ComPtr<ID3D11Texture2D> &texture,
                                  ComPtr<ID3D11ShaderResourceView> &srv);
};
} // namespace Luna