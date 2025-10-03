#pragma once

namespace stk {
    typedef float StkFloat;
    
    class Stk {
    public:
        static void setSampleRate(StkFloat rate) { sampleRate_ = rate; }
        static StkFloat sampleRate() { return sampleRate_; }
    private:
        static StkFloat sampleRate_;
    };
}
