#include <portaudio.h>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include "SynthVoice.h"

// Bundle multiple voices for the callback
struct ChordSynth {
    static constexpr int kNumVoices = 3;
    SynthVoice voices[kNumVoices];
    std::vector<float> temp; // scratch buffer for mixing
};

static int audioCallback(
    const void*,
    void* output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* userData)
{
    auto* synth = static_cast<ChordSynth*>(userData);
    float* out = static_cast<float*>(output);
    const int n = static_cast<int>(frameCount);

    // Clear output
    std::fill(out, out + n, 0.0f);

    // Ensure temp buffer is large enough
    if (static_cast<int>(synth->temp.size()) < n) {
        synth->temp.assign(n, 0.0f);
    }

    // Render each voice into temp, then sum into out
    for (int v = 0; v < ChordSynth::kNumVoices; ++v) {
        std::fill(synth->temp.begin(), synth->temp.begin() + n, 0.0f);
        synth->voices[v].processBlock(synth->temp.data(), n);

        for (int i = 0; i < n; ++i) {
            out[i] += synth->temp[i];
        }
    }

    // Prevent clipping: simple gain for 3 voices
    const float mixGain = 0.33f; // ~1/3
    for (int i = 0; i < n; ++i) {
        out[i] *= mixGain;
    }

    return paContinue;
}

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Pa_Initialize failed: " << Pa_GetErrorText(err) << "\n";
        return 1;
    }

    ChordSynth synth;

    // Configure voices
    for (int i = 0; i < ChordSynth::kNumVoices; ++i) {
        synth.voices[i].setSampleRate(48000.0f);
        synth.voices[i].setWaveform(SynthVoice::SQUARE);
        // optional: make it more chord-like
        synth.voices[i].setAttack(0.05f);
        synth.voices[i].setDecay(10.f);
        synth.voices[i].setSustain(0.1f);
        synth.voices[i].setRelease(0.2f);
    }

    PaStream* stream = nullptr;
    err = Pa_OpenDefaultStream(
        &stream,
        0,          // no input
        1,          // mono output
        paFloat32,
        48000,
        256,
        audioCallback,
        &synth
    );
    if (err != paNoError) {
        std::cerr << "Pa_OpenDefaultStream failed: " << Pa_GetErrorText(err) << "\n";
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Pa_StartStream failed: " << Pa_GetErrorText(err) << "\n";
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    // Play a chord: A major (A4=440, C#5≈554.37, E5≈659.26)
    synth.voices[0].noteOn(440.0f);
    synth.voices[1].noteOn(442.0f);
    synth.voices[2].noteOn(438.0f);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    synth.voices[0].noteOff();
    synth.voices[1].noteOff();
    synth.voices[2].noteOff();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
