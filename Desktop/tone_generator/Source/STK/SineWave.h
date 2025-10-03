#pragma once
#include "Stk.h"
#include <cmath>

namespace stk {
    class SineWave {
    public:
        SineWave() : phase_(0.0f), frequency_(440.0f), sampleRate_(44100.0f) {}
        
        void setFrequency(StkFloat frequency) { frequency_ = frequency; }
        void setSampleRate(StkFloat rate) { 
            // Minimal implementation - just do nothing for now
            (void)rate; // Suppress unused parameter warning
        }
        
        void testMethod() {
            // Absolutely minimal method with no parameters
        }
        
        StkFloat tick() {
            StkFloat output = static_cast<StkFloat>(std::sin(phase_ * 2.0f * M_PI));
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
