#pragma once
#include "Stk.h"
#include <cmath>

namespace stk {
    class BlitSquare {
    public:
        BlitSquare() : phase_(0.0f), frequency_(440.0f), sampleRate_(44100.0f) {}
        
        void setFrequency(StkFloat frequency) { frequency_ = frequency; }
        void setSampleRate(StkFloat rate) { sampleRate_ = rate; }
        
        StkFloat tick() {
            StkFloat output = (phase_ < 0.5) ? 1.0f : -1.0f;
            // Prevent division by zero
            if (sampleRate_ > 0.0f) {
                phase_ += frequency_ / sampleRate_;
                if (phase_ >= 1.0f) phase_ -= 1.0f;
            }
            return output;
        }
        
    private:
        StkFloat phase_;
        StkFloat frequency_;
        StkFloat sampleRate_;
    };
}
