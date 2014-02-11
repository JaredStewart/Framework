#include "InputClass.h"

InputClass::InputClass() {
    m_DirectInput   = nullptr;
    m_Keyboard      = nullptr;
    m_Mouse         = nullptr;
}

InputClass::~InputClass() {}

bool InputClass::Initialize( HINSTANCE hInstance, HWND hWnd, int ScreenWidth, int ScreenHeight ) {
    HRESULT result;

    m_ScreenWidth   = ScreenWidth;
    m_ScreenHeight  = ScreenHeight;
    m_MouseX        = 0;
    m_MouseY        = 0;

    result = DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, nullptr );
    if ( FAILED(result) ) return false;

    result = m_DirectInput->CreateDevice( GUID_SysKeyboard, &m_Keyboard, nullptr );
    if ( FAILED(result) ) return false;
    result = m_Keyboard->SetDataFormat( &c_dfDIKeyboard );
    if ( FAILED(result) ) return false;
    result = m_Keyboard->SetCooperativeLevel( hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
    if ( FAILED(result) ) return false;
    result = m_Keyboard->Acquire();
    if ( FAILED(result) ) return false;

    result = m_DirectInput->CreateDevice( GUID_SysMouse, &m_Mouse, nullptr);
    if ( FAILED(result) ) return false;
    result = m_Mouse->SetDataFormat( &c_dfDIMouse );
    if ( FAILED(result) ) return false;
    result = m_Mouse->SetCooperativeLevel( hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
    if ( FAILED(result) ) return false;
    result = m_Mouse->Acquire();
    if ( FAILED(result) ) return false;

    return true;
}

bool InputClass::Frame() {
    bool result;
    result = ReadKeyboard();
    if ( !result ) return false;
    result = ReadMouse();
    if ( !result ) return false;
    ProcessInput();
    return true;
}

void InputClass::Shutdown() {
    if ( m_Mouse ) {
        m_Mouse->Unacquire();
        m_Mouse->Release();
        m_Mouse = nullptr;
    }
    if ( m_Keyboard ) {
        m_Keyboard->Unacquire();
        m_Keyboard->Release();
        m_Keyboard = nullptr;
    }
    if ( m_DirectInput ) {
        m_DirectInput->Release();
        m_DirectInput = nullptr;
    }
}

bool InputClass::ReadKeyboard() {
    HRESULT result;
    result = m_Keyboard->GetDeviceState( sizeof(m_KeyboardState), (LPVOID)&m_KeyboardState );
    if ( FAILED(result) ) {
        if ( ( result == DIERR_INPUTLOST ) || ( result == DIERR_NOTACQUIRED ) ) {
            m_Keyboard->Acquire();
        } else {
            return false;
        }
    }
    return true;
}

bool InputClass::ReadMouse() {
    HRESULT result;
    result = m_Mouse->GetDeviceState( sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState );
    if ( FAILED(result) ) {
        if ( ( result == DIERR_INPUTLOST ) || ( result == DIERR_NOTACQUIRED ) ) {
            m_Mouse->Acquire();
        } else {
            return false;
        }
    }
    return true;
}

void InputClass::ProcessInput() {
    m_MouseX += m_MouseState.lX;
    m_MouseY += m_MouseState.lY;
    if ( m_MouseX < 0 )                 m_MouseX = 0;
    if ( m_MouseY < 0 )                 m_MouseY = 0;
    if ( m_MouseX > m_ScreenWidth )     m_MouseX = m_ScreenWidth;
    if ( m_MouseY > m_ScreenHeight )    m_MouseY = m_ScreenHeight;
}

bool InputClass::IsKeyDown( unsigned int Key ) {
    if ( m_KeyboardState[Key] & 0x80 ) return true;
    return false;
}

void InputClass::GetMouseLocation( int& MouseX, int& MouseY ) {
    MouseX = m_MouseX;
    MouseY = m_MouseY;
}