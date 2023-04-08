#include "Matrix.h"

#include <cassert>

namespace Math {
    Matrix4::Matrix4() noexcept
        : _00(1.f), _01(0.f), _02(0.f), _03(0.f)
        , _10(0.f), _11(1.f), _12(0.f), _13(0.f)
        , _20(0.f), _21(0.f), _22(1.f), _23(0.f)
        , _30(0.f), _31(0.f), _32(0.f), _33(1.f)
    {}

    Matrix4 Matrix4::operator*(const Matrix4& other) const noexcept {
        Vector4 columns[Matrix4::MatrixSize] = {
            other.GetColumn<0>(),
            other.GetColumn<1>(),
            other.GetColumn<2>(),
            other.GetColumn<3>()
        };

        Matrix4 res;

        for (size_t r = 0; r < Matrix4::MatrixSize; ++r) {
            const auto& row = this->r[r];

            for (size_t c = 0; c < Matrix4::MatrixSize; ++c) {
                const auto& column = columns[c];

                res.m[r][c] = row.Dot(column);
            }
        }

        return res;
    }

    Matrix4 Matrix4::Translate(float x, float y, float z /*= 0.f*/, float w /*= 1.f*/) {
        Matrix4 res;

        res._30 = x;
        res._31 = y;
        res._32 = z;
        res._33 = w;

        return res;
    }

    Matrix4 Matrix4::Scale(float x, float y, float z /*= 1.f*/, float w /*= 1.f*/) {
        Matrix4 res;

        res._00 = x;
        res._11 = y;
        res._22 = z;
        res._33 = w;

        return res;
    }
}
