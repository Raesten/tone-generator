#pragma once
#include "Stk.h"
#include <random>

namespace stk {
    class Noise {
    public:
        Noise() : generator_(std::random_device{}()), distribution_(-1.0f, 1.0f), sampleRate_(44100.0f) {}
        
        void setSampleRate(StkFloat rate) { sampleRate_ = rate; }
        
        StkFloat tick() {
            return distribution_(generator_);
        }
        
    private:
        std::mt19937 generator_;
        std::uniform_real_distribution<StkFloat> distribution_;
        StkFloat sampleRate_ = 44100.0f;
    };
}
