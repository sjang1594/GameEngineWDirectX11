#pragma once

namespace Luna {
using Microsoft::WRL::ComPtr;

class GraphicsPSO {
  public:
    void operator=(const GraphicsPSO &pso);

    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11HullShader> m_hullShader;
    ComPtr<ID3D11DomainShader> m_domainShader;
    ComPtr<ID3D11GeometryShader> m_geometryShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    ComPtr<ID3D11BlendState> m_blendState;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;

    float m_blendFactor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    UINT m_stencilRef = 0;

    D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

class ComputePSO {
  public:
    void operator=(const ComputePSO &pso) {}
};
} // namespace Luna