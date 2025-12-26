#ifndef SYNTHVOICE_H
#define SYNTHVOICE_H

#include <algorithm>

class SynthVoice {
public:
    enum waveform { SINE = 0, SQUARE = 1, SAWTOOTH = 2 };
    enum ADSR_stage { ATTACK = 0, DECAY = 1, SUSTAIN = 2, RELEASE = 3, OFF = 4 };
    SynthVoice();

    // Setters
    void setTargetFreq(float freq);
    void setSampleRate(float sr);
    void setGateState(bool gate);
    void setWaveform(waveform w);
    void setAttackDur(float ms);
    void setDecayDur(float ms);
    void setSustainLevel(float decimal_percent);
    void setReleaseDur(float ms);
    void setOscSample(float sample);
    // Getters (convenience)
    float getTargetFreq() const;
    float getSampleRate() const;
    bool getGateState() const;
    waveform getWaveform() const;
    float getAttackDur() const;
    float getDecayDur() const;
    float getSustainLevel() const;
    float getReleaseDur() const;
    float getOscSample() const;

	void noteOn(float frequency);
	void noteOff();
	void processBlock(float * outBuffer, int numSamples);

    // Additional setters and getters for internal state
    void setGateStage(ADSR_stage stage);
    void setCurrentAmplitude(float amplitude);
    float getCurrentAmplitude() const;
    ADSR_stage getGateStage() const;
    


private:
    // both in Hz
    float target_freq;
    float sample_rate;

    // binary state (envelope handles the rest)
    bool gate_state;

    // waveform type
    waveform waveform_type;

    // durations in seconds
    float attack_dur;
    float decay_dur;
    // percentage (0 -> 1.00 of peak amplitude of wave)
    float sustain_level;
    float release_dur;
    ADSR_stage gate_stage;
    float current_amplitude;
    float oscSample;
};

#endif // SYNTHVOICE_H

