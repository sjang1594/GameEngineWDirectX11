#pragma once
#include "GraphicsPSO.h"
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Luna {
namespace Graphics {
using Microsoft::WRL::ComPtr;
	// Samplers State
	extern ComPtr<ID3D11SamplerState> linearWrapSS;
	extern ComPtr<ID3D11SamplerState> linearClampSS;
    extern std::vector<ID3D11SamplerState *> sampleStates;

	// Rasterizer State
    extern ComPtr<ID3D11RasterizerState> solidRS;
    extern ComPtr<ID3D11RasterizerState> solidCCWRS; // Counter-ClockWise
    extern ComPtr<ID3D11RasterizerState> wireRS;
    extern ComPtr<ID3D11RasterizerState> wireCCWRS;

    // Depth Stencil States
    extern ComPtr<ID3D11DepthStencilState> drawDSS;       
    extern ComPtr<ID3D11DepthStencilState> maskDSS;       
    extern ComPtr<ID3D11DepthStencilState> drawMaskedDSS;

    // Shader
    extern ComPtr<ID3D11VertexShader> basicVS;
    extern ComPtr<ID3D11PixelShader> basicPS;
    extern ComPtr<ID3D11VertexShader> skyboxVS;
    extern ComPtr<ID3D11PixelShader> skyboxPS;

    // Input Layouts
    extern ComPtr<ID3D11InputLayout> basicIL;
    extern ComPtr<ID3D11InputLayout> skyboxIL;

    // Graphics PipelineStates
    extern GraphicsPSO defaultSolidPSO;
    extern GraphicsPSO defaultWirePSO;
    extern GraphicsPSO stencilMaskPSO;
    extern GraphicsPSO skyboxSolidPSO;
    extern GraphicsPSO skyboxWirePSO;

    void InitCommonStates(ComPtr<ID3D11Device> &device);
    void InitSamplers(ComPtr<ID3D11Device> &device);
    void InitRasterizerStates(ComPtr<ID3D11Device> &device);
    void InitBlendStates(ComPtr<ID3D11Device> &device);
    void InitDepthStencilStates(ComPtr<ID3D11Device> &device);
    void InitPipelineStates(ComPtr<ID3D11Device> &device);
    void InitShaders(ComPtr<ID3D11Device> &device);
}
}