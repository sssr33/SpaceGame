#pragma once

#include <cstdint>

namespace Structs {
    template<class T>
    struct Size {
        T width;
        T height;

        Size() = default;
        Size(T s);
        Size(T width, T height);

        bool operator==(const Size &other) const;
        bool operator!=(const Size &other) const;
    };

    template<class T>
    struct Point {
        T x;
        T y;

        Point() = default;
        Point(T x, T y);

        bool operator==(const Point &other) const;
        bool operator!=(const Point &other) const;
    };

    template<class T>
    struct Rect {
        Point<T> leftTop;
        Size<T> size;

        Rect() = default;
        Rect(const Point<T> &leftTop, const Size<T> &size);

        bool operator==(const Rect &other) const;
        bool operator!=(const Rect &other) const;
    };

    template<class T>
    Size<T>::Size(T s)
        : width(s), height(s)
    {}

    template<class T>
    Size<T>::Size(T width, T height)
        : width(width), height(height)
    {}

    template<class T>
    bool Size<T>::operator==(const Size &other) const {
        bool equWidth = this->width == other.width;
        bool equHeight = this->height == other.height;
        bool equ = equWidth && equHeight;

        return equ;
    }

    template<class T>
    bool Size<T>::operator!=(const Size &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }




    template<class T>
    Point<T>::Point(T x, T y)
        : x(x), y(y)
    {}

    template<class T>
    bool Point<T>::operator==(const Point &other) const {
        bool equX = this->x == other.x;
        bool equY = this->y == other.y;
        bool equ = equX && equY;

        return equ;
    }

    template<class T>
    bool Point<T>::operator!=(const Point &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }




    template<class T>
    Rect<T>::Rect(const Point<T> &leftTop, const Size<T> &size)
        : leftTop(leftTop), size(size)
    {}

    template<class T>
    bool Rect<T>::operator==(const Rect &other) const {
        bool equLeftTop = this->leftTop == other.leftTop;
        bool equSize = this->size == other.size;
        bool equ = equLeftTop && equSize;

        return equ;
    }

    template<class T>
    bool Rect<T>::operator!=(const Rect &other) const {
        bool notEqu = !this->operator==(other);
        return notEqu;
    }
}