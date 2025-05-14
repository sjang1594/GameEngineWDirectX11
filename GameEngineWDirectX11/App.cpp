#include "pch.h"
#include "App.h"

namespace Luna {
auto MakeBox() {
    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;

    const float scale = 1.0f;

// À­¸é
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    // ¾Æ·§¸é
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    // ¾Õ¸é
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    // µÞ¸é
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    // ¿ÞÂÊ
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    // ¿À¸¥ÂÊ
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));

    vector<Vertex> vertices;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        vertices.push_back(v);
    }
    
    vector<uint16_t> indices = {
        0,  1,  2,  0,  2,  3,  // Upside
        4,  5,  6,  4,  6,  7,  // downside
        8,  9,  10, 8,  10, 11, // frontside
        12, 13, 14, 12, 14, 15, // backside
        16, 17, 18, 16, 18, 19, // leftside
        20, 21, 22, 20, 22, 23  // rightside
    };

    return std::tuple{vertices, indices};
}
	
App::App() : EngineBase(), _indexCount(0) {}

bool App::Initialize() { 
    if (!EngineBase::Initialize())
        return false;

    auto [vertices, indices] = MakeBox();

    EngineBase::CreateVertexBuffer(vertices, _vertexBuffer);

    _indexCount = UINT(indices.size());

    EngineBase::CreateIndexBuffer(indices, _indexBuffer);

    // Constant Buffer
    _constantBufferData.model = Matrix();
    _constantBufferData.view = Matrix();
    _constantBufferData.projection = Matrix();

    EngineBase::CreateConstantBuffer(_constantBufferData, _constantBuffer);

    // Layout
    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    EngineBase::CreateVertexShaderAndInputLayout(L"Color.vert.hlsl", inputElements,
                                              _colorVertexShader, _colorInputLayout);

    EngineBase::CreatePixelShader(L"Color.frag.hlsl", _colorPixelShader);

    return true; 
}

void App::UpdateGUI() { 
    ImGui::Checkbox("usePerspectiveProjection", &_usePerspectiveProjection); 
}

void App::Update(float dt) {
    static float rot = 0.0f;
    rot += dt;
    _constantBufferData.model = Matrix::CreateScale(0.5f) * Matrix::CreateRotationY(rot) *
                                 Matrix::CreateTranslation(Vector3(0.0f, -0.3f, 1.0f));
    _constantBufferData.model = _constantBufferData.model.Transpose();

    using namespace DirectX;

    // ½ÃÁ¡ º¯È¯
    _constantBufferData.view =
        XMMatrixLookAtLH({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
    _constantBufferData.view = _constantBufferData.view.Transpose();

    const float aspect = EngineBase::GetAspectRatio();
    if (_usePerspectiveProjection) {
        const float fovAngleY = 70.0f * XM_PI / 180.0f;
        _constantBufferData.projection =
            XMMatrixPerspectiveFovLH(fovAngleY, aspect, 0.01f, 100.0f);
    } else {
        _constantBufferData.projection =
            XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
    }
    _constantBufferData.projection = _constantBufferData.projection.Transpose();
    EngineBase::UpdateBuffer(_constantBufferData, _constantBuffer);
}

void App::Render() {
    // READY
    _d3dContext->RSSetViewports(1, &_screenViewport);
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    _d3dContext->ClearRenderTargetView(_d3dRenderTargetView.Get(), clearColor);
    _d3dContext->ClearDepthStencilView(_d3dDepthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    _d3dContext->OMSetRenderTargets(1, _d3dRenderTargetView.GetAddressOf(),
                                    _d3dDepthStencilView.Get());

    _d3dContext->OMSetDepthStencilState(_d3dDepthStencilState.Get(), 0);
    _d3dContext->VSSetShader(_colorVertexShader.Get(), 0, 0);
    _d3dContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
    _d3dContext->PSSetShader(_colorPixelShader.Get(), 0, 0);
    _d3dContext->RSSetState(_d3dRasterizerState.Get());

    // Bind
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    _d3dContext->IASetInputLayout(_colorInputLayout.Get());
    _d3dContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    _d3dContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    _d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _d3dContext->DrawIndexed(_indexCount, 0, 0);
}
} // namespace Luna