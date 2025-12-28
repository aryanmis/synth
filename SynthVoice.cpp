#include "SynthVoice.h"
#include <cmath>
#include <algorithm>

static constexpr float TWO_PI = 6.28318530718f;

SynthVoice::SynthVoice()
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

void SynthVoice::setSampleRate(float sr) {
    sampleRate = std::max(1.0f, sr);
}

void SynthVoice::setWaveform(Waveform w) {
    waveform = w;
}

void SynthVoice::setAttack(float seconds)  { attack  = std::max(0.0f, seconds); }
void SynthVoice::setDecay(float seconds)   { decay   = std::max(0.0f, seconds); }
void SynthVoice::setSustain(float level)   { sustain = std::clamp(level, 0.0f, 1.0f); }
void SynthVoice::setRelease(float seconds) { release = std::max(0.0f, seconds); }

void SynthVoice::noteOn(float frequencyHz) {
    frequency = frequencyHz;
    stage = ATTACK;
}

void SynthVoice::noteOff() {
    if (release <= 0.0f) {
        env = 0.0f;
        stage = OFF;
        return;
    }

    releaseStep = env / (release * sampleRate);
    stage = RELEASE;
}

float SynthVoice::computeOscillatorSample() {
    float value = 0.0f;

    switch (waveform) {
        case SINE:
            value = std::sin(TWO_PI * phase);
            break;
        case SQUARE:
            value = (phase < 0.5f) ? 1.0f : -1.0f;
            break;
        case SAW:
            value = 2.0f * phase - 1.0f;
            break;
    }

    return value;
}

void SynthVoice::processBlock(float* outBuffer, int numSamples) {
    const float phaseInc = frequency / sampleRate;

    for (int i = 0; i < numSamples; ++i) {

        // oscillator
        float osc = computeOscillatorSample();
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;

        // envelope
        switch (stage) {
            case ATTACK:
                env += 1.0f / (attack * sampleRate);
                if (env >= 1.0f) {
                    env = 1.0f;
                    stage = DECAY;
                }
                break;

            case DECAY:
                env -= (1.0f - sustain) / (decay * sampleRate);
                if (env <= sustain) {
                    env = sustain;
                    stage = SUSTAIN;
                }
                break;

            case SUSTAIN:
                env = sustain;
                break;

            case RELEASE:
                env -= releaseStep;
                if (env <= 0.0f) {
                    env = 0.0f;
                    stage = OFF;
                }
                break;

            case OFF:
                env = 0.0f;
                break;
        }

        outBuffer[i] = osc * env * 0.2f; // output gain safety
    }
}
