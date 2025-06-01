#pragma once
#include <vector>
#include <memory>
#include "Mesh.h"
#include "ConstantBuffer.h"

namespace Luna {
using Microsoft::WRL::ComPtr;
class Model {
  public:
    Model() = default;
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::string &basePath, const std::string &filename);
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::vector<class MeshData> &meshes);

    void Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                    const std::string &basePath, const std::string &filename);

    void Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                    const std::vector<MeshData> &meshes);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    void RenderNormals(ComPtr<ID3D11DeviceContext> &context);

    void UpdateWorldRow(const Matrix &world);

    Matrix m_world = Matrix();
    Matrix m_worldIT = Matrix();

    MeshConstants m_meshConstsCPU;
    MaterialConstants m_materialConstsCPU;

    bool m_drawNormal = false;

    std::vector<std::shared_ptr<Mesh>> m_meshes;

  private:
    ComPtr<ID3D11Buffer> m_meshConstsGPU;
    ComPtr<ID3D11Buffer> m_materialConstsGPU;
};
} // namespace Luna
