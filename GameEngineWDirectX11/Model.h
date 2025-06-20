#pragma once

#include "ConstantBuffer.h"
#include "Mesh.h"
#include "MeshData.h"

namespace Luna {
using namespace std;
class Model {
  public:
    Model() = default;
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::string &basePath, const std::string &filename);
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &meshes);

    virtual ~Model() = default;

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
    void RenderNormal(ComPtr<ID3D11DeviceContext> &context);
    void UpdateModelWorld(const Matrix &modelToWorld);

    Matrix m_modelWorld = Matrix();
    Matrix m_modelWorldIT = Matrix();

    MeshConstants m_meshConstsCPU;
    ComPtr<ID3D11Buffer> m_meshConstsGPU;
    MaterialConstants m_materialConstsCPU;
    ComPtr<ID3D11Buffer> m_materialConstsGPU;
    
    bool m_drawNormals = false;
    bool m_drawNormalsDirtyFlags = true;
 
  private:
    std::vector<shared_ptr<Mesh>> m_meshes;

};

} // namespace Luna
