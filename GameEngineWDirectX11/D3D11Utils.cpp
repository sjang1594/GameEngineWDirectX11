#include "pch.h"
#include "D3D11Utils.h"
#include <DirectXTexEXR.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>

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

ComPtr<ID3D11Texture2D> CreateStagingTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                     const int width, const int height, const std::vector<uint8_t> &image,
                     const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
                     const int mipLevels = 1, const int arraySize = 1) {

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = pixelFormat;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(device->CreateTexture2D(&txtDesc, NULL, stagingTexture.GetAddressOf()))) {
        std::cout << "Failed()" << std::endl;
    }

    // CPU에서 이미지 데이터 복사
    size_t pixelSize = sizeof(uint8_t) * 4;
    if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) {
        pixelSize = sizeof(uint16_t) * 4;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t *pData = (uint8_t *)ms.pData;
    for (UINT h = 0; h < UINT(height); h++) {
        memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize], width * pixelSize);
    }
    context->Unmap(stagingTexture.Get(), NULL);

    return stagingTexture;
}

void ReadEXRImage(const std::string filename, std::vector<uint8_t> &image, int &width, int &height,
                  DXGI_FORMAT &pixelFormat) {

    const std::wstring wFilename(filename.begin(), filename.end());

    DirectX::TexMetadata metadata;
    ThrowIfFailed(GetMetadataFromEXRFile(wFilename.c_str(), metadata));

    DirectX::ScratchImage scratchImage;
    ThrowIfFailed(LoadFromEXRFile(wFilename.c_str(), NULL, scratchImage));

    width = static_cast<int>(metadata.width);
    height = static_cast<int>(metadata.height);
    pixelFormat = metadata.format;

    std::cout << filename << " " << metadata.width << " " << metadata.height << metadata.format << std::endl;

    image.resize(scratchImage.GetPixelsSize());
    memcpy(image.data(), scratchImage.GetPixels(), image.size());

    vector<float> f32(image.size() / 2);
    uint16_t *f16 = (uint16_t *)image.data();
    for (int i = 0; i < image.size() / 2; i++) {
        f32[i] = fp16_ieee_to_fp32_value(f16[i]);
    }

    const float minValue = *std::min_element(f32.begin(), f32.end());
    const float maxValue = *std::max_element(f32.begin(), f32.end());

    std::cout << minValue << " " << maxValue << std::endl;
}

void ReadImage(const std::string filename, std::vector<uint8_t> &image, int &width, int &height) {
    int channels;
    unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    assert(channels == 4);

    std::cout << filename << " " << width << " " << height << " " << channels << std::endl;

    // 4채널로 만들어서 복사
    image.resize(width * height * 4);

    if (channels == 1) {
        for (size_t i = 0; i < width * height; i++) {
            uint8_t g = img[i * channels + 0];
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = g;
            }
        }
    } else if (channels == 2) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 2; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 2] = 255;
            image[4 * i + 3] = 255;
        }
    } else if (channels == 3) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 3; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
            image[4 * i + 3] = 255;
        }
    } else if (channels == 4) {
        for (size_t i = 0; i < width * height; i++) {
            for (size_t c = 0; c < 4; c++) {
                image[4 * i + c] = img[i * channels + c];
            }
        }
    } else {
        std::cout << "Cannot read " << channels << " channels" << std::endl;
    }

    delete[] img;
}

void CreateTextureHelper(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                         const int width, const int height, const vector<uint8_t> &image,
                         const DXGI_FORMAT pixelFormat, ComPtr<ID3D11Texture2D> &texture,
                         ComPtr<ID3D11ShaderResourceView> &srv) {

    ComPtr<ID3D11Texture2D> stagingTexture =
        CreateStagingTexture(device, context, width, height, image, pixelFormat);

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 0; 
    txtDesc.ArraySize = 1;
    txtDesc.Format = pixelFormat;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; 
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; 
    txtDesc.CPUAccessFlags = 0;

    device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
    // texture->GetDesc(&txtDesc);
    // cout << txtDesc.MipLevels << endl;

    context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, NULL);
    device->CreateShaderResourceView(texture.Get(), 0, srv.GetAddressOf());
    context->GenerateMips(srv.Get());
}

void D3D11Utils::CreateTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                               const std::string filename, const bool useSRGB,
                               ComPtr<ID3D11Texture2D> &texture,
                               ComPtr<ID3D11ShaderResourceView> &srv) {
    int width = 0, height = 0;
    std::vector<uint8_t> image;
    DXGI_FORMAT pixelFormat =
        useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

    std::string extension(filename.end() - 3, filename.end());
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "exr") {
        ReadEXRImage(filename, image, width, height, pixelFormat);
    } else {
        ReadImage(filename, image, width, height);
    }

    CreateTextureHelper(device, context, width, height, image, pixelFormat, texture, srv);
}

void D3D11Utils::CreateMetallicRoughnessTexture(ComPtr<ID3D11Device> &device,
                                                ComPtr<ID3D11DeviceContext> &context,
                                                const std::string metallicFileName,
                                                const std::string roughnessFileName,
                                                ComPtr<ID3D11Texture2D> &texture,
                                                ComPtr<ID3D11ShaderResourceView> &srv) {}

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