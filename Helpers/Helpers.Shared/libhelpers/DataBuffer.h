#pragma once

#include <cstdint>
#include <algorithm>

template<class T>
struct DataBuffer {
    T *data;
    size_t dataByteStep;

    DataBuffer()
        : data(nullptr), dataByteStep(0)
    {}

    DataBuffer(T *data)
        : data(data), dataByteStep(sizeof(T))
    {}

    DataBuffer(T *data, size_t dataByteStep)
        : data(data), dataByteStep(dataByteStep)
    {}

    template<size_t SrcSize>
    void CopyData(const T (&src)[SrcSize]) {
        if (this->dataByteStep == sizeof(T)) {
            std::memcpy(this->data, src, SrcSize);
        }
        else {
            uint8_t *dataPos = reinterpret_cast<uint8_t *>(this->data);

            for (size_t i = 0; i < SrcSize; i++, dataPos += this->dataByteStep) {
                T *dataTmp = reinterpret_cast<T *>(dataPos);
                *dataTmp = src[i];
            }
        }
    }
};