#include "Oscillator.h"
#include <cmath>
#include <algorithm>

static constexpr float TWO_PI = 6.28318530718f;

Oscillator::Oscillator()
    : frequency(440.0f),
      sampleRate(48000.0f),
      phase(0.0f),
      stage(OFF),
      env(0.0f),
      attack(0.05f),
      decay(0.05f),
      sustain(0.8f),
      release(0.1f),
      releaseStep(0.0f),
      waveform(SINE)
{}

void Oscillator::setSampleRate(float sr) {
    sampleRate = std::max(1.0f, sr);
}

void Oscillator::setWaveform(Waveform w) {
    waveform = w;
}

void Oscillator::setAttack(float seconds)  { attack  = std::max(0.0f, seconds); }
void Oscillator::setDecay(float seconds)   { decay   = std::max(0.0f, seconds); }
void Oscillator::setSustain(float level)   { sustain = std::clamp(level, 0.0f, 1.0f); }
void Oscillator::setRelease(float seconds) { release = std::max(0.0f, seconds); }

void Oscillator::noteOn(float frequencyHz) {
    frequency = frequencyHz;
    stage = ATTACK;
}

void Oscillator::noteOff() {
    if (release <= 0.0f) {
        env = 0.0f;
        stage = OFF;
        return;
    }
    releaseStep = env / (release * sampleRate);
    stage = RELEASE;
}

void Oscillator::setFrequency(float frequencyHz) {
    frequency = std::max(1.0f, frequencyHz);
}

float Oscillator::getFrequency() const {
    return frequency;
}

float Oscillator::computeOscillatorSample() {
    switch (waveform) {
        case SINE:   return std::sin(TWO_PI * phase);
        case SQUARE: return (phase < 0.5f) ? 1.0f : -1.0f;
        case SAW:    return 2.0f * phase - 1.0f;
    }
    return 0.0f;
}

float Oscillator::processSample(float freqHz) {
    const float f = std::max(1.0f, freqHz);
    const float phaseInc = f / sampleRate;

    float osc = computeOscillatorSample();
    phase += phaseInc;
    if (phase >= 1.0f) phase -= 1.0f;

    switch (stage) {
        case ATTACK:
            env += 1.0f / (attack * sampleRate);
            if (env >= 1.0f) { env = 1.0f; stage = DECAY; }
            break;

        case DECAY:
            env -= (1.0f - sustain) / (decay * sampleRate);
            if (env <= sustain) { env = sustain; stage = SUSTAIN; }
            break;

        case SUSTAIN:
            env = sustain;
            break;

        case RELEASE:
            env -= releaseStep;
            if (env <= 0.0f) { env = 0.0f; stage = OFF; }
            break;

        case OFF:
            env = 0.0f;
            break;
    }

    return osc * env * 0.2f;
}

void Oscillator::processBlock(float* outBuffer, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        outBuffer[i] = processSample(frequency);
    }
}
