#ifndef SYNTHVOICE_H
#define SYNTHVOICE_H

class SynthVoice {
public:
    enum Waveform { SINE, SQUARE, SAW };
    enum ADSRStage { ATTACK, DECAY, SUSTAIN, RELEASE, OFF };

    SynthVoice();

    void setSampleRate(float sr);
    void setWaveform(Waveform w);

    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);   // 0–1
    void setRelease(float seconds);

    void noteOn(float frequencyHz);
    void noteOff();

    void processBlock(float* outBuffer, int numSamples);

private:
    float computeOscillatorSample();

    // oscillator
    float frequency;
    float sampleRate;
    float phase;

    // envelope
    ADSRStage stage;
    float env;
    float attack, decay, sustain, release;
    float releaseStep;

    Waveform waveform;
};

#endif
