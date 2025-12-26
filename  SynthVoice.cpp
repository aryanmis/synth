
#include "SynthVoice.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

// Implementation for SynthVoice

SynthVoice::SynthVoice()
    : target_freq(262.0F),
      sample_rate(48000.0F),
      gate_state(false),
      waveform_type(SINE),
      attack_dur(0.05f),
      decay_dur(0.0F),
      sustain_level(0.08f),
      release_dur(0.02f),
      gate_stage(OFF),
      current_amplitude(0.0F) {}


// Setters
void SynthVoice::setTargetFreq(float freq) {
    target_freq = std::max(0.0F, freq);
}

void SynthVoice::setSampleRate(float sr) {
    sample_rate = std::max(1.0F, sr);
}

void SynthVoice::setGateState(bool gate) {
    gate_state = gate;
}

void SynthVoice::setWaveform(waveform w) {
    waveform_type = w;
}

void SynthVoice::setAttackDur(float sec) {
    attack_dur = std::max(0.0F, sec);
}

void SynthVoice::setDecayDur(float sec) {
    decay_dur = std::max(0.0F, sec);
}

void SynthVoice::setSustainLevel(float decimal_percent) {
    sustain_level = std::clamp(decimal_percent, 0.0F, 1.0F);
}

void SynthVoice::setReleaseDur(float sec) {
    release_dur = std::max(0.0F, sec);
}
void SynthVoice::setGateStage(ADSR_stage stage) {
    gate_stage = stage;
}
void SynthVoice::setCurrentAmplitude(float amplitude) {
    current_amplitude = std::clamp(amplitude, 0.0F, 1.0F);
}
void SynthVoice::setOscSample(float sample) {
    oscSample = sample;
}

float SynthVoice::getTargetFreq() const { return target_freq; }
float SynthVoice::getSampleRate() const { return sample_rate; }
bool SynthVoice::getGateState() const { return gate_state; }
SynthVoice::waveform SynthVoice::getWaveform() const { return waveform_type; }
float SynthVoice::getAttackDur() const { return attack_dur; }
float SynthVoice::getDecayDur() const { return decay_dur; }
float SynthVoice::getSustainLevel() const { return sustain_level; }
float SynthVoice::getReleaseDur() const { return release_dur; }
float SynthVoice::getCurrentAmplitude() const { return current_amplitude; }
SynthVoice::ADSR_stage SynthVoice::getGateStage() const { return gate_stage; }
float SynthVoice::getOscSample() const { return oscSample; }

void SynthVoice::noteOn(float midiNoteNumber) {
    float freq = 440.0F * std::pow(2.0F, (midiNoteNumber - 69.0F) / 12.0F);
    setTargetFreq(freq);
    setGateState(true);
    gate_stage = ATTACK;

}
void SynthVoice::noteOff() {
    setGateState(false);
    gate_stage = RELEASE;
}

void SynthVoice::processBlock(float * outBuffer, int numSamples) {

    float increment_size = 1.0F / sample_rate; // seconds per sample
    for (size_t i = 0; i < numSamples; ++i) {
        if (gate_stage == 0)//attack
        {
            if(attack_dur>0.0F) {
                current_amplitude += (1.0F / (attack_dur / increment_size));
            }
            else {
                current_amplitude = 1.0F;
            }
            if (current_amplitude >= 1.0F) {
                setCurrentAmplitude(1.0F);
                setGateStage(DECAY);
            }
        }
        else if (gate_stage == 1) { //decay
            if(decay_dur > 0.0F) {
                current_amplitude -= ((1.0F - sustain_level) / (decay_dur / increment_size));
            }
            else {
                current_amplitude = sustain_level;
            }
            if (current_amplitude <= sustain_level) {
                setCurrentAmplitude(sustain_level);
                setGateStage(SUSTAIN);
            }
        }
        else if (gate_stage == 2) {
                setCurrentAmplitude(sustain_level);
        }
        else if (gate_stage == 3) { //release
            if(release_dur > 0.0F) {
                current_amplitude -= (sustain_level / (release_dur / increment_size));
            }
            else {
                current_amplitude = 0.0F;
            }
            if (current_amplitude <= 0.0F) {
                setCurrentAmplitude(0.0F);
                setGateStage(OFF); //move to off
            }
        }
        else {
            setCurrentAmplitude(0.0F);
            }

        outBuffer[i] = current_amplitude * oscSample;
        }

    }






    
