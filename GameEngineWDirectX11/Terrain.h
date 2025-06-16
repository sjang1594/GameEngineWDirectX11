#pragma once

#include "Mesh.h"

namespace Luna {

using namespace Microsoft::WRL;
class Terrain {
  public:
    Terrain() = default;
    virtual ~Terrain() = default;
    void Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext> & context);
    void Render(ComPtr<ID3D11DeviceContext> & context);
    int GetIndexCount() const { return m_terrainMesh->m_indexCount; }
    bool InitializeBuffers(ComPtr<ID3D11Device>& device);
  private:
    int m_terrainWidth;
    int m_terrainHeight;
    std::shared_ptr<Mesh> m_terrainMesh;
};
} // namespace Luna