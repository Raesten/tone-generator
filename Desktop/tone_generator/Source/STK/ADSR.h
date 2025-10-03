#pragma once
#include "Stk.h"
#include <algorithm>

namespace stk {
    class ADSR {
    public:
        enum State { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
        
        ADSR() : state_(IDLE), output_(0.0f), attackTime_(0.1f), decayTime_(0.1f), 
                 sustainLevel_(0.7f), releaseTime_(0.3f), sampleRate_(44100.0f) {}
        
        void setSampleRate(StkFloat rate) { sampleRate_ = rate; }
        
        void setAllTimes(StkFloat attack, StkFloat decay, StkFloat sustain, StkFloat release) {
            attackTime_ = attack;
            decayTime_ = decay;
            sustainLevel_ = sustain;
            releaseTime_ = release;
        }
        
        void keyOn() {
            state_ = ATTACK;
            // Prevent division by zero
            float attackDenom = attackTime_ * sampleRate_;
            float decayDenom = decayTime_ * sampleRate_;
            attackRate_ = (attackDenom > 0.0f) ? 1.0f / attackDenom : 1.0f;
            decayRate_ = (decayDenom > 0.0f) ? (1.0f - sustainLevel_) / decayDenom : 0.0f;
        }
        
        void keyOff() {
            state_ = RELEASE;
            // Prevent division by zero
            float releaseDenom = releaseTime_ * sampleRate_;
            releaseRate_ = (releaseDenom > 0.0f) ? output_ / releaseDenom : 0.0f;
        }
        
        StkFloat tick() {
            switch (state_) {
                case ATTACK:
                    output_ += attackRate_;
                    if (output_ >= 1.0) {
                        output_ = 1.0;
                        state_ = DECAY;
                    }
                    break;
                case DECAY:
                    output_ -= decayRate_;
                    if (output_ <= sustainLevel_) {
                        output_ = sustainLevel_;
                        state_ = SUSTAIN;
                    }
                    break;
                case SUSTAIN:
                    output_ = sustainLevel_;
                    break;
                case RELEASE:
                    output_ -= releaseRate_;
                    if (output_ <= 0.0) {
                        output_ = 0.0;
                        state_ = IDLE;
                    }
                    break;
                case IDLE:
                    output_ = 0.0;
                    break;
            }
            return output_;
        }
        
    private:
        State state_;
        StkFloat output_;
        StkFloat attackTime_, decayTime_, sustainLevel_, releaseTime_;
        StkFloat attackRate_, decayRate_, releaseRate_;
        StkFloat sampleRate_;
    };
}
