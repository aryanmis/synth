#pragma once
#include <vector>
#include <atomic>
#include "Oscillator.h"
#include "LFO.h"



#ifndef SYNTH_H
#define SYNTH_H

struct SynthCmd {
    enum Type { AddOsc, RemoveOsc, SetOscFreq } type;
    int index;
    float value;
};

template <size_t N>
class SpscRing {
public:
    bool push(const SynthCmd& c) {
        auto w = write_.load(std::memory_order_relaxed);
        auto n = (w + 1) % N;
        if (n == read_.load(std::memory_order_acquire)) return false;
        buf_[w] = c;
        write_.store(n, std::memory_order_release);
        return true;
    }

    bool pop(SynthCmd& c) {
        auto r = read_.load(std::memory_order_relaxed);
        if (r == write_.load(std::memory_order_acquire)) return false;
        c = buf_[r];
        read_.store((r + 1) % N, std::memory_order_release);
        return true;
    }

private:
    SynthCmd buf_[N]{};
    std::atomic<size_t> write_{0}, read_{0};
};

class Synth {
public:
    Synth();

    void setSampleRate(float sr);

    // GUI parameters
    std::atomic<float> masterPitchHz{440.0f};
    std::atomic<float> lfoRateHz{2.0f};
    std::atomic<float> lfoDepthHz{5.0f};

    SpscRing<256> cmdQ;

    void processBlock(float* out, unsigned long nFrames);

private:
    void applyGuiCommands();

    float sampleRate_ = 48000.0f;
    float lfoPhase_ = 0.0f;

    std::vector<Oscillator> oscillators_;
    LFO lfo_;
};

#endif