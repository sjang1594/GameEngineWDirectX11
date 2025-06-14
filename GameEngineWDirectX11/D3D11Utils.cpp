#include "pch.h"
#include "D3D11Utils.h"
#include "ImageUtils.h"

namespace Luna {
using namespace std;
using namespace DirectX;

void CheckResult(HRESULT hr, ID3DBlob *errorBlob) {
    if (FAILED(hr)) {
        if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
            cout << "File not found." << endl;
        }

        if (errorBlob) {
            cout << "Shader compile error\n" << (char *)errorBlob->GetBufferPointer() << endl;
        }
    }
}

bool D3D11Utils::CreateDepthBuffer(ComPtr<ID3D11Device> &device, int screenWidth, int screenHeight,
                                   UINT &numQualityLevels,
                                   ComPtr<ID3D11DepthStencilView> &depthStencilView) {

    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    depthStencilBufferDesc.Width = screenWidth;
    depthStencilBufferDesc.Height = screenHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if (numQualityLevels > 0) {
        depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
        depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
    } else {
        depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
        depthStencilBufferDesc.SampleDesc.Quality = 0;
    }
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> depthStencilBuffer;

    if (FAILED(device->CreateTexture2D(&depthStencilBufferDesc, 0,
                                       depthStencilBuffer.GetAddressOf()))) {
        std::cout << "CreateTexture2D() failed." << std::endl;
    }
    if (FAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), 0,
                                              depthStencilView.GetAddressOf()))) {
        std::cout << "CreateDepthStencilView() failed." << std::endl;
    }
    return true;
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

void D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                                   ComPtr<ID3D11PixelShader> &m_pixelShader) {
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
                              &m_pixelShader);
}

void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device> &device,
                                   const std::vector<uint32_t> &indices,
                                   ComPtr<ID3D11Buffer> &indexBuffer) {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
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

ComPtr<ID3D11Texture2D> CreateStagingTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext>& context, 
                                             const int width, const int height, const std::vector<uint8_t>& image, 
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
    ThrowIfFailed(device->CreateTexture2D(&txtDesc, nullptr, stagingTexture.GetAddressOf()));

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

void D3D11Utils::CreateTexture(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                               const std::string filename, const bool useSRGB,
                               ComPtr<ID3D11Texture2D> &texture,
                               ComPtr<ID3D11ShaderResourceView> &textureResourceView) {
    int width = 0, height = 0;
    vector<uint8_t> image;
    DXGI_FORMAT pixelFormat =
        useSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    
    string extension(filename.end() - 3, filename.end());
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (extension == "exr") {
        ImageUtils::ReadEXRImage(filename, image, width, height, pixelFormat);
    } else {
        ImageUtils::ReadImage(filename, image, width, height);
    }

    ComPtr<ID3D11Texture2D> stagingTexture =
        CreateStagingTexture(device, context, width, height, image, pixelFormat);

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 0;
    desc.ArraySize = 1;
    desc.Format = pixelFormat;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    desc.CPUAccessFlags = 0;

    device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());

    context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, nullptr);
    device->CreateShaderResourceView(texture.Get(), 0, textureResourceView.GetAddressOf());
    context->GenerateMips(textureResourceView.Get());
}

void D3D11Utils::CreateTextureArray(ComPtr<ID3D11Device> &device,
                                    ComPtr<ID3D11DeviceContext> &context,
                                    const std::vector<std::string> filenames,
                                    ComPtr<ID3D11Texture2D> &texture,
                                    ComPtr<ID3D11ShaderResourceView> &textureResourceView) {
    if (filenames.empty())
        return;

    int width = 0, height = 0;
    vector<vector<uint8_t>> imageArray;
    for (const auto &f : filenames) {

        cout << f << endl;

        std::vector<uint8_t> image;

        ImageUtils::ReadImage(f, image, width, height);

        imageArray.push_back(image);
    }

    UINT size = UINT(filenames.size());

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = UINT(width);
    txtDesc.Height = UINT(height);
    txtDesc.MipLevels = 0;
    txtDesc.ArraySize = size;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; 

    device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());
    texture->GetDesc(&txtDesc);
    
    for (size_t i = 0; i < imageArray.size(); i++) {

        auto &image = imageArray[i];

        ComPtr<ID3D11Texture2D> stagingTexture =
            CreateStagingTexture(device, context, width, height, image, txtDesc.Format, 1, 1);

        UINT subresourceIndex = D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

        context->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0,
                                       stagingTexture.Get(), 0, NULL);
    }

    device->CreateShaderResourceView(texture.Get(), NULL, textureResourceView.GetAddressOf());

    context->GenerateMips(textureResourceView.Get());
}

void D3D11Utils::CreateDDSTexture(ComPtr<ID3D11Device> &device, const wchar_t *filename,
                                  const bool bIsCubeMap,
                                  ComPtr<ID3D11ShaderResourceView> &textureResourceView) {
    ComPtr<ID3D11Texture2D> texture;

    // https://github.com/microsoft/DirectXTK/wiki/DDSTextureLoader
    ThrowIfFailed(CreateDDSTextureFromFileEx(
        device.Get(), filename, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false),
        (ID3D11Resource **)texture.GetAddressOf(), textureResourceView.GetAddressOf(),
        nullptr));
}
} // namespace Luna