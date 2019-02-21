#pragma once
#include "HMathCP.h"

#include <vector>
#include <utility>

template<class Idx, class T>
struct POTPair {
    Idx idx;
    T val;
};

template<class Idx, class T>
POTPair<Idx, T> MakePOTPair(Idx idx, T val) {
    auto pair = POTPair<Idx, T> { idx, std::move(val) };
    return pair;
}

template<class T>
class POTVector {
public:
    POTPair<size_t, T> Get(size_t idx) const {
        auto pot = H::MathCP::RoundUpPOT(idx);
        auto i = (size_t)std::log2(pot);

        if (idx >= this->vec.size()) {
            return MakePOTPair(pot, T());
        }

        return MakePOTPair(pot, this->vec[idx]);
    }

    POTPair<float, T> Get(float idx) const {
        size_t i = (size_t)std::ceil(idx);
        auto tmp = this->Get(i);
        auto res = MakePOTPair((float)tmp.idx, std::move(tmp.val));
        return res;
    }

    POTPair<double, T> Get(double idx) const {
        size_t i = (size_t)std::ceil(idx);
        auto tmp = this->Get(i);
        auto res = MakePOTPair((double)tmp.idx, std::move(tmp.val));
        return res;
    }

    void Set(POTPair<size_t, T> item) {
        auto pot = H::MathCP::RoundUpPOT(item.idx);
        auto i = (size_t)std::log2(pot);

        if (i >= this->potSize.size()) {
            this->potSize.resize(i + 1);
        }

        this->potSize[i] = std::move(item.val);
    }

    void Set(POTPair<float, T> item) {
        size_t idx = (size_t)std::ceil(item.idx);
        this->Set(MakePOTPair(idx, std::move(item.val)));
    }

    void Set(POTPair<double, T> item) {
        size_t idx = (size_t)std::ceil(item.idx);
        this->Set(MakePOTPair(idx, std::move(item.val)));
    }

private:
    std::vector<T> vec;
};