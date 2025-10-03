#pragma once
#include "Stk.h"

namespace stk {
    class FreeVerb {
    public:
        FreeVerb() : roomSize_(0.5f), damping_(0.5f) {}
        
        void setRoomSize(StkFloat size) { roomSize_ = size; }
        void setDamping(StkFloat damping) { damping_ = damping; }
        
        StkFloat tick(StkFloat input) {
            // Simple reverb approximation
            return input * (1.0f - roomSize_) + input * roomSize_ * 0.3f;
        }
        
    private:
        StkFloat roomSize_;
        StkFloat damping_;
    };
}
