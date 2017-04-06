// SpaceGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameWindow.h"

int main() {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    WindowContainer<GameWindow> window(L"Space Game. pre-alpha");
    window.WaitForClose();
    return 0;
}
