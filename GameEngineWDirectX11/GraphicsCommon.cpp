#include "pch.h"
#include "GraphicsCommon.h"
#include "D3D11Utils.h"

namespace Luna {
namespace Graphics {
// Samplers State
ComPtr<ID3D11SamplerState> linearWrapSS;
ComPtr<ID3D11SamplerState> linearClampSS;
vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
ComPtr<ID3D11RasterizerState> solidRS;
ComPtr<ID3D11RasterizerState> solidCCWRS;
ComPtr<ID3D11RasterizerState> wireRS;
ComPtr<ID3D11RasterizerState> wireCCWRS;

// Depth Stencil States
ComPtr<ID3D11DepthStencilState> drawDSS;    
ComPtr<ID3D11DepthStencilState> maskDSS;  
ComPtr<ID3D11DepthStencilState> drawMaskedDSS; 

// Shaders
ComPtr<ID3D11VertexShader> basicVS;
ComPtr<ID3D11VertexShader> skyboxVS;
ComPtr<ID3D11PixelShader> basicPS;
ComPtr<ID3D11PixelShader> skyboxPS;

// Input Layouts
ComPtr<ID3D11InputLayout> basicIL;
ComPtr<ID3D11InputLayout> skyboxIL;

// Graphics Pipeline States
GraphicsPSO defaultSolidPSO;
GraphicsPSO defaultWirePSO;
GraphicsPSO stencilMaskPSO;
GraphicsPSO skyboxSolidPSO;
GraphicsPSO skyboxWirePSO;

void InitCommonStates(ComPtr<ID3D11Device> &device) 
{
    InitShaders(device);
    InitSamplers(device);
    InitRasterizerStates(device);
    InitBlendStates(device);
    InitDepthStencilStates(device);
    InitPipelineStates(device);
}

void InitSamplers(ComPtr<ID3D11Device> &device) {
    // Basic Samplers
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, linearWrapSS.GetAddressOf());

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf());

    sampleStates.push_back(linearWrapSS.Get());
    sampleStates.push_back(linearClampSS.Get());
}

void InitRasterizerStates(ComPtr<ID3D11Device> &device) {
    // Rasterizer States
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;

    ThrowIfFailed(device->CreateRasterizerState(&rastDesc, solidRS.GetAddressOf()));

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    ThrowIfFailed(device->CreateRasterizerState(&rastDesc, wireCCWRS.GetAddressOf()));
}

void InitBlendStates(ComPtr<ID3D11Device> &device) {

}

void InitDepthStencilStates(ComPtr<ID3D11Device> &device) {
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false; // Stencil ºÒÇÊ¿ä
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    // Front Face
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // Back Face
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    
    ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));
}

void InitPipelineStates(ComPtr<ID3D11Device> &device) {
    // Define Pass
    // DefaultSolidPSO State
    defaultSolidPSO.m_vertexShader = basicVS;
    defaultSolidPSO.m_inputLayout = basicIL;
    defaultSolidPSO.m_pixelShader = basicPS;
    defaultSolidPSO.m_rasterizerState = solidRS;
    defaultSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        // defaultWirePSO
    defaultWirePSO = defaultSolidPSO;
    defaultWirePSO.m_rasterizerState = wireRS;

    // skyboxSolidPSO
    skyboxSolidPSO = defaultSolidPSO;
    skyboxSolidPSO.m_vertexShader = skyboxVS;
    skyboxSolidPSO.m_pixelShader = skyboxPS;
    skyboxSolidPSO.m_inputLayout = skyboxIL;

    // skyboxWirePSO
    skyboxWirePSO = skyboxSolidPSO;
    skyboxWirePSO.m_rasterizerState = wireRS;
}

void InitShaders(ComPtr<ID3D11Device> &device) {
    std::vector<D3D11_INPUT_ELEMENT_DESC> basicIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> skyboxIE = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"BasePass.vert.hlsl", basicIEs, basicVS,
                                                 basicIL);
    D3D11Utils::CreatePixelShader(device, L"BasePass.frag.hlsl", basicPS);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SkyBox.vert.hlsl", skyboxIE, skyboxVS,
                                                 skyboxIL);
    D3D11Utils::CreatePixelShader(device, L"SkyBox.frag.hlsl", basicPS);
}



} // namespace Graphics
}