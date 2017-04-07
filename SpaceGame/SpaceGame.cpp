// SpaceGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameWindow.h"

int main() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    WindowContainer<GameWindow> window(L"Space Game. pre-alpha");
    window.WaitForClose();

    CoUninitialize();

    return 0;
}
