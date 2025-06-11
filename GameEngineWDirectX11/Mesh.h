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

    ComPtr<ID3D11ShaderResourceView> m_albedoTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> m_normalTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> m_heightTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> m_aoTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> m_roughnessTextureResourceView;

    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;
};
} // namespace Luna