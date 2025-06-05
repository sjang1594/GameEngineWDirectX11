#pragma once

#include <d3d11.h>
#include <windows.h>
#include <wrl/client.h>

namespace Luna {

using Microsoft::WRL::ComPtr;

struct Mesh {
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    ComPtr<ID3D11Buffer> vertexConstantBuffer;
    ComPtr<ID3D11Buffer> pixelConstantBuffer;

    ComPtr<ID3D11Texture2D> albedoTexture;
    ComPtr<ID3D11Texture2D> normalTexture;
    ComPtr<ID3D11Texture2D> heightTexture;
    ComPtr<ID3D11Texture2D> aoTexture;
    ComPtr<ID3D11Texture2D> roughnessTexture;

    ComPtr<ID3D11ShaderResourceView> albedoTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> normalTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> heightTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> aoTextureResourceView;
    ComPtr<ID3D11ShaderResourceView> roughnessTextureResourceView;

    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;
};
} // namespace Luna