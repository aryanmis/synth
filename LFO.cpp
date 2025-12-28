#include "LFO.h"
#include <cmath>

void LFO::setSampleRate(float sr){
    sampleRate = sr;
}

void LFO::setFrequency(float freqHz){
    frequency = freqHz;
}

void LFO::setWaveform(Waveform w){
    waveform = w;
}
float LFO::computeSample(){
    float phase = 0.0f;
    float phaseIncrement = frequency / sampleRate;
    float sample = 0.0f;
    
}