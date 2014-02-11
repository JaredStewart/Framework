#include "GraphicsClass.h"

GraphicsClass::GraphicsClass() {
    m_D3D = nullptr;
}

GraphicsClass::~GraphicsClass() {}

bool GraphicsClass::Initialize( int ScreenWidth, int ScreenHeight, HWND hWnd ) {
    bool result;

    m_D3D = new D3DClass;
    if ( !m_D3D ) return false;
    result = m_D3D->Initialize( ScreenWidth, ScreenHeight, VSyncEnabled, hWnd, FullScreen, ScreenDepth, ScreenNear );
    if (!result ) return false;
    return true;
}

void GraphicsClass::Shutdown() {
    if ( m_D3D ) {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }
}

bool GraphicsClass::Frame() {
    bool result;
    result = Render();
    if ( !result ) return false;
    return true;
}

bool GraphicsClass::Render() {
    m_D3D->BeginScene( 0.5f, 0.5f, 0.5f, 1.0f );
    m_D3D->EndScene();
    return true;
}