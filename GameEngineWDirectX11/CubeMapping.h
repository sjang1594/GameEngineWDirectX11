#pragma once

#include <wrl/client.h>
#include <directxtk/SimpleMath.h>

namespace Luna {
using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Matrix;
class CubeMapping {
  public:
    void Initialize(ComPtr<ID3D11Device> &device, const wchar_t *originalFilename,
                    const wchar_t *diffuseFilename, const wchar_t *specularFilename);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                               const Matrix &viewCol, const Matrix &projCol);

    void Render(ComPtr<ID3D11DeviceContext> &context);

  public:
    struct VertexConstantData {
        Matrix viewProj;
    };

    static_assert((sizeof(VertexConstantData) % 16) == 0,
                  "Constant Buffer size must be 16-byte aligned");

  public:
    ComPtr<ID3D11ShaderResourceView> m_originalResView;
    ComPtr<ID3D11ShaderResourceView> m_diffuseResView;
    ComPtr<ID3D11ShaderResourceView> m_specularResView;

  private:
    std::shared_ptr<class Mesh> m_cubeMesh;

    ComPtr<ID3D11SamplerState> m_samplerState;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    CubeMapping::VertexConstantData vertexConstantData;
};
} // namespace Luna