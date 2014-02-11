#pragma once

#ifndef SYSTEMCLASS_H
#define SYSTEMCLASS_H

#include <Windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass {
public:
    SystemClass();
    ~SystemClass() {}

    bool Initialize();
    void Run();
    void Shutdown();

    LRESULT CALLBACK MessageHandler( HWND, UINT, WPARAM, LPARAM );
private:
    SystemClass( const SystemClass& );
    SystemClass& operator=( const SystemClass& );

    bool Frame();
    void InitializeWindows( int& ScreenWidth, int& ScreenHeight );
    void ShutdownWindows();
private:
    LPCWSTR         m_ApplicationName;
    HINSTANCE       m_hInstance;
    HWND            m_hWnd;
    InputClass*     m_Input;
    GraphicsClass*  m_Graphics;
    bool            m_FullScreen;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;

#endif