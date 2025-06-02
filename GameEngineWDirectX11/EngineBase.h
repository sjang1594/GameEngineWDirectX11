#pragma once

#include <d3d11.h>
#include <iostream>
#include <vector>
#include <windows.h>

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
        virtual void OnMouseMove(WPARAM btnState, int mouseX, int mouseY);

      protected:
        bool InitMainWindow();
        bool InitD3D();
        bool InitGUI();

        void SetViewport();
        bool CreateRenderTargetView();

      public:
        int m_guiWidth = 0;
        HWND                             m_mainWindow;
        int                              m_screenWidth;
        int                              m_screenHeight;
        bool                             m_fullscreen;
        bool                             m_enableDepthTest;
        bool                             m_enableDepthWrite;
        bool                             m_wireframe;
        UINT                             m_numQualityLevels;
        D3D11_VIEWPORT                   m_screenViewport;
        ComPtr<ID3D11Device>             m_d3dDevice;
        ComPtr<ID3D11DeviceContext>      m_d3dContext;
        ComPtr<IDXGIFactory1>            m_dxgiFactory;
        ComPtr<ID3D11RenderTargetView>   m_d3dRenderTargetView;
        ComPtr<ID3D11ShaderResourceView> m_d3dShareResourceView;
        ComPtr<IDXGISwapChain>           m_d3dSwapChain;
        ComPtr<ID3D11RasterizerState>    m_d3dRasterizerState;
                                        
        ComPtr<ID3D11Texture2D>          m_d3dDepthStencilBuffer;
        ComPtr<ID3D11DepthStencilView>   m_d3dDepthStencilView;
        ComPtr<ID3D11DepthStencilState>  m_d3dDepthStencilState;
        
        bool m_keyPressed[256] = {
            false,
        };

        std::shared_ptr<class Camera> m_camera;
	};
}