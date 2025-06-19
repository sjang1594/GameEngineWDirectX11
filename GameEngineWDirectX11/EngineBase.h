#pragma once

#include <d3d11.h>
#include <vector>
#include <windows.h>
#include "ConstantBuffer.h"
#include "GraphicsPSO.h"
#include "Camera.h"
#include "PostProcess.h"

namespace Luna {
    using DirectX::BoundingSphere;
    using DirectX::SimpleMath::Quaternion;
	using Microsoft::WRL::ComPtr;
	using std::vector;
	using std::wstring;

    struct CubeMapInfo {
        const wstring &fileName;
        bool isCubeMap;
        ComPtr<ID3D11ShaderResourceView> &srv;
    };
	
	class EngineBase {
      public:
        EngineBase();
        virtual ~EngineBase();

        int Run();
        float GetAspectRatio() const;
        virtual bool Initialize();
        virtual void UpdateGUI() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;

        virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        virtual void OnMouseMove(int mouseX, int mouseY);
        bool UpdateMouseCtrl(const BoundingSphere &boundingSphere, Quaternion &qr,
                             Vector3 &dragTranslation, Vector3 &pickPoint) {}
        
        void SetGlobalConstants(ComPtr<ID3D11Buffer> &globalConstantsGPU);
        void UpdateGlobalConstants(const Vector3 eyeWorld, const Matrix &view,
                                   const Matrix &proj, const Matrix &refl);
        
        void InitCubemaps(const wstring& basePath, 
                          const wstring& envFilename, 
                          const wstring& specularFilename,
                          const wstring& irradianceFilename, 
                          const wstring& brdfFilename);

        void CreateDepthBuffers();
        void SetPipelineState(const GraphicsPSO &pso);
      
      protected:
        bool InitMainWindow();
        bool InitD3D();
        bool InitGUI();
        void SetViewport();
        void CreateBuffers();

      public:
        int  m_guiWidth = 0;
        HWND m_mainWindow;
        int  m_screenWidth;
        int  m_screenHeight;
        bool m_fullscreen;
        bool m_enableDepthTest = true;
        bool m_enableDepthWrite = false;
        bool m_wireframe = false;

        UINT                             m_numQualityLevels;
        D3D11_VIEWPORT                   m_screenViewport;

        ComPtr<ID3D11Device>             m_d3dDevice;
        ComPtr<ID3D11DeviceContext>      m_d3dContext;
        ComPtr<IDXGIFactory1>            m_dxgiFactory;
        ComPtr<IDXGISwapChain>           m_d3dSwapChain;
        
        ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
        ComPtr<ID3D11Texture2D> m_floatBuffer;
        ComPtr<ID3D11Texture2D> m_resolvedBuffer;
        ComPtr<ID3D11RenderTargetView> m_floatRTV;
        ComPtr<ID3D11RenderTargetView> m_resolvedRTV;
        ComPtr<ID3D11ShaderResourceView> m_floatSRV;
        ComPtr<ID3D11ShaderResourceView> m_resolvedSRV;

        // Depth Buffer Related 
        ComPtr<ID3D11DepthStencilView> m_d3dDepthStencilView;
        
        // ConstantBuffer
        GlobalConstants m_globalConstsCPU;
        ComPtr<ID3D11Buffer> m_globalConstsGPU;

        // Env Mapping
        ComPtr<ID3D11ShaderResourceView> m_envSRV;
        ComPtr<ID3D11ShaderResourceView> m_diffuseSRV; // = irridiance
        ComPtr<ID3D11ShaderResourceView> m_specularSRV;
        ComPtr<ID3D11ShaderResourceView> m_brdfSRV;

        // Mouse
        float m_cursor_ndc_x = 0.0f;
        float m_cursor_ndc_y = 0.0f;
        bool m_keyPressed[256] = {
            false,
        };

        // Post Processing
        PostProcess m_postProcess;

        // Camera
        std::shared_ptr<Camera> m_camera;
	};
}