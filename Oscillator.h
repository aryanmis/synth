#ifndef OSCILLATOR_H
#define OSCILLATOR_H

// contents of file


class Oscillator {
public:
    enum Waveform { SINE, SQUARE, SAW };
    enum ADSRStage { ATTACK, DECAY, SUSTAIN, RELEASE, OFF };

    Oscillator();

    void setSampleRate(float sr);
    void setWaveform(Waveform w);

    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);

    void noteOn(float frequencyHz);
    void noteOff();

    // GUI-friendly
    void setFrequency(float frequencyHz);
    float getFrequency() const;

    // Block rendering (kept for compatibility)
    void processBlock(float* outBuffer, int numSamples);

    // Sample rendering (needed for LFO modulation)
    float processSample(float modulatedFrequencyHz);

private:
    float computeOscillatorSample();

    float frequency;
    float sampleRate;
    float phase;

    ADSRStage stage;
    float env;
    float attack, decay, sustain, release;
    float releaseStep;

    Waveform waveform;
};

#endif // OSCILLATOR_H

