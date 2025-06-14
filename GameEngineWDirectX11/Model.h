#pragma once

#include "ConstantBuffer.h"
#include "Mesh.h"
#include "MeshData.h"

namespace Luna {
using namespace std;
class Model {
  public:
    void Initialize(ComPtr<ID3D11Device> &device, 
                    ComPtr<ID3D11DeviceContext>& context, 
                    const std::string &basePath, 
                    const std::string &filename);

    void Initialize(ComPtr<ID3D11Device> &device, 
                    ComPtr<ID3D11DeviceContext>& context, 
                    const std::vector<MeshData> &meshes);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device, 
                               ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    void UpdateModelWorld(const Matrix &modelToWorld);

  public:
    Matrix m_modelWorld = Matrix();
    Matrix m_modelWorldIT = Matrix();

    BasicVertexConstantData m_basicVertexConstantData;
    BasicPixelConstantData m_basicPixelConstantData;

    ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
    ComPtr<ID3D11ShaderResourceView> m_specularSRV;
    ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

    NormalVertexConstantData m_normalVertexConstantData;
    
  private:
    std::vector<shared_ptr<Mesh>> m_meshes;

    ComPtr<ID3D11VertexShader> m_basicVertexShader;
    ComPtr<ID3D11PixelShader> m_basicPixelShader;
    ComPtr<ID3D11InputLayout> m_basicInputLayout;

    ComPtr<ID3D11SamplerState> m_linearSamplerState;
    ComPtr<ID3D11SamplerState> m_clmapSamplerState;

    ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

    ComPtr<ID3D11VertexShader> m_normalVertexShader;
    ComPtr<ID3D11PixelShader> m_normalPixelShader;

    ComPtr<ID3D11Buffer> m_normalVertexConstantBuffer;
    ComPtr<ID3D11Buffer> m_normalPixelConstantBuffer;
};

} // namespace Luna
