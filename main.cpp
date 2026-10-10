#include <miniaudio.h>
#include <sokol_app.h>
#include <stdio.h>
#include <cmath>
#include "miniaudio_init.h"

namespace audioWorkshop {
    int currentSampleIndex = 0;
    const int SAMPLE_RATE = 48000;

    void audioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        float* fOutput = (float*)pOutput;
        for(int i = 0; i < frameCount; i++) {
            fOutput[i] = 0.2f * std::sin(2 * 3.14159265f * 220 * (float)currentSampleIndex / SAMPLE_RATE);
            currentSampleIndex++;
        }
        (void)pInput;   // Unused
    }

    audioWorkshop::MiniAudioWrapper miniAudioWrapper(SAMPLE_RATE, audioDataCallback);

    void init() {
    }
    void frame() {
    }
    void cleanup() {
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return sapp_desc{
        .init_cb = audioWorkshop::init,
        .frame_cb = audioWorkshop::frame,
        .cleanup_cb = audioWorkshop::cleanup,
    };
}
