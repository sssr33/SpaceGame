#include "HMathCP.h"

namespace H {
    void MathCP::Rotate90CCW(float &x, float &y) {
        /*
        x = 1 y = 0 ==>> x = 0 y = 1
        x = 0 y = 1 ==>> x = -1 y = 0
        x = -1 y = 0 ==>> x = 0 y = -1
        x = 0 y = -1 ==>> x = 1 y = 0
        */
        float tmp = x;
        x = -y;
        y = tmp;
    }

    void MathCP::Rotate90CW(float &x, float &y) {
        /*
        x = 1 y = 0 ==>> x = 0 y = -1
        x = 0 y = -1 ==>> x = -1 y = 0
        x = -1 y = 0 ==>> x = 0 y = 1
        x = 0 y = 1 ==>> x = 1 y = 0
        */
        float tmp = x;
        x = y;
        y = -tmp;
    }
}