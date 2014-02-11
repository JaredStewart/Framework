#pragma once

#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H

#include <Windows.h>
#include "D3DClass.h"

const bool FullScreen = false;
const bool VSyncEnabled = true;
const float ScreenDepth = 1000.0f;
const float ScreenNear = 0.1f;

class GraphicsClass {
public:
    GraphicsClass();
    ~GraphicsClass();

    bool Initialize( int, int, HWND );
    bool Frame();
    void Shutdown();
private:
    GraphicsClass( const GraphicsClass& );
    bool Render();
private:
    D3DClass* m_D3D;
};

#endif 