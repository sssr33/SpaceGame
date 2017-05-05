#pragma once

#include <cstdint>
#include <algorithm>

template<class T>
struct DataBuffer {
    T *data;
    size_t dataByteStep;
    size_t dataCount;

    DataBuffer()
        : data(nullptr), dataByteStep(0), dataCount(0)
    {}

    DataBuffer(T *data, size_t dataCount)
        : data(data), dataByteStep(sizeof(T)), dataCount(dataCount)
    {}

    DataBuffer(T *data, size_t dataByteStep, size_t dataCount)
        : data(data), dataByteStep(dataByteStep), dataCount(dataCount)
    {}

    // init from static array
    template<size_t SrcSize> DataBuffer(T(&src)[SrcSize])
        : data(src), dataByteStep(sizeof(T)), dataCount(SrcSize)
    {}

    // init from structured static array
    // src - array of structures
    // srcField - field inside structure
    template<size_t SrcSize, class SrcT> DataBuffer(SrcT(&src)[SrcSize], T *srcField)
        : data(srcField), dataByteStep(sizeof(SrcT)), dataCount(SrcSize)
    {}

    template<size_t SrcSize>
    void CopyData(const T (&src)[SrcSize], size_t dataOffset = 0) {
        if (this->dataByteStep == sizeof(T)) {
            std::memcpy(&this->data[dataOffset], src, SrcSize * sizeof(T));
        }
        else {
            uint8_t *dataPos = reinterpret_cast<uint8_t *>(this->data);

            dataPos += dataOffset * this->dataByteStep;

            for (size_t i = 0; i < SrcSize; i++, dataPos += this->dataByteStep) {
                T *dataTmp = reinterpret_cast<T *>(dataPos);
                *dataTmp = src[i];
            }
        }
    }

    const T &Get(size_t i) const {
        const uint8_t *dataBytes = reinterpret_cast<const uint8_t *>(this->data);
        const T *dataPtr = reinterpret_cast<const T *>(&dataBytes[i * dataByteStep]);
        return *dataPtr;
    }

    void Set(size_t i, const T &v) {
        uint8_t *dataBytes = reinterpret_cast<uint8_t *>(this->data);
        T *dataPtr = reinterpret_cast<T *>(&dataBytes[i * dataByteStep]);
        *dataPtr = v;
    }
};