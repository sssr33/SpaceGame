#pragma once
#include "libhelpers\config.h"

class IRenderer {
public:
	virtual ~IRenderer() = default;

	virtual void Render() = 0;
	virtual void OutputParametersChanged() = 0;
};