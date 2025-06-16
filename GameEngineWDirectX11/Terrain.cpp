#include "pch.h"
#include "Terrain.h"

namespace Luna {
//Terrain::Terrain() : m_terrainHeight(0), m_terrainWidth(0) { 
//	m_terrainMesh->m_vertexBuffer = nullptr;
//    m_terrainMesh->m_indexBuffer = nullptr;
//}

void Terrain::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext>& context) {
    m_terrainHeight = 100;
    m_terrainWidth = 100;
    m_terrainMesh = std::make_shared<Mesh>();
    
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;

    D3D11_SUBRESOURCE_DATA verrtexData;
    D3D11_SUBRESOURCE_DATA indexData;
}
void Terrain::Render(ComPtr<ID3D11DeviceContext> &context) {}
bool Terrain::InitializeBuffers(ComPtr<ID3D11Device> &device) { return false; }
} // namespace Luna