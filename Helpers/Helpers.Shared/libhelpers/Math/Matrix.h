#pragma once
#include "Vector.h"

namespace Math {
    // row-major matrix
    struct Matrix4 {
        static constexpr size_t MatrixSize = 4;

        union
        {
            Vector4 r[Matrix4::MatrixSize];
            struct
            {
                float _00, _01, _02, _03;
                float _10, _11, _12, _13;
                float _20, _21, _22, _23;
                float _30, _31, _32, _33;
            };
            float m[Matrix4::MatrixSize][Matrix4::MatrixSize];
        };

        Matrix4() noexcept;

        Matrix4 operator*(const Matrix4& other) const noexcept;

        template<size_t ColumnIdx>
        Vector4 GetColumn() const noexcept {
            static_assert(ColumnIdx < Matrix4::MatrixSize);
            return { m[0][ColumnIdx], m[1][ColumnIdx], m[2][ColumnIdx], m[3][ColumnIdx] };
        }

        static Matrix4 Translate(float x, float y, float z = 0.f, float w = 1.f);
        static Matrix4 Scale(float x, float y, float z = 1.f, float w = 1.f);
    };
}
