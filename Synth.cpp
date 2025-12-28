#include "Synth.h"
#include <algorithm>

Synth::Synth() {
    oscillators_.emplace_back();
    oscillators_[0].setSampleRate(sampleRate_);
    oscillators_[0].setWaveform(Oscillator::SAW);
    oscillators_[0].noteOn(masterPitchHz.load());

    // Use LFO module
    lfo_.setSampleRate(sampleRate_);
    lfo_.setWaveform(LFO::SINE);
    lfo_.setFrequency(lfoRateHz.load());
}

void Synth::setSampleRate(float sr) {
    sampleRate_ = std::max(1.0f, sr);
    for (auto& o : oscillators_) o.setSampleRate(sampleRate_);

    // Keep LFO in sync
    lfo_.setSampleRate(sampleRate_);
}

void Synth::applyGuiCommands() {
    SynthCmd c;
    while (cmdQ.pop(c)) {
        if (c.type == SynthCmd::AddOsc) {
            Oscillator o;
            o.setSampleRate(sampleRate_);
            o.setWaveform(Oscillator::SAW);
            o.noteOn(masterPitchHz.load());
            oscillators_.push_back(o);
        }
        else if (c.type == SynthCmd::RemoveOsc) {
            if (c.index >= 0 && c.index < (int)oscillators_.size()) {
                oscillators_.erase(oscillators_.begin() + c.index);
            }
        }
        else if (c.type == SynthCmd::SetOscFreq) {
            if (c.index >= 0 && c.index < (int)oscillators_.size()) {
                oscillators_[c.index].setFrequency(c.value);
            }
        }
    }
}

void Synth::processBlock(float* out, unsigned long nFrames) {
    applyGuiCommands();

    const float pitch    = masterPitchHz.load(std::memory_order_relaxed);
    const float lfoRate  = lfoRateHz.load(std::memory_order_relaxed);
    const float lfoDepth = lfoDepthHz.load(std::memory_order_relaxed);

    // Drive the LFO module from GUI
    lfo_.setFrequency(lfoRate);

    for (unsigned long i = 0; i < nFrames; ++i) {
        // LFO output is [-1, 1]
        const float lfo = lfo_.computeSample() * lfoDepth; // depth in Hz
        float sample = 0.0f;

        for (auto& osc : oscillators_) {
            sample += osc.processSample(pitch + lfo);
        }

        out[i] = sample;
    }
}
