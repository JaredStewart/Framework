#pragma once

#ifndef INPUTCLASS_H
#define INPUTCLASS_H

#define DIRECTINPUT_VERSION 0x0800

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

#include <dinput.h>

class InputClass{
public:
    InputClass();
    ~InputClass();

    bool Initialize( HINSTANCE, HWND, int ScreenWidth, int ScreenHeight );
    bool Frame();
    void Shutdown();

    bool IsKeyDown( unsigned int Key );
    void GetMouseLocation( int& MouseX , int& MouseY );
private:
    InputClass( const InputClass& );

    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();
private:
    IDirectInput8*          m_DirectInput;
    IDirectInputDevice8*    m_Keyboard;
    IDirectInputDevice8*    m_Mouse;

    unsigned char           m_KeyboardState[256];
    DIMOUSESTATE            m_MouseState;
    int                     m_ScreenWidth,  m_ScreenHeight;
    int                     m_MouseX,       m_MouseY;
};

#endif