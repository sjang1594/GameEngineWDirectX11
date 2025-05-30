#include "pch.h"
#include "D3D11Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace Luna {

/* SHADER DEBUG */ 
void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
    if (FAILED(hr)) {
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            std::cout << "File not found." << std::endl;
        }
        if (errorBlob) {
            std::cout << "Shader compile error\n" << (char *)errorBlob->GetBufferPointer() << std::endl;
        }
    }
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
    ComPtr<ID3D11Device> &device, const wstring &filename,
    const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
    ComPtr<ID3D11VertexShader> &m_vertexShader, ComPtr<ID3D11InputLayout> &m_inputLayout) {
    
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
  
    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "vs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                               &m_vertexShader);

    device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
                              shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
                              &m_inputLayout);
}

void D3D11Utils::CreateHullShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                  ComPtr<ID3D11HullShader> &hullShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);
    CheckResult(hr, errorBlob.Get());
    device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                             &hullShader);
}

void D3D11Utils::CreateDomainShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                    ComPtr<ID3D11DomainShader> &domainShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                               &domainShader);
}

void D3D11Utils::CreateGeometryShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                      ComPtr<ID3D11GeometryShader> &geometryShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;
    
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                                 &geometryShader);
}

void D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                   ComPtr<ID3D11PixelShader> &pixelShader) {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
                                    "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

    CheckResult(hr, errorBlob.Get());

    device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL,
                              &pixelShader);
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device> &device, const vector<uint32_t> &indices,
                                   ComPtr<ID3D11Buffer> &indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Don't Change After Init
    bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
    bufferDesc.StructureByteStride = sizeof(uint32_t);

    D3D11_SUBRESOURCE_DATA indexBufferData = {0};
    indexBufferData.pSysMem = indices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
}

void D3D11Utils::CreateDDSTexture(ComPtr<ID3D11Device> &device, const wchar_t *fileName,
                                  const bool isCubeMap, ComPtr<ID3D11ShaderResourceView> &srv) {
    ComPtr<ID3D11Texture2D> texture2D;

    UINT miscFlags = 0;
    if (isCubeMap) {
        miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(
        device.Get(), fileName, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, miscFlags,
        DirectX::DDS_LOADER_FLAGS(false), (ID3D11Resource **)texture2D.GetAddressOf(),
        srv.GetAddressOf(), NULL));
}

} // namespace Luna