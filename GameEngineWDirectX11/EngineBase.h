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

#include <directxtk/SimpleMath.h>
#include <DirectXCollision.h>
#include "InputSystem.h"
#include "ConstantBuffer.h"

namespace Luna {
	using Microsoft::WRL::ComPtr;
    using DirectX::BoundingSphere;
    using DirectX::SimpleMath::Quaternion;
    using DirectX::SimpleMath::Vector3;
	using std::vector;
	using std::wstring;
	
	class EngineBase {
      public:
        EngineBase();
        virtual ~EngineBase();
        float GetAspectRatio() const { return float(m_screenWidth) / m_screenHeight; }

        int Run();

        virtual bool Initialize();
        virtual void UpdateGUI() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;
        virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        
        void SetGlobalConstants(ComPtr<ID3D11Buffer>& buffer);

      protected:
        bool InitMainWindow();
        bool InitD3D();
        bool InitGUI();
        void CreateBuffers();
        void SetMainViewport();

      public:
        HWND m_mainWindow;
        int m_screenWidth;
        int m_screenHeight;
        bool m_fullscreen;
        bool m_enableDepthTest;
        bool m_enableDepthWrite;
        bool m_wireframe = false;
        bool m_useMSAA = true;
        UINT m_numQualityLevels = 0;

        D3D11_VIEWPORT                  m_screenViewport;

        ComPtr<ID3D11Device>            m_d3dDevice;
        ComPtr<ID3D11DeviceContext>     m_d3dContext;
        ComPtr<IDXGIFactory1>           m_dxgiFactory;
        ComPtr<IDXGISwapChain>          m_d3dSwapChain;
        ComPtr<ID3D11RasterizerState>   m_d3dRasterizerState;
        ComPtr<ID3D11RenderTargetView>  m_d3dBackBufferRTV;
        
        ComPtr<ID3D11Texture2D>         m_d3dDepthStencilBuffer;
        ComPtr<ID3D11DepthStencilView>  m_d3dDepthStencilView;
        ComPtr<ID3D11DepthStencilState> m_d3dDepthStencilState;

        // Resource
        ComPtr<ID3D11Texture2D> m_floatBuffer;
        ComPtr<ID3D11Texture2D> m_resolvedBuffer;
        ComPtr<ID3D11RenderTargetView> m_floatRTV;
        ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
        ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;
        
        GlobalConstants m_globalConstsCPU;
        ComPtr<ID3D11Buffer> m_globalConstsGPU;

        InputState m_inputState;

	};
}