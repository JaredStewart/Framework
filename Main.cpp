#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SystemClass* System = nullptr;
    bool result;

    System = new SystemClass;
    if ( !System ) return 1;
    result = System->Initialize();
    if ( result ) System->Run();
    System->Shutdown();
    delete System;
    System = nullptr;

    return 0;
}