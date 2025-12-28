#include <portaudio.h>
#include "Synth.h"
#include "gui.h"

static int audioCallback(
    const void*,
    void* output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void* userData)
{
    auto* synth = static_cast<Synth*>(userData);
    float* out = static_cast<float*>(output);
    synth->processBlock(out, frameCount);
    return paContinue;
}

int main() {
    Pa_Initialize();

    Synth synth;
    synth.setSampleRate(48000.0f);

    PaStream* stream = nullptr;
    Pa_OpenDefaultStream(
        &stream,
        0,
        1,
        paFloat32,
        48000,
        256,
        audioCallback,
        &synth
    );

    Pa_StartStream(stream);

    // GUI runs on main thread (important on macOS)
    runGui(synth);

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
