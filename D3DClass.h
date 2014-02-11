#pragma once

#ifndef D3DCLASS_H
#define D3DCLASS_H

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

class D3DClass {
public:
    D3DClass();
    ~D3DClass();

    bool Initialize( int ScreenWidth, int ScreenHeight, bool VSync_Enabled, HWND hWnd, bool FullScreen, float ScreenDepth, float ScreenNear );
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix( D3DXMATRIX& );
    void GetWorldMatrix     ( D3DXMATRIX& );
    void GetOrthoMatrix     ( D3DXMATRIX& );

    void GetVideoCardInfo( char*, int& );
private:
    D3DClass( const D3DClass& );

    bool    m_VSync_Enabled;
    int     m_VideoCardMemory;
    char    m_VideoCardDescription[128];
    IDXGISwapChain*             m_SwapChain;
    ID3D11Device*               m_Device;
    ID3D11DeviceContext*        m_DeviceContext;
    ID3D11RenderTargetView*     m_RenderTargetView;
    ID3D11Texture2D*            m_DepthStencilBuffer;
    ID3D11DepthStencilState*    m_DepthStencilState;
    ID3D11DepthStencilView*     m_DepthStencilView;
    ID3D11RasterizerState*      m_RasterState;
    D3DXMATRIX                  m_ProjectionMatrix;
    D3DXMATRIX                  m_WorldMatrix;
    D3DXMATRIX                  m_OrthoMatrix;
};

#endif