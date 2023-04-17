#include "GameWindow.h"
#include "Renderer/GameRenderer/DxRenderer/MakeDxRenderer.h"

GameWindow::GameWindow(WindowBaseData &baseData)
    : Window(baseData)
{}

GameWindow::~GameWindow() {
}

ProcessMsgResult GameWindow::ProcessMsg(uint32_t msg, WPARAM wparam, LPARAM lparam) {
    ProcessMsgResult res(true);

    switch (msg) {
    case WM_CREATE: {
        GameRenderer::MakeDxRenderer dxRendererFactory;
        this->renderer = std::make_unique<HwndRenderer<SpaceGameRenderer>>(this->GetHwnd(), dxRendererFactory);
        break;
    }
    case WM_SIZE: {
        DirectX::XMFLOAT2 size = this->GetXMFLOAT2FromLParam(lparam);

        if (size.x == 0.0f || size.y == 0.0f) {
            this->renderer->PauseRendering();
        }
        else {
            this->renderer->ContinueRendering();
        }

        this->renderer->Resize(size);

        break;
    }
    case WM_MOUSEMOVE: {
        auto pos = Math::Vector2 { static_cast<float>(LOWORD(lparam)), static_cast<float>(HIWORD(lparam)) };
        this->renderer->MouseMove(pos);
        break;
    }
    case WM_LBUTTONDOWN: {
        auto pos = Math::Vector2{ static_cast<float>(LOWORD(lparam)), static_cast<float>(HIWORD(lparam)) };
        this->renderer->MouseDown(pos);
        break;
    }
    case WM_LBUTTONUP: {
        auto pos = Math::Vector2{ static_cast<float>(LOWORD(lparam)), static_cast<float>(HIWORD(lparam)) };
        this->renderer->MouseUp(pos);
        break;
    }
    default: {
        do {
            res = this->ProcessInput(msg, wparam, lparam);
            if (res.handled) {
                break;
            }

        } while (false);
        break;
    }
    }

    return res;
}

DirectX::XMFLOAT2 GameWindow::GetXMFLOAT2FromLParam(LPARAM lparam) {
    DirectX::XMFLOAT2 float2;

    float2.x = (float)LOWORD(lparam);
    float2.y = (float)HIWORD(lparam);

    return float2;
}

DirectX::XMFLOAT2 GameWindow::GetPointFromLParam(LPARAM lparam) {
    DirectX::XMFLOAT2 float2;

    // when using SetCapture+ReleaseCapture coordinates can be negative when pointer moves behind left-top
    float2.x = (float)(int16_t)LOWORD(lparam);
    float2.y = (float)(int16_t)HIWORD(lparam);

    return float2;
}

ProcessMsgResult GameWindow::ProcessInput(uint32_t msg, WPARAM wparam, LPARAM lparam) {
    ProcessMsgResult res(true);

    switch (msg) {
    case WM_LBUTTONDOWN: {
        /*auto pt = this->GetPointFromLParam(lparam);

        SetCapture(this->GetHwnd());

        this->inputWorker.AddTask(Unique(MakeGenericThreadTask([=]() {
            (*this->renderer)->PointerPressed(pt);
        })));*/
        break;
    }
    case WM_LBUTTONUP: {
        /*auto pt = this->GetPointFromLParam(lparam);

        ReleaseCapture();

        this->inputWorker.AddTask(Unique(MakeGenericThreadTask([=]() {
            (*this->renderer)->PointerReleased(pt);
        })));*/
        break;
    }
    case WM_MOUSEMOVE: {
        /*auto pt = this->GetPointFromLParam(lparam);

        this->inputWorker.AddTask(Unique(MakeGenericThreadTask([=]() {
            (*this->renderer)->PointerMoved(pt);
        })));*/
        break;
    }
    default:
        res.handled = false;
        break;
    }

    return res;
}