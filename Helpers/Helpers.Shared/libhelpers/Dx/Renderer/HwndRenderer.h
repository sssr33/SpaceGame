#pragma once
#include "HwndOutput.h"

#include <thread>
#include <type_traits>
#include <vector>
#include <functional>
#include <libhelpers/Thread/critical_section.h>
#include <libhelpers/Thread/condition_variable.h>
#include <libhelpers/Dx\Renderer/IRenderer.h>
#include <libhelpers/Math/Vector.h>

template<class T>
class HwndRenderer {
public:
    template<class RendererFactoryT, class... Args>
    HwndRenderer(HWND hwnd, RendererFactoryT rendererFactory, Args&&... args)
        : output(&this->dxDev, hwnd),
        renderer(rendererFactory(&this->dxDev, &this->output), std::forward<Args>(args)...),
        renderThreadState(RenderThreadState::Pause),
        newSize(0.0f, 0.0f), resizeRequested(false)
    {
        this->renderThread = std::thread([=]() { this->RenderProc(); });
    }

    ~HwndRenderer() {
        static_assert(std::is_base_of<IRenderer, T>::value, "Renderer must inherit from IRenderer");

        this->renderThreadState = RenderThreadState::Stop;
        this->renderThreadStateCv.notify();

        if (this->renderThread.joinable()) {
            this->renderThread.join();
        }
    }

    T *operator->() {
        return &this->renderer;
    }

    DxDevice& GetDxDevice() {
        return dxDev;
    }

    void ContinueRendering() {
        thread::critical_section::scoped_lock lk(this->cs);

        this->renderThreadState = RenderThreadState::Work;
        this->renderThreadStateCv.notify();
    }

    void PauseRendering() {
        thread::critical_section::scoped_lock lk(this->cs);

        this->renderThreadState = RenderThreadState::Pause;
    }

    void Resize(const DirectX::XMFLOAT2 &size) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->resizeRequested = true;
        this->newSize = size;
    }

    void MouseMove(const Math::Vector2& pos) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->inputQueue.push_back([this, pos]() {
            this->renderer.MouseMove(pos);
            });
    }

    void MouseDown(const Math::Vector2& pos) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->inputQueue.push_back([this, pos]() {
            this->renderer.MouseDown(pos);
            });
    }

    void MouseUp(const Math::Vector2& pos) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->inputQueue.push_back([this, pos]() {
            this->renderer.MouseUp(pos);
            });
    }

    void KeyDown(KeyboardKey key) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->inputQueue.push_back([this, key]() {
            this->renderer.KeyDown(key);
            });
    }

    void KeyUp(KeyboardKey key) {
        thread::critical_section::scoped_lock lk(this->cs);

        this->inputQueue.push_back([this, key]() {
            this->renderer.KeyUp(key);
            });
    }

private:
    enum class RenderThreadState {
        Work,
        Pause, // can be changed to Work to let thread continue and not exit
        Stop, // thread will exit very soon
    };

    DxDevice dxDev;
    HwndOutput output;
    T renderer;

    thread::critical_section cs;
    thread::condition_variable renderThreadStateCv;
    std::thread renderThread;
    RenderThreadState renderThreadState;

    DirectX::XMFLOAT2 newSize;
    bool resizeRequested;
    std::vector<std::function<void()>> inputQueue;

    void RenderProc() {
        std::vector<std::function<void()>> threadInputQueue;

        while (this->CheckRenderThreadState()) {
            bool resize = false;
            DirectX::XMFLOAT2 size;

            {
                thread::critical_section::scoped_lock lk(this->cs);
                if (this->resizeRequested) {
                    resize = true;
                    size = this->newSize;
                    this->resizeRequested = false;
                }

                std::swap(threadInputQueue, this->inputQueue);
            }

            if (resize) {
                this->output.SetLogicalSize(size);
                this->output.Resize();
                this->renderer.OutputParametersChanged();
            }

            this->output.BeginRender();
            for (const auto& i : threadInputQueue) {
                i();
            }

            this->renderer.Render();
            this->output.EndRender();

            threadInputQueue.clear();
        }
    }

    bool CheckRenderThreadState() {
        bool continueWork = true;
        thread::critical_section::scoped_yield_lock lk(this->cs);

        while (this->renderThreadState == RenderThreadState::Pause) {
            this->renderThreadStateCv.wait(this->cs);
        }

        if (this->renderThreadState == RenderThreadState::Stop) {
            continueWork = false;
        }

        return continueWork;
    }
};