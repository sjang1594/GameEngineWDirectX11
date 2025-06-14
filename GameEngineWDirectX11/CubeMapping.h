#pragma once

#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include "ConstantBuffer.h"

namespace Luna {
using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Matrix;

class CubeMapping {
  public:
    void Initialize(ComPtr<ID3D11Device> &device, 
                    const wchar_t *envFilename,
                    const wchar_t *diffuseFilename, 
                    const wchar_t *specularFilename,
                    const wchar_t *brdfFilename);

    void UpdateVertexConstantBuffers(ComPtr<ID3D11Device> &device, 
                                     ComPtr<ID3D11DeviceContext> &context,
                                     const Matrix &viewCol, 
                                     const Matrix &projCol);

    void UpdatePixelConstantBuffers(ComPtr<ID3D11Device> &device,
                                    ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    ComPtr<ID3D11ShaderResourceView> m_envSRV;
    ComPtr<ID3D11ShaderResourceView> m_diffuseSRV; // = irridiance
    ComPtr<ID3D11ShaderResourceView> m_specularSRV;
    ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

    CubeMappingVertexConstantData m_vertexConstantData;
    CubeMappingPixelConstantData m_pixelConstantData;
  
  private:
    std::shared_ptr<class Mesh> m_cubeMesh;

    ComPtr<ID3D11SamplerState> m_samplerState;

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
};
} // namespace Luna