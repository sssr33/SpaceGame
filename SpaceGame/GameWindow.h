#pragma once
#include "Renderer\SpaceGameRenderer.h"

#include <libhelpers\Window\Window.h>
#include <libhelpers\Window\WindowContainer.h>
#include <libhelpers\Dx\Renderer\HwndRenderer.h>
#include <libhelpers\Thread\TaskQueueWorker.h>

#include <optional>

class GameWindow : public Window {
public:
    GameWindow(WindowBaseData &baseData);
    virtual ~GameWindow();

    ProcessMsgResult ProcessMsg(uint32_t msg, WPARAM wparam, LPARAM lparam) override;

private:
    DirectX::XMFLOAT2 GetXMFLOAT2FromLParam(LPARAM lparam);
    DirectX::XMFLOAT2 GetPointFromLParam(LPARAM lparam);

    ProcessMsgResult ProcessInput(uint32_t msg, WPARAM wparam, LPARAM lparam);

    static std::optional<KeyboardKey> MapWinKeyboardKey(WPARAM wparam);

    std::unique_ptr<HwndRenderer<SpaceGameRenderer>> renderer;
    TaskQueueWorker inputWorker;
};