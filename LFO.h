class LFO{
public : 
    enum Waveform {SINE, SQUARE, TRIANGLE, SAW};
    enum Parameter {FREQUENCY, AMPLITUDE, PHASE};
    LFO();
    void setSampleRate(float sr);
    void setFrequency(float freqHz);
    void setWaveform(Waveform w);
    void setParameter(Parameter param, float value);
    float computeSample();







private : 
    float frequency;
    float sampleRate;
    Waveform waveform;
    


};