#pragma once
#include <vector>

namespace Dsp {
    typedef std::vector<float> Params;
    
    class Filter {
    public:
        virtual ~Filter() = default;
        virtual void setParams(const Params& params) = 0;
        virtual void setParam(int index, float value) = 0;
        virtual void process(int numSamples, float* const* channels) = 0;
    };
    
    template<typename DesignClass, int Channels>
    class FilterDesign : public Filter {
    public:
        FilterDesign() {}
        
        void setParams(const Params& params) override {
            if (params.size() >= 3) {
                sampleRate_ = params[0];
                order_ = static_cast<int>(params[1]);
                cutoff_ = params[2];
            }
        }
        
        void setParam(int index, float value) override {
            if (index == 2) cutoff_ = value;
        }
        
        void process(int numSamples, float* const* channels) override {
            // Simple low-pass filter approximation
            for (int sample = 0; sample < numSamples; ++sample) {
                for (int ch = 0; ch < Channels; ++ch) {
                    if (channels[ch]) {
                        float input = channels[ch][sample];
                        float alpha = cutoff_ / (cutoff_ + sampleRate_);
                        lastOutput_[ch] = alpha * input + (1.0f - alpha) * lastOutput_[ch];
                        channels[ch][sample] = lastOutput_[ch];
                    }
                }
            }
        }
        
    private:
        float sampleRate_ = 44100.0f;
        int order_ = 4;
        float cutoff_ = 1000.0f;
        float lastOutput_[Channels] = {0.0f};
    };
    
    namespace Butterworth {
        namespace Design {
            template<int Order>
            class LowPass {};
        }
    }
}
