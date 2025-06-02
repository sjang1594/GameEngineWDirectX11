#include "pch.h"
#include "CubeMapping.h"
#include "D3D11Utils.h"

namespace Luna {
void CubeMapping::Initialize(ComPtr<ID3D11Device> &device, const wchar_t *originalFilename,
                             const wchar_t *diffuseFilename, const wchar_t *specularFilename) {
    D3D11Utils::CreateDDSTexture(device, )
}

void CubeMapping::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                        ComPtr<ID3D11DeviceContext> &context, const Matrix &viewCol,
                                        const Matrix &projCol) {}
void CubeMapping::Render(ComPtr<ID3D11DeviceContext> &context) {}
void CubeMapping::
(ComPtr<ID3D11DeviceContext> &context) {}
} // namespace Luna