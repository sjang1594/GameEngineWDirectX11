#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "EngineBase.h"

namespace Luna {
	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector3;

	struct Vertex {
        Vector3 position;
        Vector3 color;
	};

	struct ModelViewProjectionConstantBuffer {
        Matrix model;
        Matrix view;
        Matrix projection;
	};

	class App : public EngineBase {
      public:
        App();

		virtual bool Initialize() override;
        virtual void UpdateGUI() override;
        virtual void Update(float dt) override;
        virtual void Render() override;

      protected:
        ComPtr<ID3D11VertexShader>          _colorVertexShader;
        ComPtr<ID3D11PixelShader>           _colorPixelShader;
        ComPtr<ID3D11InputLayout>           _colorInputLayout;

        ComPtr<ID3D11Buffer>                _vertexBuffer;
        ComPtr<ID3D11Buffer>                _indexBuffer;
        ComPtr<ID3D11Buffer>                _constantBuffer;
        UINT                                _indexCount;

        ModelViewProjectionConstantBuffer   _constantBufferData;
        bool m_usePerspectiveProjection = true;
	};
    }
