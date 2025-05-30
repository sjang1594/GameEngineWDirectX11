#pragma once
#include "Vertex.h"
#include <wrl.h>
#include <d3d11.h>

namespace Luna {
using Microsoft::WRL::ComPtr;

struct Mesh {
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    ComPtr<ID3D11Buffer> vertexConstBuffer;
    ComPtr<ID3D11Buffer> pixelConstBuffer;

    ComPtr<ID3D11Texture2D> albedoTexture;
    ComPtr<ID3D11Texture2D> emissiveTexture;
    ComPtr<ID3D11Texture2D> normalTexture;
    ComPtr<ID3D11Texture2D> heightTexture;
    ComPtr<ID3D11Texture2D> aoTexture;
    ComPtr<ID3D11Texture2D> metallicRoughnessTexture;

    ComPtr<ID3D11ShaderResourceView> albedoSRV;
    ComPtr<ID3D11ShaderResourceView> emissiveSRV;
    ComPtr<ID3D11ShaderResourceView> normalSRV;
    ComPtr<ID3D11ShaderResourceView> heightSRV;
    ComPtr<ID3D11ShaderResourceView> aoSRV;
    ComPtr<ID3D11ShaderResourceView> metallicRoughnessSRV;

    UINT indexCount = 0; // Number of indiecs = 3 * number of triangles
    UINT vertexCount = 0;
    UINT stride = 0;
    UINT offset = 0;
};
} // namespace Luna
