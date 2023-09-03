#pragma once
#include "libhelpers/config.h"
#include "libhelpers/Math/Vector.h"
#include "libhelpers/Dx/Renderer/KeyboardKeyEnum.h"

class IRenderer {
public:
	virtual ~IRenderer() = default;

	virtual void Render() = 0;
	virtual void OutputParametersChanged() = 0;

	virtual void MouseMove(const Math::Vector2& pos) = 0;
	virtual void MouseDown(const Math::Vector2& pos) = 0;
	virtual void MouseUp(const Math::Vector2& pos) = 0;

	virtual void KeyDown(KeyboardKey key) = 0;
	virtual void KeyUp(KeyboardKey key) = 0;
};
