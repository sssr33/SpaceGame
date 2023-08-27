// SpaceGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameWindow.h"

#include <libhelpers\StopWatch.h>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

int main() {
    (void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    srand(static_cast<uint32_t>(time(NULL)));

    // uncomment if need to hide console window
    /*HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);*/

    WindowContainer<GameWindow> window(L"Space Game. pre-alpha");
    window.WaitForClose();

    CoUninitialize();

    return 0;
}
