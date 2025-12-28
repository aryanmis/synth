#include "LFO.h"
#include <cmath>

static constexpr float TWO_PI = 6.28318530718f;

LFO::LFO() {}

void LFO::setSampleRate(float sr){
    sampleRate = (sr > 1.0f) ? sr : 1.0f;
}

void LFO::setFrequency(float freqHz){
    frequency = freqHz;
}

void LFO::setWaveform(Waveform w){
    waveform = w;
}

void LFO::setParameter(Parameter parameter){
    param = parameter;
}

float LFO::computeSample(){
    float sample = 0.0f;

    switch(waveform){
        case SINE:
            sample = std::sin(TWO_PI * phase);
            break;
        case SQUARE:
            sample = (phase < 0.5f) ? 1.0f : -1.0f;
            break;
        case TRIANGLE:
            sample = 4.0f * std::abs(phase - 0.5f) - 1.0f;
            break;
        case SAW:
            sample = 2.0f * phase - 1.0f;
            break;
    }

    phase += frequency / sampleRate;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample; // [-1, 1]
}
