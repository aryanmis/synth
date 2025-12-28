#ifndef LFO_H
#define LFO_H

class LFO {
public:
    enum Waveform { SINE, SQUARE, TRIANGLE, SAW };
    enum Parameter { NONE, PITCH, AMP, FILTER };

    LFO();

    void setSampleRate(float sr);
    void setFrequency(float freqHz);
    void setWaveform(Waveform w);
    void setParameter(Parameter parameter);

    float computeSample(); // [-1, 1]

private:
    float sampleRate = 48000.0f;
    float frequency  = 1.0f;
    float phase      = 0.0f;

    Waveform waveform = SINE;
    Parameter param   = NONE;
};

#endif
