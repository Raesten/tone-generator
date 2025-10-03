#pragma once
#include "Stk.h"
#include <cmath>

namespace stk {
    class Chorus {
    public:
        Chorus() : modDepth_(0.1f), modFrequency_(1.0f), phase_(0.0f), sampleRate_(44100.0f) {}
        
        void setModDepth(StkFloat depth) { modDepth_ = depth; }
        void setModFrequency(StkFloat freq) { modFrequency_ = freq; }
        void setSampleRate(StkFloat rate) { sampleRate_ = rate; }
        
        StkFloat tick(StkFloat input) {
            // Simple chorus approximation
            StkFloat lfo = static_cast<StkFloat>(std::sin(phase_ * 2.0f * M_PI)) * modDepth_;
            phase_ += modFrequency_ / sampleRate_;
            if (phase_ >= 1.0f) phase_ -= 1.0f;
            
            return input * (1.0f + lfo * 0.5f);
        }
        
    private:
        StkFloat modDepth_;
        StkFloat modFrequency_;
        StkFloat phase_;
        StkFloat sampleRate_;
    };
}
