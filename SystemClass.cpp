#include "SystemClass.h"
#include <Windows.h>

SystemClass::SystemClass() {
    m_Input     = nullptr;
    m_Graphics  = nullptr;
}

bool SystemClass::Initialize() {
    int ScreenWidth, ScreenHeight;
    bool result;

    ScreenWidth = ScreenHeight = 0;
    InitializeWindows( ScreenWidth, ScreenHeight );
    m_Input = new InputClass;
    if ( !m_Input ) return false;
    result = m_Input->Initialize(m_hInstance, m_hWnd, ScreenWidth, ScreenHeight);
    if ( !result ) return false;
    m_Graphics = new GraphicsClass;
    if (!m_Graphics ) return false;
    result = m_Graphics->Initialize( ScreenWidth, ScreenHeight, m_hWnd );
    if ( !result ) return false;

    return true;
}

void SystemClass::Run() {
    MSG msg;
    bool done, result;

    done = false;
    while ( !done ) {
        if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        if ( msg.message == WM_QUIT ) done = true;
        else {
            result = Frame();
            if ( !result ) done = true;
        }
        if ( m_Input->IsKeyDown( DIK_ESCAPE ) ) done = true;
    }
}

void SystemClass::Shutdown() {
    if ( m_Graphics ) {
        m_Graphics->Shutdown();
        delete m_Graphics;
        m_Graphics = nullptr;
    }
    if ( m_Input ) {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = nullptr;
    }
    ShutdownWindows();
}

bool SystemClass::Frame() {
    bool result;
    int MouseX, MouseY;
    result = m_Input->Frame();
    if ( !result ) return false;
    m_Input->GetMouseLocation( MouseX, MouseY );
    result = m_Graphics->Frame();
    if ( !result ) return false;
    return true;
}

LRESULT CALLBACK SystemClass::MessageHandler( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) {
    return DefWindowProc( hWnd, Msg, wParam, lParam );
}

void SystemClass::InitializeWindows( int& ScreenWidth, int& ScreenHeight ) {
    WNDCLASSEX WindowClass;
    DEVMODE dmScreenSettings;
    int positionX, positionY;
    positionX = positionY = 0;

    ApplicationHandle = this;
    m_hInstance = GetModuleHandle( nullptr );

    m_ApplicationName = L"Ἀφροδίτη";
    m_FullScreen = true;

    ZeroMemory ( &WindowClass, sizeof( WNDCLASSEX ) );
    WindowClass.style           = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc     = WndProc;
    WindowClass.cbClsExtra      = 0;
    WindowClass.cbWndExtra      = 0;
    WindowClass.hInstance       = m_hInstance;
    WindowClass.hIcon           = LoadIcon( nullptr, IDI_WINLOGO );
    WindowClass.hIconSm         = WindowClass.hIcon;
    WindowClass.hCursor         = LoadCursor( nullptr, IDC_ARROW );
    WindowClass.hbrBackground   = (HBRUSH) GetStockObject( BLACK_BRUSH );
    WindowClass.lpszMenuName    = nullptr;
    WindowClass.lpszClassName   = m_ApplicationName;
    WindowClass.cbSize          = sizeof( WNDCLASSEX );
    RegisterClassEx( &WindowClass );

    ScreenWidth     = GetSystemMetrics( SM_CXSCREEN );
    ScreenHeight    = GetSystemMetrics( SM_CYSCREEN );

    if ( m_FullScreen ) {
        ZeroMemory ( &dmScreenSettings, sizeof( dmScreenSettings ) );
        dmScreenSettings.dmSize         = sizeof( dmScreenSettings );
        dmScreenSettings.dmPelsWidth    = (unsigned long)ScreenWidth;
        dmScreenSettings.dmPelsHeight   = (unsigned long)ScreenHeight;
        dmScreenSettings.dmBitsPerPel   = 32;
        dmScreenSettings.dmFields       = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    }

    m_hWnd = CreateWindowEx( WS_EX_APPWINDOW,
        m_ApplicationName,
        m_ApplicationName,
        //WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        WS_OVERLAPPEDWINDOW,
        positionX, positionY,
        ScreenWidth, ScreenHeight,
        nullptr, nullptr,
        m_hInstance,
        nullptr );

    ShowWindow( m_hWnd, SW_SHOW );
    SetForegroundWindow( m_hWnd );
    SetFocus( m_hWnd );
    //ShowCursor( false );
}

void SystemClass::ShutdownWindows() {
    //ShowCursor( true );
    if ( m_FullScreen ) ChangeDisplaySettings( nullptr, 0 );
    DestroyWindow( m_hWnd );
    m_hWnd = nullptr;
    UnregisterClass( m_ApplicationName, m_hInstance );
    m_hInstance = nullptr;
    ApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam ) {
    switch ( Message ) {
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
          }
    case WM_CLOSE: {
        PostQuitMessage(0);
        return 0;
          }
    default: {
        return ApplicationHandle->MessageHandler( hWnd, Message, wParam, lParam );
             }
    }
}