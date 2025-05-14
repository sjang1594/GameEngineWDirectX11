#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <wrl.h> 

namespace Luna {
	using Microsoft::WRL::ComPtr;
	using std::vector;
	using std::wstring;
	
	class EngineBase {
      public:
        EngineBase();
        virtual ~EngineBase();

        float GetAspectRatio() const;

        int Run();

        virtual bool Initialize();
        virtual void UpdateGUI() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;

        virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
        virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
        virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

      protected:
        bool InitMainWindow();
        bool InitD3D();
        bool InitGUI();
        void CreateVertexShaderAndInputLayout(const wstring &filename,
                                              const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
                                              ComPtr<ID3D11VertexShader> &vertexShader,
                                              ComPtr<ID3D11InputLayout> &inputLayout);
        void CreatePixelShader(const wstring &filename, ComPtr<ID3D11PixelShader> &pixelShader);
        void CreateIndexBuffer(const vector<uint16_t> &indices,
                               ComPtr<ID3D11Buffer> &_indexBuffer);

        template <typename T_VERT>
        void CreateVertexBuffer(const vector<T_VERT> &vertices,
                                ComPtr<ID3D11Buffer> &vertexBuffer) {

            D3D11_BUFFER_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(bufferDesc));
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDesc.ByteWidth = UINT(sizeof(T_VERT) * vertices.size());
            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufferDesc.CPUAccessFlags = 0; 
            bufferDesc.StructureByteStride = sizeof(T_VERT);

            D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
            vertexBufferData.pSysMem = vertices.data();
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;

            const HRESULT hr = _d3dDevice->CreateBuffer(&bufferDesc, &vertexBufferData,
                                                        vertexBuffer.GetAddressOf());
            if (FAILED(hr)) {
                std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
            };
        }

        template <typename T_CONST>
        void CreateConstantBuffer(const T_CONST &constantBufferData,
                                  ComPtr<ID3D11Buffer> &constantBuffer) {
            D3D11_BUFFER_DESC bufferDesc;
            bufferDesc.ByteWidth = sizeof(constantBufferData);
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;

            // Fill in the subresource data.
            D3D11_SUBRESOURCE_DATA InitData;
            InitData.pSysMem = &constantBufferData;
            InitData.SysMemPitch = 0;
            InitData.SysMemSlicePitch = 0;

            _d3dDevice->CreateBuffer(&bufferDesc, &InitData, constantBuffer.GetAddressOf());
        }

        template <typename T_DATA>
        void UpdateBuffer(const T_DATA &bufferData, ComPtr<ID3D11Buffer> &buffer) {
            D3D11_MAPPED_SUBRESOURCE ms;
            _d3dContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
            memcpy(ms.pData, &bufferData, sizeof(bufferData));
            _d3dContext->Unmap(buffer.Get(), NULL);
        }

      public:
        HWND                            _mainWindow;
        int                             _screenWidth;
        int                             _screenHeight;
        bool                            _fullscreen;
        bool                            _enableDepthTest;
        bool                            _enableDepthWrite;
        bool                            _wireframe;


        D3D11_VIEWPORT                  _screenViewport;
        ComPtr<ID3D11Device>            _d3dDevice;
        ComPtr<ID3D11DeviceContext>     _d3dContext;
        ComPtr<IDXGIFactory1>           _dxgiFactory;
        ComPtr<ID3D11RenderTargetView>  _d3dRenderTargetView;
        ComPtr<IDXGISwapChain>          _d3dSwapChain;
        ComPtr<ID3D11RasterizerState>   _d3dRasterizerState;

        ComPtr<ID3D11Texture2D>         _d3dDepthStencilBuffer;
        ComPtr<ID3D11DepthStencilView>  _d3dDepthStencilView;
        ComPtr<ID3D11DepthStencilState> _d3dDepthStencilState;

	};
}