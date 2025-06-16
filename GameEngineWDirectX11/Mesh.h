#pragma once

#include <d3d11.h>
#include <windows.h>
#include <wrl/client.h>

namespace Luna {

using Microsoft::WRL::ComPtr;

struct Mesh {
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

    ComPtr<ID3D11Texture2D> m_albedoTexture;
    ComPtr<ID3D11Texture2D> m_normalTexture;
    ComPtr<ID3D11Texture2D> m_heightTexture;
    ComPtr<ID3D11Texture2D> m_aoTexture;
    ComPtr<ID3D11Texture2D> m_roughnessTexture;
    ComPtr<ID3D11Texture2D> m_metalicTexture;
    ComPtr<ID3D11Texture2D> m_emissiveTexture;


    ComPtr<ID3D11ShaderResourceView> m_albedoTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_normalTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_heightTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_aoTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_roughnessTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_metalicTextureSRV;
    ComPtr<ID3D11ShaderResourceView> m_emissiveTextureSRV;

    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;
};
} // namespace Luna