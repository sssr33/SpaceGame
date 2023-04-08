// SpaceGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameWindow.h"

#include <libhelpers\StopWatch.h>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

struct MemoryRecord {
    size_t sz;
    void *mem;

    MemoryRecord(size_t sz)
        : sz(sz), mem(nullptr)
    {}

    bool operator<(const MemoryRecord &other) const {
        return this->sz < other.sz;
    }
};

int main() {
    (void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    WindowContainer<GameWindow> window(L"Space Game. pre-alpha");
    window.WaitForClose();

    CoUninitialize();

    return 0;
}
